#pragma once 

#include <vector>
#include <list>
#include <fstream>

#include <functional>

#include "Image.hpp"

#define SUB_IMAGE_ROW 32
#define SUB_IMAGE_SIZE SUB_IMAGE_ROW*SUB_IMAGE_ROW

class subImageChannel
{
    public :

        union
        {
            uint8 array[SUB_IMAGE_ROW][SUB_IMAGE_ROW];
            uint8 data[SUB_IMAGE_SIZE];
        };
};

class LF1_subImageChannel
{
    public :
        union
        {
            float array[SUB_IMAGE_ROW/2][SUB_IMAGE_ROW/2];
            float data[SUB_IMAGE_SIZE/4];
        };  
};

class LF2_subImageChannel
{
    public :
        union
        {
            float array[SUB_IMAGE_ROW/4][SUB_IMAGE_ROW/4];
            float data[SUB_IMAGE_SIZE/16];
        };  
};

class LF3_subImageChannel
{
    public :
        union
        {
            float array[SUB_IMAGE_ROW/8][SUB_IMAGE_ROW/8];
            float data[SUB_IMAGE_SIZE/64];
        };  
};


uint32 operator-(const subImageChannel & __restrict__ a, const subImageChannel & __restrict__ b);

typedef glm::vec<3, subImageChannel, glm::packed_highp> subImage;

struct LF1_subImage{LF1_subImageChannel rgb[3];};
struct LF2_subImage{LF2_subImageChannel rgb[3];};
struct LF3_subImage{LF3_subImageChannel rgb[3];};

struct DataElem : subImage
{
    void toImage(Image &out, ivec2 pos);
};

class Dataset : public std::vector<DataElem>
{
    private :
        ivec2 imgSize = ivec2(0);

    public : 
        Dataset(){};
        Dataset(std::vector<const char*> filenames, int size = 3e4){load(filenames, size);};
        Dataset(const Image &img){load(img);};

        void load(std::vector<const char*> filenames, int size = 1e4);
        void load(const Image &img);

        ImageRef toImage();

        void convertSpace(std::function<pixel(pixel)> f);

        ivec2 getImgSize(){return imgSize;};


};

template<typename T>
class DatasetAttribute : public std::vector<T>
{
    public :

        DatasetAttribute(){};
        DatasetAttribute(const Dataset &d, std::function<T(const DataElem &)> f)
        {compute(d, f);};

        void compute(const Dataset &d, std::function<T(const DataElem &)> f)
        {
            this->resize(d.size());
            int id = 0;
            for(auto &e : d)
                this->at(id++) = f(e);
        }
};

