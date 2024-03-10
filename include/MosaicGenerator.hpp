#pragma once

#include <thread>
#include "Dataset.hpp"

#define SubImageScoringFunction std::function<T(subImage &a, subImage &b, const T &scoreClosest)>

class MosaicGenerator 
{

    private : 
        template<typename T>
        static void mosaicThread(Dataset *img, Dataset *dat, SubImageScoringFunction f, int beg, int jump, T maxScore)
        {
            const int size = (int)img->size();
            const int dSize = (int)dat->size();
            for(int i = beg; i < size; i += jump)
            {
                int idClosest;
                T scoreClosest = maxScore;

                for(int j = 0; j < dSize; j++)
                {
                    T score = f(img->at(i), dat->at(j), scoreClosest);
                    if(score < scoreClosest)
                    {
                        scoreClosest = score;
                        idClosest = j;
                    }
                }

                img->at(i) = dat->at(idClosest);
                // std::cout << i << "\n";
            }
        };

    public :

        template<typename T>
        static void mosaic(Dataset *img, Dataset *dat, SubImageScoringFunction f, T maxScore, int threadNB = 8)
        {
            std::thread t[threadNB];

            for(int i = 0; i < threadNB; i++)
                t[i] = std::thread(mosaicThread<T>, img, dat, f, i, threadNB, maxScore);
            
            for(int i = 0; i < threadNB; i++)
                t[i].join();
        };
};