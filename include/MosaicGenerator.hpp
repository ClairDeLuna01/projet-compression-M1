#pragma once

#include <thread>
#include <mutex>
#include <array>
#include <algorithm>
#include <random>
#include <iostream>
#include <glm/glm.hpp>
#include "Dataset.hpp"

#define SubImageScoringFunction std::function<T(subImage &a, subImage &b, int aid, int bid, const T &scoreClosest)>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

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

    static inline const int repredSampleSize = 32;
    static int progress;

    template <typename T>
    static void mosaicThreadFast(
        Dataset *img,
        Dataset *dat,
        SubImageScoringFunction f,
        std::vector<std::pair<int, T>> *bestCandidates,
        std::mutex *progressBarMutex,
        int beg, int end, int jump, T maxScore)
    {
        const int dSize = (int)dat->size();

        const int iSize = (int)img->size();

        for (int i = beg; i < end; i += jump)
        {
            T scoreClosest = maxScore;

            progressBarMutex->lock();
            progress++;
            if (progress % 50 == 0)
            {
                // print progress bar
                std::cout << "\r[";
                for (int i = 0; i < 50; i++)
                {
                    std::cout << (i < (progress - 1) * 50 / iSize ? "=" : (i == progress * 50 / iSize ? ">" : " "));
                }

                std::cout << "] " << progress * 100 / iSize << "%" << std::flush;
            }
            progressBarMutex->unlock();

            for (int j = 0; j < dSize; j++)
            {
                T score = f(img->at(i), dat->at(j), i, j, scoreClosest);
                bestCandidates[i].push_back({j, score});
                scoreClosest = score < scoreClosest ? score : scoreClosest;
            }

            std::partial_sort(
                bestCandidates[i].begin(),
                bestCandidates[i].begin() + repredSampleSize,
                bestCandidates[i].end(),
                [](const std::pair<int, T> &a, const std::pair<int, T> &b)
                {
                    return a.second < b.second;
                });

#ifdef DO_REPETITION_REDUCTION
            std::shuffle(
                bestCandidates[i].begin(),
                bestCandidates[i].begin() + repredSampleSize,
                std::default_random_engine(i));
#endif

            bestCandidates[i].resize(repredSampleSize);
            bestCandidates[i].shrink_to_fit();
        }
    };

public:
    template <typename T>
    static void mosaic(Dataset *img, Dataset *dat, SubImageScoringFunction f, T maxScore, int threadNB = 8)
    {
        // std::thread t[threadNB];
        // const int size = (int)img->size();
        // std::vector<ivec3> history(dat->size());
        // std::vector<std::mutex> historyMutex(dat->size());

        // std::fill(history.begin(), history.end(), ivec3(0));

        // for(int i = 0; i < threadNB; i++)
        //     t[i] = std::thread(mosaicThread<T>,
        //     img, dat, f,
        //     &history, &historyMutex,
        //     i*size/threadNB, (i+1)*size/threadNB-1,
        //     1, maxScore);

        // for(int i = 0; i < threadNB; i++)
        //     t[i].join();

        const int size = (int)img->size();
        std::thread t[threadNB];
        std::vector<std::pair<int, T>> bestCandidates[size];

        // for(int i = 0; i < size; i++)
        //     bestCandidates->reserve(dat->size());

        std::mutex progressBarMutex;

        for (int i = 0; i < threadNB; i++)
        {
            int beg = i * (size / threadNB);
            int end = i == threadNB ? size : beg + size / threadNB;
            t[i] = std::thread(
                mosaicThreadFast<T>, img, dat, f, &bestCandidates[0],
                &progressBarMutex, beg, end, 1, maxScore);
        }

        for (int i = 0; i < threadNB; i++)
            t[i].join();

        std::cout << "\r[==================================================] 100%\n";

#ifdef DO_REPETITION_REDUCTION
        const int dsize = dat->size();
        std::vector<ivec2> history(dsize);
        std::fill(history.begin(), history.end(), ivec2(-50, -50));
        const ivec2 imgSize = img->getImgSize();

        int maxtmp = -1e6;

        int i = 0;
        for (int x = 0; x < imgSize.x; x++)
            for (int y = 0; y < imgSize.y; y++, i++)
            {
                ivec2 imgPos(x, y);
                int bestID = 0;

                auto &p = bestCandidates[i];

                for (; bestID < repredSampleSize - 1; bestID++)
                {
                    ivec2 vec(abs(imgPos - history[p[bestID].first]));

                    // if((std::rand()%4) >= 1) continue;

                    if (vec.x != 0 && vec.y != 0)
                    {
                        history[p[bestID].first] = imgPos;
                        break;
                    }
                }

                maxtmp = max(maxtmp, bestID);

                img->at(i) = dat->at(p[bestID].first);
            }
        std::cout << maxtmp << "\n";
#else
        for (int i = 0; i < size; i++)
            img->at(i) = dat->at(bestCandidates[i][0].first);
#endif
    };
};