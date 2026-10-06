"""Miles AIL 2.x XMIDI/XDIR -> Standard MIDI File conversion.

Authoritative behavior sources:
- Original Miles Design MIDIFORM.C writer (1991/1992)
- Original Miles AIL XMIDI.ASM runtime
- Standard MIDI File timing/event rules from The MIDI Association

The original XMI remains canonical. The SMF derivative uses SMPTE timing:
30 frames/second * 4 ticks/frame = exactly 120 ticks/second, matching the
default Ravenloft XMIDI quantization without tempo-dependent rescaling.
"""
from __future__ import annotations

from dataclasses import dataclass
from typing import Iterable


class XmiError(RuntimeError):
    pass


@dataclass(frozen=True)
class XmiSequence:
    evnt: bytes
    timb: bytes | None
    rbrn: bytes | None


@dataclass(frozen=True)
class MidiConversionInfo:
    sequence_index: int
    end_tick: int
    event_count: int
    loop_for_count: int
    loop_next_count: int
    branch_count: int


def _be32(data: bytes, offset: int) -> int:
    if offset + 4 > len(data):
        raise XmiError("truncated IFF 32-bit length")
    return int.from_bytes(data[offset:offset + 4], "big")


def _iff_children(data: bytes, start: int, end: int):
    pos = start
    while pos + 8 <= end:
        tag = data[pos:pos + 4]
        length = _be32(data, pos + 4)
        payload_start = pos + 8
        payload_end = payload_start + length
        if payload_end > end:
            raise XmiError(
                f"IFF chunk {tag!r} at 0x{pos:X} exceeds parent boundary"
            )
        yield pos, tag, payload_start, payload_end
        pos = payload_end + (length & 1)
    if pos != end:
        raise XmiError(f"IFF container has {end - pos} trailing byte(s)")


def parse_xmi_sequences(data: bytes) -> list[XmiSequence]:
    sequences: list[XmiSequence] = []

    def walk(start: int, end: int) -> None:
        for _pos, tag, payload_start, payload_end in _iff_children(data, start, end):
            if tag not in (b"FORM", b"CAT "):
                continue
            if payload_end - payload_start < 4:
                raise XmiError(f"short IFF container {tag!r}")
            form_type = data[payload_start:payload_start + 4]
            children_start = payload_start + 4

            if tag == b"FORM" and form_type == b"XMID":
                evnt = timb = rbrn = None
                for _cpos, ctag, cstart, cend in _iff_children(
                    data, children_start, payload_end
                ):
                    payload = data[cstart:cend]
                    if ctag == b"EVNT":
                        if evnt is not None:
                            raise XmiError("FORM XMID contains more than one EVNT chunk")
                        evnt = payload
                    elif ctag == b"TIMB":
                        timb = payload
                    elif ctag == b"RBRN":
                        rbrn = payload
                if evnt is None:
                    raise XmiError("FORM XMID is missing its mandatory EVNT chunk")
                sequences.append(XmiSequence(evnt, timb, rbrn))
            else:
                walk(children_start, payload_end)

    walk(0, len(data))
    if not sequences:
        raise XmiError("no FORM XMID sequence found")
    return sequences


def _read_vlq(data: bytes, pos: int) -> tuple[int, int]:
    value = 0
    for _ in range(4):
        if pos >= len(data):
            raise XmiError("truncated variable-length number")
        byte = data[pos]
        pos += 1
        value = (value << 7) | (byte & 0x7F)
        if not (byte & 0x80):
            return value, pos
    raise XmiError("variable-length number exceeds four bytes")


def _write_vlq(value: int) -> bytes:
    if not 0 <= value <= 0x0FFFFFFF:
        raise XmiError(f"MIDI VLQ value out of range: {value}")
    encoded = [value & 0x7F]
    value >>= 7
    while value:
        encoded.append(0x80 | (value & 0x7F))
        value >>= 7
    return bytes(reversed(encoded))


def _rbrn_count(payload: bytes | None) -> int:
    if not payload:
        return 0
    if len(payload) < 2:
        raise XmiError("RBRN chunk is truncated")
    count = int.from_bytes(payload[:2], "little")
    expected = 2 + count * 6
    if len(payload) != expected:
        raise XmiError(
            f"RBRN size mismatch: count {count} requires {expected} bytes, "
            f"found {len(payload)}"
        )
    return count


def _event_stream_to_smf_events(
    sequence: XmiSequence,
) -> tuple[list[tuple[int, int, int, bytes]], MidiConversionInfo]:
    data = sequence.evnt
    pos = 0
    tick = 0
    order = 0
    end_tick: int | None = None
    events: list[tuple[int, int, int, bytes]] = []
    note_offs: list[tuple[int, int, int, bytes]] = []
    loop_for_count = 0
    loop_next_count = 0

    while pos < len(data):
        delay = 0
        while pos < len(data) and data[pos] < 0x80:
            delay += data[pos]
            pos += 1
        tick += delay
        if pos >= len(data):
            if delay:
                raise XmiError("EVNT ends after an interval count without an event")
            break

        status = data[pos]
        pos += 1
        family = status & 0xF0
        raw: bytes | None

        if family in (0x80, 0xA0, 0xB0, 0xE0):
            if pos + 2 > len(data):
                raise XmiError("truncated three-byte channel event")
            d1, d2 = data[pos], data[pos + 1]
            pos += 2
            raw = bytes((status, d1, d2))
            if family == 0xB0:
                if d1 == 116:
                    loop_for_count += 1
                elif d1 == 117:
                    loop_next_count += 1

        elif family == 0x90:
            if pos + 2 > len(data):
                raise XmiError("truncated XMIDI note-on event")
            note, velocity = data[pos], data[pos + 1]
            pos += 2
            duration, pos = _read_vlq(data, pos)
            raw = bytes((status, note, velocity))

            # Miles XMIDI.ASM stores duration-1 and expires notes when the
            # signed countdown becomes negative. A serialized duration of zero
            # therefore still sounds for one 120-Hz service interval.
            off_tick = tick + max(1, duration)
            note_offs.append(
                (off_tick, 0, order, bytes((0x80 | (status & 0x0F), note, 0)))
            )

        elif family in (0xC0, 0xD0):
            if pos >= len(data):
                raise XmiError("truncated two-byte channel event")
            raw = bytes((status, data[pos]))
            pos += 1

        elif status in (0xF0, 0xF7):
            length, length_end = _read_vlq(data, pos)
            vlq = data[pos:length_end]
            pos = length_end
            if pos + length > len(data):
                raise XmiError("truncated XMIDI system-exclusive event")
            payload = data[pos:pos + length]
            pos += length
            raw = bytes((status,)) + vlq + payload

        elif status == 0xFF:
            if pos >= len(data):
                raise XmiError("truncated XMIDI meta-event type")
            meta_type = data[pos]
            pos += 1
            length, length_end = _read_vlq(data, pos)
            vlq = data[pos:length_end]
            pos = length_end
            if pos + length > len(data):
                raise XmiError("truncated XMIDI meta-event payload")
            payload = data[pos:pos + length]
            pos += length

            if meta_type == 0x2F:
                if length != 0:
                    raise XmiError("XMIDI end-of-track meta-event has nonzero length")
                end_tick = tick
                raw = None
            else:
                raw = bytes((0xFF, meta_type)) + vlq + payload

        else:
            raise XmiError(f"unsupported XMIDI status byte 0x{status:02X}")

        if raw is not None:
            # Priority 1: normal serialized events. Note-offs use priority 0,
            # reproducing the AIL service order at an interval boundary.
            events.append((tick, 1, order, raw))
        order += 1

    events.extend(note_offs)
    last_event_tick = max((event[0] for event in events), default=0)
    if end_tick is None:
        end_tick = last_event_tick
    elif last_event_tick > end_tick:
        raise XmiError(
            f"scheduled note-off at tick {last_event_tick} exceeds EOT tick {end_tick}"
        )

    # EOT is always emitted last at its serialized absolute interval.
    events.append((end_tick, 2, order, b"\xFF\x2F\x00"))
    events.sort(key=lambda item: (item[0], item[1], item[2]))

    info = MidiConversionInfo(
        sequence_index=-1,
        end_tick=end_tick,
        event_count=len(events),
        loop_for_count=loop_for_count,
        loop_next_count=loop_next_count,
        branch_count=_rbrn_count(sequence.rbrn),
    )
    return events, info


def _build_smf0(events: Iterable[tuple[int, int, int, bytes]]) -> bytes:
    track = bytearray()
    previous_tick = 0
    for tick, _priority, _order, raw in events:
        if tick < previous_tick:
            raise XmiError("MIDI events are not in chronological order")
        track.extend(_write_vlq(tick - previous_tick))
        track.extend(raw)
        previous_tick = tick

    # Standard MIDI File SMPTE division:
    # high byte E2h == signed -30 frames/second, low byte 04h == 4 ticks/frame.
    # 30 * 4 = exactly 120 ticks/second, the Miles MIDIFORM default quantum.
    division = 0xE204
    header = (
        b"MThd"
        + (6).to_bytes(4, "big")
        + (0).to_bytes(2, "big")
        + (1).to_bytes(2, "big")
        + division.to_bytes(2, "big")
    )
    return header + b"MTrk" + len(track).to_bytes(4, "big") + bytes(track)


def convert_all(data: bytes) -> tuple[list[bytes], list[MidiConversionInfo]]:
    midi_files: list[bytes] = []
    infos: list[MidiConversionInfo] = []
    for index, sequence in enumerate(parse_xmi_sequences(data)):
        events, info = _event_stream_to_smf_events(sequence)
        midi_files.append(_build_smf0(events))
        infos.append(
            MidiConversionInfo(
                sequence_index=index,
                end_tick=info.end_tick,
                event_count=info.event_count,
                loop_for_count=info.loop_for_count,
                loop_next_count=info.loop_next_count,
                branch_count=info.branch_count,
            )
        )
    return midi_files, infos
