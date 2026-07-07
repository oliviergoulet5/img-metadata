#include "image.h"

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

auto read_be16(std::span<const std::byte> bytes, size_t offset) -> uint16_t {
    return (std::to_integer<uint32_t>(bytes[offset]) << 8) 
        | (std::to_integer<uint32_t>(bytes[offset + 1]));
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

/// @brief Gets dimensions of an image.
/// @param bytes The byte span of the image file.
/// @param format The image file format.
/// @return The dimensions of the image.
auto get_dimensions(std::span<const std::byte> bytes, ImageFormat format) -> std::optional<Dimensions> {
    int w = 0, h = 0;
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
            // Bytes 22-25 are the height for BMP (4 bytes, little-endian)
            h = static_cast<int>(read_le32(bytes, 22));
            break;
        case ImageFormat::jpg:
            return read_jpg_dimensions(bytes);
        default:
            return std::nullopt;
    }

    return Dimensions{w, h};
}

/// @brief Scans each JPEG marker starting at offset 2 and calls visitor.
/// @param bytes The byte span of the JPEG image file.
/// @param visitor A callable with signature void(uint8_t marker, size_t offset).
///     Markers: 0xC0-C3 are SOF (contains width/height/precision).
///     0xD9 is EOI, 0xDA is SOS — stop scanning.
///     Anything else: read 2-byte length, then skip ahead by that many bytes.
void for_each_jpg_marker(std::span<const std::byte> bytes, auto visitor) {
    // Start at offset 2 (skip JPEG's magic bytes)
    size_t offset = 2;

    while (offset + 3 < bytes.size()) {
        if (std::to_integer<uint8_t>(bytes[offset]) != 0xFF) {
            ++offset; // skip padding bytes between markers
            continue;
        }

        // Read next byte to identify marker type
        auto marker = std::to_integer<uint8_t>(bytes[offset + 1]);
        // EOI (D9) or SOS (DA): stop, nothing useful follows
        if (marker == 0xD9 || marker == 0xDA) return;

        // Skip past the marker and its data
        auto len = read_be16(bytes, offset + 2);
        visitor(marker, offset);
        offset += len + 2; // +2 for FF + marker byte
    }
}

auto read_jpg_dimensions(std::span<const std::byte> bytes) -> std::optional<Dimensions> {
    std::optional<Dimensions> dimensions;

    for_each_jpg_marker(bytes, [&](uint8_t marker, size_t offset) {
        if (marker >= 0xC0 && marker <= 0xC3) {
            auto w = static_cast<int>(read_be16(bytes, offset + 7));
            auto h = static_cast<int>(read_be16(bytes, offset + 5));
            dimensions = Dimensions{w, h};
        }
    });

    return dimensions;
}

auto get_png_bit_depth(std::span<const std::byte> bytes) -> std::optional<int> {
    // Byte 24 is the bit depth for PNGs.
    auto bit_depth = std::to_integer<int>(bytes[24]);
    return bit_depth;
}

auto get_bmp_bit_depth(std::span<const std::byte> bytes) -> std::optional<int> {
    // Byte offest 28, 2 bytes, little endian for BMP bit depth.
    auto bit_depth = static_cast<int>(read_le16(bytes, 28));
    return bit_depth;
}

auto get_gif_bit_depth(std::span<const std::byte> bytes) -> std::optional<int> {
    // GIFs uses a packed byte (bitfield) to encode bit depth as opposed to an entire byte.
    // Bits 4-6 are color resolution (gives you bit depth when you add 1)
    // Bits 0-2 is size of global color table.
    // Bit 7 is global color table flag.
    // Bit 3 is sort flag.
    auto packed = std::to_integer<uint8_t>(bytes[10]);
    auto bit_depth = ((packed >> 4) & 0x07) + 1;

    return bit_depth;
}

auto get_jpg_bit_depth(std::span<const std::byte> bytes) -> std::optional<int> {
    // JPGs use the term "precision" instead of "bit depth". For the sake of
    // consistency, I'll refer to it as "bit depth".
    std::optional<int> bit_depth;

    for_each_jpg_marker(bytes, [&](uint8_t marker, size_t offset) {
        if (marker >= 0xC0 && marker <= 0xC3) {
            bit_depth = std::to_integer<int>(bytes[offset + 4]);
        }
    });

    return bit_depth;
}

auto get_bit_depth(std::span<const std::byte> bytes, ImageFormat format) -> std::optional<int> {
    switch (format) {
        case ImageFormat::jpg:
            return get_jpg_bit_depth(bytes);
        case ImageFormat::png:
            return get_png_bit_depth(bytes);
        case ImageFormat::bmp:
            return get_bmp_bit_depth(bytes);
        case ImageFormat::gif:
            return get_gif_bit_depth(bytes);
        default:
            return std::nullopt;
    }
}

auto get_png_color_type(std::span<const std::byte> bytes) -> ColorType {
    auto color_type_raw = std::to_integer<int>(bytes[25]);
    ColorType color_type;

    switch (color_type_raw) {
        case 0:
            color_type = ColorType::grayscale;
            break;
        case 2:
            color_type = ColorType::rgb;
            break;
        case 3:
            color_type = ColorType::indexed;
            break;
        case 4:
            color_type = ColorType::grayscale_alpha;
            break;
        case 6:
            color_type = ColorType::rgba;
            break;
        default:
            color_type = ColorType::unknown;
            break;
    }

    return color_type;
}

auto get_color_type(std::span<const std::byte> bytes, ImageFormat format) -> ColorType {
    switch (format) {
        case ImageFormat::png:
            return get_png_color_type(bytes);
        default:
            return ColorType::unknown;
    }
}

auto color_type_to_string(ColorType color_type) -> std::string_view {
    switch (color_type) {
        case ColorType::grayscale:
            return "Grayscale";
        case ColorType::grayscale_alpha:
            return "Grayscale Alpha";
        case ColorType::indexed:
            return "Indexed";
        case ColorType::rgb:
            return "RGB";
        case ColorType::rgba:
            return "RGBA";
        default:
            return "Unknown";
    }
}

auto get_image_metadata(std::span<const std::byte> bytes) -> std::optional<ImageMetadata> {
    auto format = detect_format(bytes);
    if (!format) return std::nullopt;
    
    auto dimensions = get_dimensions(bytes, format.value());
    auto bit_depth = get_bit_depth(bytes, format.value());
    auto color_type = get_color_type(bytes, format.value());

    return ImageMetadata{format.value(), dimensions.value_or(Dimensions{0,0}), bit_depth.value_or(0), color_type};
}