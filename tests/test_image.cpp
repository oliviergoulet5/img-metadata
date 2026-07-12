#include <gtest/gtest.h>
#include "image.h"
#include "file.h"

TEST(ImageMetadataTest, ReadsPngMetadata) {
    auto bytes = read_file("samples/lemon.png");
    ASSERT_TRUE(bytes.has_value());

    auto metadata = get_image_metadata(*bytes);
    ASSERT_TRUE(metadata.has_value());
    EXPECT_EQ(metadata->format, ImageFormat::png);
    EXPECT_EQ(metadata->dimensions.width, 225);
    EXPECT_EQ(metadata->dimensions.height, 225);
    EXPECT_EQ(metadata->bit_depth, 8);
    EXPECT_EQ(metadata->color_type, ColorType::indexed);
}

TEST(ImageMetadataTest, ReadsJpgMetadata) {
    auto bytes = read_file("samples/lotus.jpg");
    ASSERT_TRUE(bytes.has_value());

    auto metadata = get_image_metadata(*bytes);
    ASSERT_TRUE(metadata.has_value());
    EXPECT_EQ(metadata->format, ImageFormat::jpg);
    EXPECT_EQ(metadata->dimensions.width, 640);
    EXPECT_EQ(metadata->dimensions.height, 427);
    EXPECT_EQ(metadata->bit_depth, 8);
}

TEST(ImageMetadataTest, ReadsGifMetadata) {
    auto bytes = read_file("samples/satellite.gif");
    ASSERT_TRUE(bytes.has_value());
    
    auto metadata = get_image_metadata(*bytes);
    ASSERT_TRUE(metadata.has_value());
    EXPECT_EQ(metadata->format, ImageFormat::gif);
    EXPECT_EQ(metadata->dimensions.width, 730);
    EXPECT_EQ(metadata->dimensions.height, 548);
    EXPECT_EQ(metadata->bit_depth, 8);
}

TEST(ImageMetadataTest, ReadsBmpMetadata) {
    auto bytes = read_file("samples/icecream.bmp");
    ASSERT_TRUE(bytes.has_value());

    auto metadata = get_image_metadata(*bytes);
    ASSERT_TRUE(metadata.has_value());
    EXPECT_EQ(metadata->format, ImageFormat::bmp);
    EXPECT_EQ(metadata->dimensions.width, 1920);
    EXPECT_EQ(metadata->dimensions.height, 1920);
    EXPECT_EQ(metadata->bit_depth, 24);
}