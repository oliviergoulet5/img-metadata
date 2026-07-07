#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

enum class ImageFormat {
    png,
    jpg,
    gif,
    bmp
};

auto detect_format(std::span<const std::byte> bytes) -> std::optional<ImageFormat>;

auto image_format_to_string(ImageFormat format) -> std::string_view;

struct Dimensions {
    int width, height;
};

auto read_jpg_dimensions(std::span<const std::byte> bytes) -> std::optional<Dimensions>;

auto get_dimensions(std::span<const std::byte> bytes, ImageFormat format) -> std::optional<Dimensions>;

auto read_be16(std::span<const std::byte> bytes, size_t offset) -> uint16_t;

auto read_be32(std::span<const std::byte> bytes, size_t offset) -> uint32_t;

auto read_le16(std::span<const std::byte> bytes, size_t offset) -> uint16_t;

auto read_le32(std::span<const std::byte> bytes, size_t offset) -> uint32_t;

struct ImageMetadata {
    ImageFormat format;
    Dimensions dimensions = {0, 0};
    int bit_depth = 0;
};

auto get_image_metadata(std::span<const std::byte> bytes) -> std::optional<ImageMetadata>;