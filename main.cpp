#include <iostream>
#include <string>


#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize.h>

#include "Utils.hpp"
#include "Dataset.hpp"
#include "Image.hpp"
#include "MosaicGenerator.hpp"

#include <immintrin.h>

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

    std::vector<const char *> dataFiles =
    {
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

/****** Defining dataset atributes ******/
    std::function<vec3(const DataElem &)> avgF = [](const DataElem &e)
    {
        vec3 avg;
        for(int i = 0; i < SUB_IMAGE_ROW; i++)
        for(int j = 0; j < SUB_IMAGE_ROW; j++)
        {
            vec3 p(e.r.array[i][j], e.g.array[i][j], e.b.array[i][j]);
            avg += p;
        }

        avg /= SUB_IMAGE_SIZE;
        return avg;
    };

    static DatasetAttribute<vec3> dAvg(dat, avgF);
    static DatasetAttribute<vec3> iAvg(img, avgF);


    std::function<subImageQuarter(const DataElem &)> waveF = [](const DataElem &e)
    {
        subImageQuarter wave;

        for(int c = 0; c < 3; c++)
        for(int i = 0; i < SUB_IMAGE_ROW; i+=2)
        for(int j = 0; j < SUB_IMAGE_ROW; j+=2)
        {
            float A = (float)e[c].array[i][j];
            float B = (float)e[c].array[i+1][j];
            float C = (float)e[c].array[i][j+1];
            float D = (float)e[c].array[i+1][j+1];

            wave.rgb[c].array[i/2][j/2] = (A+B+C+D);

            // wave.rgb[c].array[i/2][j/2] = A + abs(A-B) + abs(A-C) + abs(A - D);
        }

        return wave;
    };

    static DatasetAttribute<subImageQuarter> dWave(dat, waveF);
    static DatasetAttribute<subImageQuarter> iWave(img, waveF);

/****** Defining subImage comparison techniques ******/
    std::function<int(subImage &, subImage &, int, int,  const int &)> diff = 
    [](subImage &a, subImage &b, int aid, int bid, const int &scoreClosest) -> int{
        int score = 0;
        for(int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
            score += a.r.data[i] > b.r.data[i] ? a.r.data[i] - b.r.data[i] : b.r.data[i] - a.r.data[i];

        for(int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
            score += a.g.data[i] > b.g.data[i] ? a.g.data[i] - b.g.data[i] : b.g.data[i] - a.g.data[i];

        for(int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
            score += a.b.data[i] > b.b.data[i] ? a.b.data[i] - b.b.data[i] : b.b.data[i] - a.b.data[i];
        
        return score;
    }; 


    std::function<float(subImage &, subImage &, int, int, const float &)> spec = 
    [](subImage &a, subImage &b, int aid, int bid, const float &scoreClosest) -> float{

        vec3 score(0);
        
        for(int c = 0; c < 3 && score[c] < scoreClosest; c++)
        {
            __m256 _res = _mm256_set1_ps(0.f);
            // float *aptr = iWave[aid].rgb[c].data;
            // float *bptr = dWave[bid].rgb[c].data;

            for(int i = 0; i < SUB_IMAGE_SIZE/4; i+=8)
            {
                __m256 _a = _mm256_loadu_ps(&iWave[aid].rgb[c].data[i]);
                __m256 _b = _mm256_loadu_ps(&dWave[bid].rgb[c].data[i]);
                _res = _mm256_add_ps(_res, _mm256_andnot_ps(_mm256_set1_ps(-0.f), _mm256_sub_ps(_a, _b)));
            }

            float tmp[8];
            _mm256_storeu_ps(tmp, _res);
            score[c] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
        }

        // score += 128.f * abs(iAvg[aid] - dAvg[bid]);

        // return 1.2*score.r + 1.5*score.g + score.b;

        // score *= pow(1.f/iAvg[aid], vec3(0.25));
        score *= vec3(0.3, 0.6, 0.1) + iAvg[aid]/256.f;

        return score.r + score.g + score.b;
    }; 

    // const char* technique = "LFDIFF_AVGPOND_RGB_REPRED";
    // MosaicGenerator::mosaic(&img, &dat, spec, 1e6f, 1); 

    const char* technique = "DIFF_RGB_REPRED";
    MosaicGenerator::mosaic(&img, &dat, diff, (int)1e6 , 1); 
    


    // );

    // const int size = (int)img.size();
    // const int dSize = (int)dat.size();
    // for(int i = 0; i < size; i++)
    // {
    //     int idClosest = 0;
    //     uint32 scoreClosest = 1e6;

    //     for(int j = 0; j < dSize; j++)
    //     {
    //         uint32 score = (dat[j].r - img[i].r) + (dat[j].g - img[i].g) + (dat[j].b - img[i].b);

    //         if(score < scoreClosest)
    //         {
    //             scoreClosest = score;
    //             idClosest = j;
    //         }
    //     }

    //     img[i] = dat[idClosest];
    //     std::cout << i << "\n";
    // }

    /* AVG
    technique = "AVG_RGB"
    std::function<vec3(const DataElem &)> avgF = [](const DataElem &e)
    {
        vec3 avg;
        for(int i = 0; i < SUB_IMAGE_ROW; i++)
        for(int j = 0; j < SUB_IMAGE_ROW; j++)
        {
            vec3 p(e.pixels.r[i][j], e.pixels.g[i][j], e.pixels.b[i][j]);
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

        img[i].pixels = dat[idClosest].pixels;
    }
    */
    

    img.toImage()->save(composeOutputName(
        inputFile, img.getImgSize().x, img.getImgSize().y, dat.size(), technique
    ).c_str());

    std::cout << "Done!\n";
    return EXIT_SUCCESS;
}
