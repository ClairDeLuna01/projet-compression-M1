#pragma once 

#include <memory>

#include <glm/glm.hpp>
using namespace glm;

#include "pixel.hpp"

class Image
{
    private : 

        
    public :
        Image(){};
        Image(const char *filename){load(filename);};
        ~Image();

        void load(const char *filename);
        void save(const char *filename);
        void alloc(ivec2 size);

        ivec2 size;
        pixel* pixels = nullptr;
        
        pixel& operator ()(int i, int j){return pixels[i + j*size.x];};
        const pixel& operator ()(int i, int j) const {return pixels[i + j*size.x];};

        Image& resizeForMinSubImageRes(const int n);
        Image& resize(const ivec2 res);
};

typedef std::shared_ptr<Image> ImageRef;