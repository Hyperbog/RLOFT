#include "rloft/dev_session_trace.hpp"

#include <array>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

namespace rloft {
namespace {
void write_bytes(const std::filesystem::path& path,
                 const std::uint8_t* bytes,std::size_t size) {
    std::ofstream out(path,std::ios::binary|std::ios::trunc);
    if(!out || !out.write(reinterpret_cast<const char*>(bytes),
                          static_cast<std::streamsize>(size)))
        throw std::runtime_error("unable to write diagnostic frame");
}
std::string number(std::size_t index) {
    std::ostringstream out;
    out<<std::setw(4)<<std::setfill('0')<<index;
    return out.str();
}
} // namespace

DevSessionTrace::DevSessionTrace(
    const std::filesystem::path& directory,
    std::string_view source_revision,
    std::string_view source_kind)
    :directory_(directory) {
    std::filesystem::create_directories(directory_);
    log_.open(directory_/"events.jsonl",
              std::ios::binary|std::ios::trunc);
    if(!log_)throw std::runtime_error(
        "unable to create diagnostic event log");
    std::ofstream manifest(directory_/"manifest.json",
                           std::ios::binary|std::ios::trunc);
    if(!manifest)throw std::runtime_error(
        "unable to create diagnostic manifest");
    manifest<<"{\n"
            <<"  \"format\": \"rloft-level22-diagnostic-v1\",\n"
            <<"  \"provenance\": \"native-preview-not-original-DOS\",\n"
            <<"  \"source_revision\": \""
            <<escape_json(source_revision)<<"\",\n"
            <<"  \"media_kind\": \""
            <<escape_json(source_kind)<<"\",\n"
            <<"  \"framebuffer\": \"320x400-indexed\",\n"
            <<"  \"palette\": \"256x3-raw-6bit\",\n"
            <<"  \"cursor\": \"host-overlay-not-in-indexed-frame\",\n"
            <<"  \"events_max\": "<<kMaxEvents<<",\n"
            <<"  \"snapshots_max\": "<<kMaxSnapshots<<",\n"
            <<"  \"markers_max\": "<<kMaxMarkers<<"\n"
            <<"}\n";
    if(!manifest)throw std::runtime_error(
        "unable to write diagnostic manifest");
    event("session.start");
}

DevSessionTrace::~DevSessionTrace() {
    try { finish(); } catch(...) {}
}

std::string DevSessionTrace::escape_json(std::string_view s) {
    std::string output;
    output.reserve(s.size()+8U);
    constexpr char digits[]="0123456789abcdef";
    for(const auto ch:s) {
        const auto c=static_cast<unsigned char>(ch);
        if(c=='"' || c=='\\') {
            output.push_back('\\');
            output.push_back(static_cast<char>(c));
        }else if(c=='\n')output+="\\n";
        else if(c=='\r')output+="\\r";
        else if(c=='\t')output+="\\t";
        else if(c<0x20U) {
            output+="\\u00";
            output.push_back(digits[c>>4U]);
            output.push_back(digits[c&0x0FU]);
        }else output.push_back(static_cast<char>(c));
    }
    return output;
}

void DevSessionTrace::append(std::string line) {
    if(events_>=kMaxEvents)return;
    log_<<line<<"\n";
    log_.flush(); // Preserve the last event on abrupt native failure.
    if(!log_)throw std::runtime_error(
        "unable to append diagnostic event");
    if(recent_.size()>=kRecentEvents)recent_.pop_front();
    recent_.push_back(std::move(line));
    ++events_;
}

void DevSessionTrace::event(
    std::string_view name,std::int64_t a,std::int64_t b) {
    if(finished_)return;
    // Every record includes a monotonic logical event order, not
    // wall-clock timestamps or local user information.
    std::ostringstream line;
    line<<"{\"seq\":"<<++sequence_<<",\"event\":\""
        <<escape_json(name)<<"\",\"a\":"<<a
        <<",\"b\":"<<b<<"}";
    append(line.str());
    dirty_=true;
}

void DevSessionTrace::indexed_draw(const IndexedDrawAuditCall& call) {
    if(finished_)return;
    if(drawn_this_present_.size()<kMaxDrawsPerPresent)
        drawn_this_present_.push_back(call);
    else ++draws_discarded_;
}

void DevSessionTrace::request_marker() {
    if(!finished_ && markers_<kMaxMarkers) {
        // Marker has its OWN 16-slot budget. Do not accidentally
        // consume an automatic screenshot just for pressing F12.
        const bool prior_input_dirty=dirty_;
        pending_marker_=true;
        event("f12.marker.request");
        dirty_=prior_input_dirty;
    }
}

void DevSessionTrace::capture(
    std::string_view screen,
    const IndexedFramebuffer& framebuffer,
    const Palette6& palette,
    const DevLevelSnapshot& snapshot,
    std::uint32_t irq_tick,
    bool is_marker) {
    if(!is_marker && snapshots_>=kMaxSnapshots)return;
    if(!is_marker && (screen=="viewport" || screen=="top_edge_menu") &&
       viewport_snapshots_>=kMaxViewportSnapshots)return;
    if(is_marker && markers_>=kMaxMarkers)return;
    const auto& pixels=framebuffer.pixels();
    if(pixels.size()!=kFramebufferBytes)
        throw std::invalid_argument("diagnostic frame not 320x400");

    const auto hash=[](const std::uint8_t* data,std::size_t size) {
        std::uint64_t value=UINT64_C(14695981039346656037);
        for(std::size_t i=0;i<size;++i) {
            value^=data[i];
            value*=UINT64_C(1099511628211);
        }
        return value;
    };
    const auto pixel_hash=hash(pixels.data(),pixels.size());
    const auto palette_hash=hash(palette.rgb.data(),palette.rgb.size());
    const auto id=number(is_marker?markers_+1U:snapshots_+1U);
    const auto stem=(is_marker?"marker_":"frame_")+id;
    const auto path=directory_/stem;
    write_bytes(path.string()+".idx",pixels.data(),pixels.size());
    write_bytes(path.string()+".pal",
                palette.rgb.data(),palette.rgb.size());
    std::ofstream metadata(path.string()+".json",
                           std::ios::binary|std::ios::trunc);
    if(!metadata)throw std::runtime_error(
        "unable to create frame metadata");
    metadata<<"{\"format\":\"rloft-trace-frame-v1\","
            <<"\"screen\":\""<<escape_json(screen)<<"\","
            <<"\"irq\":"<<irq_tick<<","
            <<"\"level\":"<<snapshot.level_id<<","
            <<"\"camera_a\":"<<snapshot.coordinate_a<<","
            <<"\"camera_b\":"<<snapshot.coordinate_b<<","
            <<"\"facing\":"<<snapshot.facing<<","
            <<"\"time\":"<<snapshot.time_of_day<<","
            <<"\"actors\":"<<snapshot.active_actors<<","
            <<"\"npcs\":"<<snapshot.npc_count<<","
            <<"\"objects\":"<<snapshot.placed_object_count<<","
            <<"\"loose_items\":"<<snapshot.loose_item_count<<","
            <<"\"containers_open\":"<<snapshot.active_container_views<<","
            <<"\"flags_set\":"<<snapshot.set_story_flags<<","
            <<"\"hash_camera\":"<<snapshot.camera_hash<<","
            <<"\"hash_party\":"<<snapshot.party_hash<<","
            <<"\"hash_inventory\":"<<snapshot.inventory_hash<<","
            <<"\"hash_world\":"<<snapshot.world_hash<<","
            <<"\"hash_flags\":"<<snapshot.flags_hash<<","
            <<"\"hash_pixels\":"<<pixel_hash<<","
            <<"\"hash_palette\":"<<palette_hash<<"}\n";
    if(!metadata)throw std::runtime_error(
        "unable to write frame metadata");

    // Render-order audit for the exact indexed framebuffer API:
    // captures original modal sprite placements, fills, scaled draws
    // and backing-rectangle restores. Separate CODE.2 RES3/SINTAB
    // per-column traces live in renderer/*.tsv and are not folded
    // into this higher-level call list.
    std::ofstream calls(path.string()+".draws.tsv",
                        std::ios::binary|std::ios::trunc);
    if(!calls)throw std::runtime_error(
        "unable to create native draw audit");
    calls<<"order\top\tframe\tx\ty\twidth\theight"
         <<"\tparameter_a\tparameter_b\n";
    for(std::size_t i=0U;i<drawn_this_present_.size();++i) {
        const auto& call=drawn_this_present_[i];
        calls<<i<<"\t"<<call.operation
             <<"\t"<<call.frame
             <<"\t"<<call.x<<"\t"<<call.y
             <<"\t"<<call.width<<"\t"<<call.height
             <<"\t"<<call.parameter_a
             <<"\t"<<call.parameter_b<<"\n";
    }
    calls<<"omitted\t"<<draws_discarded_<<"\n";
    if(!calls)throw std::runtime_error(
        "unable to write native draw audit");

    if(is_marker) {
        std::ofstream recent(directory_/(stem+"_recent.jsonl"),
                             std::ios::binary|std::ios::trunc);
        if(!recent)throw std::runtime_error(
            "unable to create recent-event capture");
        for(const auto& line:recent_)recent<<line<<"\n";
        if(!recent)throw std::runtime_error(
            "unable to write recent events");
    }
    if(is_marker) ++markers_;
    else {
        ++snapshots_;
        if(screen=="viewport" || screen=="top_edge_menu")
            ++viewport_snapshots_;
    }
    last_captured_tick_=irq_tick;
    // Markers and automatic snapshots are indexed as event records.
    event(is_marker?"frame.marker":"frame.capture",
          static_cast<std::int64_t>(
              is_marker?markers_:snapshots_),
          static_cast<std::int64_t>(snapshot.level_id));
}

void DevSessionTrace::observe(
    std::string_view screen,
    const IndexedFramebuffer& framebuffer,
    const Palette6& palette,
    const DevLevelSnapshot& snapshot,
    std::uint32_t irq_tick) {
    if(finished_)return;
    const std::string current(screen);
    const bool changed_screen=!observed_ || current!=last_screen_;
    const bool changed_state=!observed_ ||
        last_camera_!=snapshot.camera_hash ||
        last_party_!=snapshot.party_hash ||
        last_inventory_!=snapshot.inventory_hash ||
        last_world_!=snapshot.world_hash ||
        last_flags_!=snapshot.flags_hash;
    // The native time-of-day field can change every gameplay tick.
    // Recording each state change at ~70Hz would use the entire frame
    // budget before the first user interaction. Always preserve screen
    // transitions and explicit user input; throttle other state/pose
    // changes to >=9 BIOS IRQ ticks (~0.5s). F12 bypasses that cap.
    const bool meaningful=changed_screen || dirty_ ||
        (changed_state && static_cast<std::uint32_t>(
            irq_tick-last_captured_tick_)>=9U);
    if(changed_screen)
        event("screen.enter",
              static_cast<std::int64_t>(snapshot.level_id));
    if(changed_state && meaningful) {
        event("state.changed",
              static_cast<std::int64_t>(snapshot.level_id),
              static_cast<std::int64_t>(snapshot.loose_item_count));
    }
    if(meaningful) {
        capture(current,framebuffer,palette,snapshot,irq_tick,false);
    }
    if(pending_marker_) {
        capture(current,framebuffer,palette,snapshot,irq_tick,true);
        pending_marker_=false;
    }
    observed_=true;
    last_screen_=current;
    last_camera_=snapshot.camera_hash;
    last_party_=snapshot.party_hash;
    last_inventory_=snapshot.inventory_hash;
    last_world_=snapshot.world_hash;
    last_flags_=snapshot.flags_hash;
    dirty_=false;
    drawn_this_present_.clear();
    draws_discarded_=0U;
}

void DevSessionTrace::error(
    std::string_view stage,std::string_view message) {
    if(finished_)return;
    // Diagnostic errors can be returned by resource decoders: trim to
    // 160 chars to bound output. Avoid serializing long local paths.
    const auto safe_stage=escape_json(stage.substr(0,64));
    std::ostringstream record;
    record<<"{\"seq\":"<<++sequence_
          <<",\"event\":\"error\",\"stage\":\""
          <<safe_stage<<"\",\"reason\":\""
          <<escape_json(
              (message.find('/')!=std::string_view::npos ||
               message.find('\\')!=std::string_view::npos)
                  ? std::string_view("path-containing-detail-redacted")
                  : message.substr(0,160))
          <<"\"}";
    append(record.str());
    log_.flush();
}

void DevSessionTrace::finish() {
    if(finished_)return;
    event("session.finish",
          static_cast<std::int64_t>(snapshots_),
          static_cast<std::int64_t>(markers_));
    finished_=true;
    log_.flush();
}
} // namespace rloft
