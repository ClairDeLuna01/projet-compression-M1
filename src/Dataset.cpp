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
            r.array[j][i], g.array[j][i], b.array[j][i]
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
            e.r.array[y][x] = p.r;
            e.g.array[y][x] = p.g;
            e.b.array[y][x] = p.b;
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

#include <immintrin.h>

uint32 operator-(const subImageChannel & __restrict__ a, const subImageChannel & __restrict__ b)
{
    uint32 r = 0;
    for(int i = 0; i < SUB_IMAGE_SIZE; i++)
        r += a.data[i] < b.data[i] ? b.data[i]-a.data[i] : a.data[i]-b.data[i];

    // __m256i vecr = _mm256_set1_epi8(0);

    // for(int i = 0; i < SUB_IMAGE_SIZE; i+=32)
    // {
    //     __m256i vec1 = (__m256i)_mm256_loadu_pd((double*)&a.data[i]);
    //     __m256i vec2 = (__m256i)_mm256_loadu_pd((double*)&b.data[i]);

    //     __m256i diff1 = _mm256_sub_epi8(vec1, vec2);
    //     __m256i diff2 = _mm256_subs_epi8(vec2, vec1);
    //     __m256i diff = _mm256_add_epi8(diff1, diff2);

    //     vecr = _mm256_add_epi8(diff, vecr);
    // }

    // uint8 res[32] = {0};

    // _mm256_storeu_pd((double *)res, (__m256d)vecr);

    // int r = 0;

    // for(int i = 0; i < 32; i++)
    //     r += res[i];

    return r;
}
