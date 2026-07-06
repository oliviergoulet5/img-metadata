#pragma once

#include <optional>
#include <string_view>
#include <vector>
#include <cstddef>

auto read_file(std::string_view path) -> std::optional<std::vector<std::byte>>;