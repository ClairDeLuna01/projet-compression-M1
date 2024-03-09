#include <iostream>
#include <string.h>

#include "Dataset.hpp"
#include "Utils.hpp"

void DataElem::toImage(Image &out, ivec2 pos)
{
    pos *= SUB_IMAGE_ROW;

    for(int i = 0; i < SUB_IMAGE_ROW; i++)
    for(int j = 0; j < SUB_IMAGE_ROW; j++)
    {
        out(i+pos.x, j+pos.y) = pixel(
            pixels.r[j][i], pixels.g[j][i], pixels.b[j][i]
        );
    }
}

void Dataset::load(std::vector<const char*> filenames, int size)
{
    resize(size * filenames.size());
    
    char *dat = (char*)data();
    const uint64 dataSetSize = size*sizeof(DataElem);

    for(auto &filename : filenames)
    {
        std::fstream file(filename, std::ios::in | std::ios::binary);

        file.read(dat, dataSetSize);
        
        dat += dataSetSize;
        file.close();
    }
}

void Dataset::load(const Image &img)
{
    if(img.size.x%SUB_IMAGE_ROW || img.size.y%SUB_IMAGE_ROW)
    {
        std::cerr << "Error : Dataset::load(Image), this image size is not a multiple of " << SUB_IMAGE_ROW << "\n";
        exit(EXIT_FAILURE);
    }

    imgSize = ivec2(img.size.x/SUB_IMAGE_ROW, img.size.y/SUB_IMAGE_ROW);

    resize(imgSize.x*imgSize.y);

    int id = 0;
    for(int i = 0; i < imgSize.x; i++)
    for(int j = 0; j < imgSize.y; j++, id++)
    {
        DataElem &e = at(id);
        for(int x = 0; x < 32; x++)
        for(int y = 0; y < 32; y++)
        {
            pixel p = img(i*32 + x, j*32 + y);
            e.pixels.r[y][x] = p.r;
            e.pixels.g[y][x] = p.g;
            e.pixels.b[y][x] = p.b;
        }
    }
}

ImageRef Dataset::toImage()
{
    ImageRef out(new Image);

    out->alloc(imgSize*SUB_IMAGE_ROW);

    int id = 0;
    for(int i = 0; i < imgSize.x; i++)
    for(int j = 0; j < imgSize.y; j++, id++)
        at(id).toImage(*out, ivec2(i, j));
    
    return out;
}

void Dataset::convertSpace(std::function<pixel(pixel)> f)
{
    
}