#include "rloft/original_automap_file_export.hpp"
#include "rloft/automap_presentation.hpp"
#include "rloft/automap_annotations.hpp"
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace rloft {
namespace {
void write_layer(std::vector<std::uint8_t>& out,
                 const std::vector<std::uint8_t>& layer) {
    out.insert(out.end(),layer.begin(),layer.end());
}
void write_newline(std::vector<std::uint8_t>& out) {
    out.push_back(13U);
    out.push_back(10U);
}
void write_legend(std::vector<std::uint8_t>& out,
                  const LevelState& level) {
    // CODE.2 2474 prints player annotations after the map.
    // Preserve the exact stored byte payload (including CR).
    const auto annotations=AutomapAnnotationModel::parse(
        level.automap_annotations);
    for(std::size_t i=0;i<annotations.size();++i) {
        const auto label=AutomapAnnotationModel::label_for_index(i);
        out.push_back(static_cast<std::uint8_t>(label.first));
        if(label.second!=' ')
            out.push_back(static_cast<std::uint8_t>(label.second));
        out.push_back(static_cast<std::uint8_t>(':'));
        out.insert(out.end(),annotations[i].payload.begin(),
                   annotations[i].payload.end());
        write_newline(out);
    }
}
std::vector<AutomapExportColumn> checked_columns(
    const LevelState& level) {
    auto columns=AutomapExportModel::columns(level);
    for(const auto& col:columns) {
        if(col.label_write_overflow)
            throw std::runtime_error(
                "CODE.2 automap label would overflow DOS row buffer");
    }
    return columns;
}
} // namespace

std::string OriginalAutomapFileExport::source_filename(int id) {
    if(id<0 || id>999)
        throw std::out_of_range("CODE.2 automap 3-digit level ID");
    // CODE.2 02A34 physically writes HUNDREDS, ONES, TENS
    // into DGROUP 5FE7..5FE9. Do not normalize to %03d.
    std::string filename="AUTOMAPS/AUTO";
    filename.push_back(static_cast<char>('0'+(id/100)%10));
    filename.push_back(static_cast<char>('0'+id%10));
    filename.push_back(static_cast<char>('0'+(id/10)%10));
    filename+=".MAP";
    return filename;
}

std::vector<std::uint8_t> OriginalAutomapFileExport::merged_02a34(
    const LevelState& level) {
    const auto columns=checked_columns(level);
    std::vector<std::uint8_t> out;
    for(const auto& col:columns) {
        write_layer(out,col.merged);
        write_newline(out);
    }
    write_legend(out,level);
    return out;
}

std::vector<std::uint8_t> OriginalAutomapFileExport::printer_03321(
    const LevelState& level) {
    const auto columns=checked_columns(level);
    std::vector<std::uint8_t> out;
    for(const auto& col:columns) {
        // CODE.2 026E8 in nonmerged mode writes all three layers,
        // returning the DOS printer head to the SAME LINE via CR.
        // A final CR/LF advances to the next physical line.
        write_layer(out,col.base);
        out.push_back(13U);
        write_layer(out,col.placed);
        out.push_back(13U);
        write_layer(out,col.labels);
        write_newline(out);
    }
    write_legend(out,level);
    return out;
}
} // namespace rloft
