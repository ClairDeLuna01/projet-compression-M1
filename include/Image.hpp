#pragma once

#include <memory>

#include <glm/glm.hpp>
using namespace glm;

#include "pixel.hpp"

class Image;

class Image
{
private:
public:
    Image(){};
    Image(const char *filename) { load(filename); };
    ~Image();

    void load(const char *filename);
    void save(const char *filename);
    void alloc(ivec2 size);

    ivec2 size;
    pixel *pixels = nullptr;

    pixel &operator()(int i, int j) { return pixels[i + j * size.x]; };
    const pixel &operator()(int i, int j) const { return pixels[i + j * size.x]; };

    Image &resizeForMinSubImageRes(const int n);
    Image &resize(const ivec2 res);
};

typedef std::shared_ptr<Image> ImageRef;

template <int N>
double pixelSampleMean(const Image &img, int i, int j)
{
    double sum = 0;
    for (int x = i; x < i + N; x++)
    {
        for (int y = j; y < j + N; y++)
        {
            sum += img(x, y).r + img(x, y).g + img(x, y).b;
        }
    }
    return sum / (N * N * 3);
}

template <int N>
double pixelSampleVariance(const Image &img, int i, int j, double mean)
{
    double sum = 0;
    for (int x = i; x < i + N; x++)
    {
        for (int y = j; y < j + N; y++)
        {
            sum += (img(x, y).r + img(x, y).g + img(x, y).b - mean) * (img(x, y).r + img(x, y).g + img(x, y).b - mean);
        }
    }
    return sum / (N * N * 3);
}

template <int N>
double pixelSampleCovariance(const Image &img1, const Image &img2, int i, int j, double mean1, double mean2)
{
    double sum = 0;
    for (int x = i; x < i + N; x++)
    {
        for (int y = j; y < j + N; y++)
        {
            sum += (img1(x, y).r + img1(x, y).g + img1(x, y).b - mean1) * (img2(x, y).r + img2(x, y).g + img2(x, y).b - mean2);
        }
    }
    return sum / (N * N * 3);
}

template <int N = 8>
double ComputeSSIM(const Image &img1, const Image &img2)
{
    constexpr double k1 = 0.01;
    constexpr double k2 = 0.03;
    constexpr double L = 255;

    constexpr double c1 = (k1 * L) * (k1 * L);
    constexpr double c2 = (k2 * L) * (k2 * L);

    double mean1, mean2;
    double variance1, variance2;
    double covariance;

    double ssim = 0;

    for (int i = 0; i < img1.size.x; i += N)
    {
        for (int j = 0; j < img1.size.y; j += N)
        {
            mean1 = pixelSampleMean<N>(img1, i, j);
            mean2 = pixelSampleMean<N>(img2, i, j);

            variance1 = pixelSampleVariance<N>(img1, i, j, mean1);
            variance2 = pixelSampleVariance<N>(img2, i, j, mean2);

            covariance = pixelSampleCovariance<N>(img1, img2, i, j, mean1, mean2);

            ssim += (2 * mean1 * mean2 + c1) * (2 * covariance + c2) / ((mean1 * mean1 + mean2 * mean2 + c1) * (variance1 + variance2 + c2));
        }
    }

    return ssim / (img1.size.x * img1.size.y / (N * N));
}