#pragma once

#include <iostream>
#include <string>

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <stb/stb_image_resize2.h>

#include <colorm.h>

#include "Utils.hpp"
#include "Dataset.hpp"
#include "Image.hpp"
#include "MosaicGenerator.hpp"

#include <immintrin.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

struct convertedColor
{
    double c[4];
};

extern char colorSpace[256];
convertedColor getConvertedColor(float r, float g, float b);

extern std::function<vec3(const DataElem &)> avgF;
extern std::function<LF1_subImage(const DataElem &)> L1FF;
extern std::function<LF2_subImage(const DataElem &)> L2FF;
extern std::function<LF3_subImage(const DataElem &)> L3FF;

extern DatasetAttribute<vec3> dAvg;
extern DatasetAttribute<vec3> iAvg;

extern DatasetAttribute<LF1_subImage> dL1F;
extern DatasetAttribute<LF1_subImage> iL1F;

extern DatasetAttribute<LF2_subImage> dL2F;
extern DatasetAttribute<LF2_subImage> iL2F;

extern DatasetAttribute<LF3_subImage> dL3F;
extern DatasetAttribute<LF3_subImage> iL3F;

extern std::function<int(subImage &, subImage &, int, int, const int &)> DIFF;
extern std::function<float(subImage &, subImage &, int, int, const float &)> L1DIFF;
extern std::function<float(subImage &, subImage &, int, int, const float &)> L2DIFF;
extern std::function<float(subImage &, subImage &, int, int, const float &)> L3DIFF;
extern std::function<float(subImage &, subImage &, int, int, const float &)> AVG;
