#include <print>
#include <string_view>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <vector>
#include <array>
#include <span>
#include <cstdint>
#include <optional>
#include <ranges>
#include <algorithm>

consteval auto version() -> std::string_view {
    return "0.1.0";
}

// @brief Reads file and returns buffer.
// @param path The path to the file.
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

enum class ImageFormat {
    png,
    jpg,
    gif,
    bmp
};

// @brief Stringifies the ImageFormat enum.
// @param fmt The ImageFormat value.
auto image_format_to_string(ImageFormat fmt) -> std::string_view {
    switch (fmt) {
        case ImageFormat::png:
            return "PNG";
        case ImageFormat::jpg:
            return "JPEG";
        case ImageFormat::gif:
            return "GIF";
        case ImageFormat::bmp:
            return "BMP";
    }

    return "Unknown";
}

auto detect_format(std::span<const std::byte> bytes) -> std::optional<ImageFormat> {
    auto header = std::string_view{
        reinterpret_cast<const char*>(bytes.data()), bytes.size()
    };

    if (header.starts_with("\x89PNG\r\n\x1a\n")) return ImageFormat::png;
    if (header.starts_with("\xff\xd8\xff")) return ImageFormat::jpg;
    if (header.starts_with("\x47\x49\x46\x38")) return ImageFormat::gif;
    if (header.starts_with("\x42\x4d")) return ImageFormat::bmp;

    return std::nullopt;
}

auto main(int argc, char* argv[]) -> int {
    if (argc < 2) {
        std::println("metaimg v{} - Image metadata viewer", version());
        std::println("Usage: metaimg <image-file>");
        return EXIT_FAILURE;
    }

    std::string_view file = argv[1];
    std::println("File: {}", file);

    // Read image file and retrieve bytes.
    auto bytes = read_file(argv[1]);
    if (!bytes) return EXIT_FAILURE;

    // Display size of image file and first couple of bytes.
    auto view = std::span{*bytes};
    auto image_format = detect_format(view);

    if (!image_format) return EXIT_FAILURE;

    std::println("Size: {} bytes", bytes->size());
    std::println("First bytes: {:02X} {:02X} ...",
        std::to_integer<unsigned>(view[0]),
        std::to_integer<unsigned>(view[1])
    );
    std::println("Format: {}", image_format_to_string(image_format.value()));

    return EXIT_SUCCESS;
}