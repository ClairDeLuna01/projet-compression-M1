#include <iostream>
#include <string>
#include "Dataset.hpp"
#include "Image.hpp"

#include <Utils.hpp>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(vec2(c.b, c.g), vec2(K.w, K.z)), vec4(vec2(c.g, c.b), vec2(K.x, K.y)), step(c.b, c.g));
    vec4 q = mix(vec4(vec3(p.x, p.y, p.w), c.r), vec4(c.r, vec3(p.y, p.z, p.x)), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

int main(int argc, const char *argv[])
{

    /******* Saving a n*n mosaic of the first element in the dataset 
    {
        const int mosaicSize = 100;
        Dataset data("data/in/data_batch_1.bin");

        Image img;
        img.alloc(ivec2(mosaicSize*32));
        
        for(int i = 0; i < mosaicSize; i++)
        for(int j = 0; j < mosaicSize; j++)
            data[i*mosaicSize + j].toImage(img, ivec2(i, j)); 
        
        img.save("data/out/test4.png");
    }
    *******/

    /******* Converting an Image to Dataset then back to Image 
    {
        Dataset(Image("data/in/gradient.png")).toImage()->save("data/out/gradientReConverted.png");
    }
    *******/
    // const char* inputFile = "data/in/cat-hres.png";
    const char* inputFile = argv[1];
    const char* technique = "AVG_RGB";

    std::vector<const char *> dataFiles =
    {
        // "data/in/data_batch_1.bin",
        // "data/in/data_batch_2.bin",
        // "data/in/data_batch_3.bin",
        // "data/in/data_batch_4.bin",
        // "data/in/data_batch_5.bin"

        // "data/in/dataSets/data_batch.CIFAR-10",
        // "data/in/dataSets/data_batch.CIFAR-100"

        "data/in/dataSets/part1.CIFAR-10",
        "data/in/dataSets/part2.CIFAR-10",
    };
 
    int res = 0;

    if(argc >= 3)
        sscanf(argv[2], "%d", &res);

    Dataset dat(dataFiles);
    Image input(inputFile);
    if(res) input.resizeForMinSubImageRes(res);
    Dataset img(input);

    std::function<vec3(const DataElem &)> avgF = [](const DataElem &e)
    {
        vec3 avg;
        for(int i = 0; i < SUB_IMAGE_ROW; i++)
        for(int j = 0; j < SUB_IMAGE_ROW; j++)
        {
            vec3 p(e.pixels.r[i][j], e.pixels.g[i][j], e.pixels.b[i][j]);

            // p = rgb2hsv(p/255.f);

            avg += p;
        }

        avg /= SUB_IMAGE_SIZE;
        return avg;
    };

    DatasetAttribute<vec3> dAvg(dat, avgF);
    DatasetAttribute<vec3> iAvg(img, avgF);
    
    const int size = (int)iAvg.size();
    const int dSize = (int)dAvg.size();
    for(int i = 0; i < size; i++)
    {
        vec3 a = iAvg[i];

        int idClosest = 0;
        float scoreClosest = 1e9;
        for(int j = 0; j < dSize; j++)
        {
            float score = distance(a, dAvg[j]);

            if(score < scoreClosest)
            {
                scoreClosest = score;
                idClosest = j;
            }
        }

        // dAvg[idClosest] = vec3(1e9);
        
        img[i].pixels = dat[idClosest].pixels;
    }
    

    img.toImage()->save(composeOutputName(
        inputFile, img.getImgSize().x, img.getImgSize().y, dat.size(), technique
    ).c_str());

    std::cout << "Done!\n";
    return EXIT_SUCCESS;
}
