#pragma once

#include <thread>
#include <mutex>
#include <array>
#include <algorithm>
#include <glm/glm.hpp>
#include "Dataset.hpp"

#define SubImageScoringFunction std::function<T(subImage &a, subImage &b, int aid, int bid, const T &scoreClosest)>

class MosaicGenerator
{

private:
    template <typename T>
    static void mosaicThread(
        Dataset *img,
        Dataset *dat,
        SubImageScoringFunction f,
        std::vector<ivec3> *history,
        std::vector<std::mutex> *historyMutex,
        int beg, int end, int jump, T maxScore)
    {
        // const int size = (int)img->size();
        const int dSize = (int)dat->size();

        const ivec2 imgSize = img->getImgSize();

        for (int i = beg; i < end; i += jump)
        {
            int idClosest = 0;
            T scoreClosest = maxScore;

            ivec2 imgPos(i / imgSize.x, i % imgSize.x);

            for (int j = 0; j < dSize; j++)
            {
#ifdef DO_REPETITION_REDUCTION
                (*historyMutex)[j].lock();
                if ((*history)[j].z > 0)
                {

                    float sdisty = abs((*history)[j].y - imgPos.y);
                    float sdistx = abs((*history)[j].x - imgPos.x);
                    float sdist = distance(vec2((*history)[j].x, (*history)[j].y), vec2(imgPos));

                    if (sdist <= 25.f || sdistx <= 1.f || sdisty <= 1.f)
                    // if(sdistx <= 2+rand()%4 || sdisty <= 2+rand()%4)
                    {
                        (*historyMutex)[j].unlock();
                        continue;
                    }
                }
                (*historyMutex)[j].unlock();
#endif

                T score = f(img->at(i), dat->at(j), i, j, scoreClosest);
                if (score < scoreClosest)
                {
                    scoreClosest = score;
                    idClosest = j;
                }
            }

            (*historyMutex)[idClosest].lock();
            (*history)[idClosest] = ivec3(imgPos, (*history)[idClosest].z + 1);
            (*historyMutex)[idClosest].unlock();

            img->at(i) = dat->at(idClosest);
        }
    };

public:
    template <typename T>
    static void mosaic(Dataset *img, Dataset *dat, SubImageScoringFunction f, T maxScore, int threadNB = 8)
    {
        std::thread t[threadNB];
        const int size = (int)img->size();
        std::vector<ivec3> history(dat->size());
        std::vector<std::mutex> historyMutex(dat->size());

        std::fill(history.begin(), history.end(), ivec3(0));

        for (int i = 0; i < threadNB; i++)
            t[i] = std::thread(mosaicThread<T>,
                               img, dat, f,
                               &history, &historyMutex,
                               i * size / threadNB, (i + 1) * size / threadNB - 1,
                               1, maxScore);

        // t[i] = std::thread(mosaicThread<T>, img, dat, f, &history, &historyMutex, i, threadNB, maxScore);

        for (int i = 0; i < threadNB; i++)
            t[i].join();
    };
};