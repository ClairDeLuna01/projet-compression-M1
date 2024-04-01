#include <Functions.hpp>

char colorSpace[256];

convertedColor getConvertedColor(float r, float g, float b)
{
    if(!strcmp(colorSpace, "RGB"))
    {
        colorm::Rgb res(r,g,b);
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "HSL"))
    {
        colorm::Hsl res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "HWB"))
    { 
        colorm::Hwb res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "LRGB"))
    {
        colorm::Lrgb res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "XYZD50"))
    {
        colorm::XyzD50 res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "LAB"))
    {
        colorm::Lab res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "LCH"))
    {
        colorm::Lch res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }
    
    if(!strcmp(colorSpace, "XYZD65"))
    {
        colorm::XyzD65 res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "OKLAB"))
    {
        colorm::Oklab res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "OKLCH"))
    {
        colorm::Oklch res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "SRGB"))
    {
        colorm::Srgb res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "DISPLAYP3"))
    {
        colorm::DisplayP3 res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "A98RGB"))
    {
        colorm::A98Rgb res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "PROPHOTORGB"))
    {
        colorm::ProphotoRgb res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    if(!strcmp(colorSpace, "REC2020"))
    {
        colorm::Rec2020 res(colorm::Rgb(r,g,b));
        return *(convertedColor*)&res;
    }

    return {0., 0., 0., 0.};
}


/****** Defining dataset atributes ******/
std::function<vec3(const DataElem &)> avgF = [](const DataElem &e)
{
    vec3 avg;
    for (int i = 0; i < SUB_IMAGE_ROW; i++)
        for (int j = 0; j < SUB_IMAGE_ROW; j++)
        {
            vec3 p(e.r.array[i][j], e.g.array[i][j], e.b.array[i][j]);
            avg += p;
        }

    avg /= SUB_IMAGE_SIZE;
    return avg;
};

std::function<LF1_subImage(const DataElem &)> L1FF = [](const DataElem &e)
{
    LF1_subImage wave;

    /****** LOW FREQUENCY REC 1 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW; j += 2)
            {
                float A = (float)e[c].array[i][j];
                float B = (float)e[c].array[i + 1][j];
                float C = (float)e[c].array[i][j + 1];
                float D = (float)e[c].array[i + 1][j + 1];

                wave.rgb[c].array[i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** COLOR SPACE CONVERT ******/
    for (int i = 0; i < SUB_IMAGE_ROW / 2; i ++)
        for (int j = 0; j < SUB_IMAGE_ROW / 2; j ++)
        {
            convertedColor conv = getConvertedColor(wave.rgb[0].array[i][j], wave.rgb[1].array[i][j], wave.rgb[2].array[i][j]);
            
            wave.rgb[0].array[i][j] = conv.c[0];
            wave.rgb[1].array[i][j] = conv.c[1];
            wave.rgb[2].array[i][j] = conv.c[2];
        }

    return wave;
};

std::function<LF2_subImage(const DataElem &)> L2FF = [](const DataElem &e)
{
    LF2_subImage wave;

    float array[3][SUB_IMAGE_ROW / 2][SUB_IMAGE_ROW / 2];

    /****** LOW FREQUENCY REC 1 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW; j += 2)
            {
                float A = (float)e[c].array[i][j];
                float B = (float)e[c].array[i + 1][j];
                float C = (float)e[c].array[i][j + 1];
                float D = (float)e[c].array[i + 1][j + 1];

                array[c][i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** LOW FREQUENCY REC 2 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW/2; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW/2; j += 2)
            {
                float A = (float)array[c][i][j];
                float B = (float)array[c][i + 1][j];
                float C = (float)array[c][i][j + 1];
                float D = (float)array[c][i + 1][j + 1];

                wave.rgb[c].array[i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** COLOR SPACE CONVERT ******/
    for (int i = 0; i < SUB_IMAGE_ROW / 4; i ++)
        for (int j = 0; j < SUB_IMAGE_ROW / 4; j ++)
        {
            convertedColor conv = getConvertedColor(wave.rgb[0].array[i][j], wave.rgb[1].array[i][j], wave.rgb[2].array[i][j]);
            
            wave.rgb[0].array[i][j] = conv.c[0];
            wave.rgb[1].array[i][j] = conv.c[1];
            wave.rgb[2].array[i][j] = conv.c[2];
        }

    return wave;
};

std::function<LF3_subImage(const DataElem &)> L3FF = [](const DataElem &e)
{
    LF3_subImage wave;

    float array[3][SUB_IMAGE_ROW / 2][SUB_IMAGE_ROW / 2];
    float array2[3][SUB_IMAGE_ROW / 4][SUB_IMAGE_ROW / 4];

    /****** LOW FREQUENCY REC 1 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW; j += 2)
            {
                float A = (float)e[c].array[i][j];
                float B = (float)e[c].array[i + 1][j];
                float C = (float)e[c].array[i][j + 1];
                float D = (float)e[c].array[i + 1][j + 1];

                array[c][i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** LOW FREQUENCY REC 2 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW/2; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW/2; j += 2)
            {
                float A = (float)array[c][i][j];
                float B = (float)array[c][i + 1][j];
                float C = (float)array[c][i][j + 1];
                float D = (float)array[c][i + 1][j + 1];

                array2[c][i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** LOW FREQUENCY REC 3 ******/
    for (int c = 0; c < 3; c++)
        for (int i = 0; i < SUB_IMAGE_ROW / 4; i += 2)
            for (int j = 0; j < SUB_IMAGE_ROW / 4; j += 2)
            {
                float A = array2[c][i][j];
                float B = array2[c][i + 1][j];
                float C = array2[c][i][j + 1];
                float D = array2[c][i + 1][j + 1];
                wave.rgb[c].array[i / 2][j / 2] = (A + B + C + D)/4.f;
            }

    /****** COLOR SPACE CONVERT ******/
    for (int i = 0; i < SUB_IMAGE_ROW / 8; i ++)
        for (int j = 0; j < SUB_IMAGE_ROW / 8; j ++)
        {
            convertedColor conv = getConvertedColor(wave.rgb[0].array[i][j], wave.rgb[1].array[i][j], wave.rgb[2].array[i][j]);
            
            wave.rgb[0].array[i][j] = conv.c[0];
            wave.rgb[1].array[i][j] = conv.c[1];
            wave.rgb[2].array[i][j] = conv.c[2];
        }

    return wave;
};

DatasetAttribute<vec3> dAvg;
DatasetAttribute<vec3> iAvg;
DatasetAttribute<LF1_subImage> dL1F;
DatasetAttribute<LF1_subImage> iL1F;
DatasetAttribute<LF2_subImage> dL2F;
DatasetAttribute<LF2_subImage> iL2F;
DatasetAttribute<LF3_subImage> dL3F;
DatasetAttribute<LF3_subImage> iL3F;

/****** Defining subImage comparison techniques ******/
std::function<int(subImage &, subImage &, int, int, const int &)> DIFF =
    [](subImage &a, subImage &b, int aid, int bid, const int &scoreClosest) -> int
{
    int score = 0;
    for (int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
        score += a.r.data[i] > b.r.data[i] ? a.r.data[i] - b.r.data[i] : b.r.data[i] - a.r.data[i];

    for (int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
        score += a.g.data[i] > b.g.data[i] ? a.g.data[i] - b.g.data[i] : b.g.data[i] - a.g.data[i];

    for (int i = 0; i < SUB_IMAGE_SIZE && score < scoreClosest; i++)
        score += a.b.data[i] > b.b.data[i] ? a.b.data[i] - b.b.data[i] : b.b.data[i] - a.b.data[i];

    return score;
};

std::function<float(subImage &, subImage &, int, int, const float &)> L1DIFF =
    [](subImage &a, subImage &b, int aid, int bid, const float &scoreClosest) -> float
{
    vec3 score(0);
    for (int c = 0; c < 3 && score[c] < scoreClosest; c++)
    {
        __m256 _res = _mm256_set1_ps(0.f);

        for (int i = 0; i < SUB_IMAGE_SIZE/4; i += 8)
        {
            __m256 _a = _mm256_loadu_ps(&iL1F[aid].rgb[c].data[i]);
            __m256 _b = _mm256_loadu_ps(&dL1F[bid].rgb[c].data[i]);
            _res = _mm256_add_ps(_res, _mm256_andnot_ps(_mm256_set1_ps(-0.f), _mm256_sub_ps(_a, _b)));
        }

        float tmp[8];
        _mm256_storeu_ps(tmp, _res);
        score[c] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
    }

    return score.r + score.g + score.b;
};

std::function<float(subImage &, subImage &, int, int, const float &)> L2DIFF =
    [](subImage &a, subImage &b, int aid, int bid, const float &scoreClosest) -> float
{
    vec3 score(0);
    for (int c = 0; c < 3 && score[c] < scoreClosest; c++)
    {
        __m256 _res = _mm256_set1_ps(0.f);

        for (int i = 0; i < SUB_IMAGE_SIZE/16; i += 8)
        {
            __m256 _a = _mm256_loadu_ps(&iL2F[aid].rgb[c].data[i]);
            __m256 _b = _mm256_loadu_ps(&dL2F[bid].rgb[c].data[i]);
            _res = _mm256_add_ps(_res, _mm256_andnot_ps(_mm256_set1_ps(-0.f), _mm256_sub_ps(_a, _b)));
        }

        float tmp[8];
        _mm256_storeu_ps(tmp, _res);
        score[c] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
    }

    return score.r + score.g + score.b;
};

std::function<float(subImage &, subImage &, int, int, const float &)> L3DIFF =
    [](subImage &a, subImage &b, int aid, int bid, const float &scoreClosest) -> float
{
    vec3 score(0);
    for (int c = 0; c < 3 && score[c] < scoreClosest; c++)
    {
        __m256 _res = _mm256_set1_ps(0.f);

        for (int i = 0; i < SUB_IMAGE_SIZE/64; i += 8)
        {
            __m256 _a = _mm256_loadu_ps(&iL3F[aid].rgb[c].data[i]);
            __m256 _b = _mm256_loadu_ps(&dL3F[bid].rgb[c].data[i]);
            _res = _mm256_add_ps(_res, _mm256_andnot_ps(_mm256_set1_ps(-0.f), _mm256_sub_ps(_a, _b)));
        }

        float tmp[8];
        _mm256_storeu_ps(tmp, _res);
        score[c] = tmp[0] + tmp[1] + tmp[2] + tmp[3] + tmp[4] + tmp[5] + tmp[6] + tmp[7];
    }

    return score.r + score.g + score.b;
};