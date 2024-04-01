#define DO_REPETITION_REDUCTION

#include <Functions.hpp>
#include <chrono>

int main(int argc, const char *argv[])
{
    if (argc > 1 && !strcmp(argv[1], "--comp"))
    {
        if (argc <= 3)
        {
            std::cout << "Usage: " << argv[0] << " --comp <originalFile> <mosaicFile>\n";
            return EXIT_FAILURE;
        }
        const char *originalFile = argv[2];
        const char *mosaicFile = argv[3];

        Image original(originalFile);
        Image mosaic(mosaicFile);

        mosaic.resize(original.size);

        const int size = original.size.x * original.size.y;

        vec3 eqm(0);
        for (int i = 0; i < size; i++)
        {
            vec3 d = vec3(original.pixels[i]) - vec3(mosaic.pixels[i]);
            eqm += d * d;
        }

        eqm /= (float)size;

        float PSNR = 10 * log10(3.f * 255.f * 255.f / (eqm.x + eqm.y + eqm.z));

        double SSIM = ComputeSSIM<8>(original, mosaic);

        std::cout << "EQM\t" << to_string(eqm) << "\n";
        std::cout << "PSNR\t" << PSNR << "\n";
        std::cout << "SSIM\t" << SSIM << "\n";

        return EXIT_SUCCESS;
    }

    if (argc <= 4)
    {
        std::cout << "Usage: " << argv[0] << " <inputFile> <res> <colorSpace> <technique>\n";
        std::cout << "\tTechniques:  L1DIFF, L2DIFF, L3DIFF\n";
        std::cout << "\tColorSpaces: RGB, HSL, HWB, LRGB, XYZD50, LAB, LCH, XYZD65, OKLAB, OKLCH, SRGB, DISPLAYP3, A98RGB, PROPHOTORGB, REC2020\n";
        return EXIT_FAILURE;
    }

    int res = 0;

    const char *inputFile = argv[1];
    sscanf(argv[2], "%d", &res);
    strcpy(colorSpace, argv[3]);
    const char *techniqueName = argv[4];

    std::vector<const char *> dataFiles =
        {
            "data/in/dataSets/part1.CIFAR-10",
            "data/in/dataSets/part2.CIFAR-10",
            "data/in/dataSets/part1.CIFAR-100",
            "data/in/dataSets/part2.CIFAR-100",
        };

    Dataset dat(dataFiles);
    Image input(inputFile);
    if (res)
        input.resizeForMinSubImageRes(res);
    Dataset img(input);

    std::string technique(techniqueName);
    technique += "_";

    auto start = std::chrono::high_resolution_clock::now();
    if (!strcmp(techniqueName, "L1DIFF"))
    {
        dL1F.compute(dat, L1FF);
        iL1F.compute(img, L1FF);
        MosaicGenerator::mosaic(&img, &dat, L1DIFF, 1e6f, 16);
    }
    if (!strcmp(techniqueName, "L2DIFF"))
    {
        dL2F.compute(dat, L2FF);
        iL2F.compute(img, L2FF);
        MosaicGenerator::mosaic(&img, &dat, L2DIFF, 1e6f, 16);
    }
    if (!strcmp(techniqueName, "L3DIFF"))
    {
        dL3F.compute(dat, L3FF);
        iL3F.compute(img, L3FF);
        MosaicGenerator::mosaic(&img, &dat, L3DIFF, 1e6f, 16);
    }
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Time: " << ((double)std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()) / 1000.0f << "s\n";

    // std::string technique = "LLFDIFF_AVGPOND_RGB";

    // std::string technique = "DIFF_RGB";
    // MosaicGenerator::mosaic(&img, &dat, diff, (int)1e6 , 4);

    technique += colorSpace;

#ifdef DO_REPETITION_REDUCTION
    technique += "_REPRED";
#endif

    img.toImage()->save(composeOutputName(
                            inputFile, img.getImgSize().x, img.getImgSize().y, dat.size(), technique.c_str())
                            .c_str());

    std::cout << "Done!\n";
    return EXIT_SUCCESS;
}