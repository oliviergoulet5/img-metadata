#include "file.h"

#include <filesystem>
#include <fstream>
#include <cstdlib>

/// @brief Reads file and returns buffer.
/// @param path The path to the file.
auto read_file(std::string_view path) -> std::optional<std::vector<std::byte>>
{
    auto path_str = std::string(path);
    auto size = std::filesystem::file_size(path_str);

    std::ifstream file(path_str, std::ios::binary);
    if (!file) return std::nullopt;

    std::vector<std::byte> buf(size);
    file.read(reinterpret_cast<char*>(buf.data()), size);
    if (!file) return std::nullopt;

    return buf;
}