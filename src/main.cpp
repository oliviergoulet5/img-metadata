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

enum class ImageFormat {
    png,
    jpg,
    gif,
    bmp
};

/// @brief Stringifies the ImageFormat enum.
/// @param format The ImageFormat value.
auto image_format_to_string(ImageFormat format) -> std::string_view {
    switch (format) {
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

/// @brief Reads 4 bytes as a big-endian 32-bit unsigned integer.
/// @param bytes The byte span of the image file.
/// @param offset The starting position of the 4-byte value.
/// @return The reconstructed 32-bit value.
auto read_be32(std::span<const std::byte> bytes, size_t offset) -> uint32_t {
    // Reconstructs the integer by placing each byte at correct position.
    return (std::to_integer<uint32_t>(bytes[offset]) << 24) 
        | (std::to_integer<uint32_t>(bytes[offset + 1]) << 16)
        | (std::to_integer<uint32_t>(bytes[offset + 2]) << 8)
        | (std::to_integer<uint32_t>(bytes[offset + 3]));
}

auto read_le16(std::span<const std::byte> bytes, size_t offset) -> uint16_t {
    return (std::to_integer<uint16_t>(bytes[offset]))
        | (std::to_integer<uint16_t>(bytes[offset + 1]) << 8);
}

auto read_le32(std::span<const std::byte> bytes, size_t offset) -> uint32_t {
    return (std::to_integer<uint32_t>(bytes[offset]))
        | (std::to_integer<uint32_t>(bytes[offset + 1]) << 8)
        | (std::to_integer<uint32_t>(bytes[offset + 2]) << 16)
        | (std::to_integer<uint32_t>(bytes[offset + 3]) << 24);
}

struct Dimensions {
    int width, height;
};

/// @brief Gets dimensions of an image.
/// @param bytes The byte span of the image file.
/// @param format The image file format.
/// @return The dimensions of the image.
auto get_dimensions(std::span<const std::byte> bytes, ImageFormat format) -> std::optional<Dimensions> {
    int w, h = 0;
    switch (format) {
        case ImageFormat::png:
            // Bytes 16-19 are width for PNG (4 bytes, big-endian)
            w = static_cast<int>(read_be32(bytes, 16));
            // Bytes 20-23 are height for PNG (4 bytes, big-endian)
            h = static_cast<int>(read_be32(bytes, 20));
            break;
        case ImageFormat::gif:
            // Byte 6-7 are the width for GIF (2 bytes, little-endian)
            w = static_cast<int>(read_le16(bytes, 6));
            // Byte 8-9 are the height for GIF (2 bytes, little-endian)
            h = static_cast<int>(read_le16(bytes, 8));
            break;
        case ImageFormat::bmp:
            // Byte 18-21 are the width for BMP (4 bytes, little-endian)
            w = static_cast<int>(read_le32(bytes, 18));
            // Byte 22-23 are the height for BMP (4 bytes, little-endian)
            h = static_cast<int>(read_le32(bytes, 22));
            break;
        default:
            return std::nullopt;
    }

    return Dimensions{width: w, height: h};
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

    auto image_dimensions = get_dimensions(view, image_format.value());

    std::println("Size: {} bytes", bytes->size());
    std::println("First bytes: {:02X} {:02X} ...",
        std::to_integer<unsigned>(view[0]),
        std::to_integer<unsigned>(view[1])
    );
    std::println("Format: {}", image_format_to_string(image_format.value()));
    
    if (image_dimensions) { 
        std::println("Width: {}", image_dimensions.value().width);
        std::println("Height: {}", image_dimensions.value().height);
    }

    return EXIT_SUCCESS;
}