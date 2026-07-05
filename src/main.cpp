#include <print>
#include <string_view>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <vector>
#include <span>
#include <cstdint>
#include <optional>

consteval auto version() -> std::string_view {
    return "0.1.0";
}

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

auto main(int argc, char* argv[]) -> int {
    if (argc < 2) {
        std::println("metaimg v{} - Image metadata viewer", version());
        std::println("Usage: metaimg <image-file>");
        return EXIT_FAILURE;
    }

    std::string_view file = argv[1];
    std::println("File: {}", file);

    auto bytes = read_file(argv[1]);
    if (!bytes) return EXIT_FAILURE;

    auto view = std::span{*bytes};
    std::println("Size: {} bytes", bytes->size());
    std::println("First bytes: {:02X} {:02X} ...",
        std::to_integer<unsigned>(view[0]),
        std::to_integer<unsigned>(view[1])
    );

    return EXIT_SUCCESS;
}