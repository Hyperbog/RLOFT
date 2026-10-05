# SAVE.DAT

Status: source captured; strong executable-side evidence already exists in reconstructed CODE.2 save/load routines.

- Size: 9,288 bytes
- SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Begins with the NUL-terminated description `QUICK START PARTY; ELVEN F/M - DWARVEN F/C` followed by binary state.

Goal: map the serializer/deserializer field-by-field against CODE.2, preserve legacy/deferred record transformations exactly, identify embedded party/world state, and implement byte-exact parse/write verification against this sample.