#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#include "Dataset.hpp"

Image::~Image()
{
    if(pixels) delete [] pixels;
}

void Image::load(const char *filename)
{
    pixels = (pixel*)stbi_load(filename, &size.x, &size.y, nullptr, 3);
}

void Image::save(const char *filename)
{
    stbi_write_png(filename, size.x, size.y, 3, pixels, 0);
}

void Image::alloc(ivec2 size)
{
    this->size = size;
    pixels = new pixel[size.x * size.y];
}

Image& Image::resizeForMinSubImageRes(const int n)
{
    pixel *oldPixels = pixels;
    ivec2 oldSize = size;

    vec2 tmp = vec2(size)/(float)size.x; 
    alloc(ivec2(round(tmp*(float)n))*SUB_IMAGE_ROW);

    stbir_resize_uint8((const uint8*)oldPixels, oldSize.x, oldSize.y, 0, (uint8*)pixels, size.x, size.y, 0, 3);

    delete [] oldPixels;
    return *this;
}