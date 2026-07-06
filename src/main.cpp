#include "file.h"
#include "image.h"

#include <print>
#include <cstdlib>

consteval auto version() -> std::string_view {
    return "0.1.0";
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