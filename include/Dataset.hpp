#pragma once 

#include <vector>
#include <list>
#include <fstream>

#include <functional>

#include "Image.hpp"

#define SUB_IMAGE_ROW 32
#define SUB_IMAGE_SIZE SUB_IMAGE_ROW*SUB_IMAGE_ROW

typedef glm::vec<3, uint8[32][32], glm::packed_highp> subImage;

struct DataElem
{
    // char label;
    subImage pixels;

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

