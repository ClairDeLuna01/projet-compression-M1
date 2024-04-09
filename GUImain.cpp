#define SHM_UTILS_IMPLEMENTATION
#include "shmUtils.hpp"
#include <vector>
#include <string>
#include <thread>
#include <signal.h>
#include <iostream>

#include <glm/glm.hpp>
using namespace std;

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Choice.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Image.H>
#include <FL/Fl_Shared_Image.H>

#include "stb/stb_image.h"
// #include "stb_image_write.h"
#include "stb/stb_image_resize2.h"

void _atexit()
{
    finalizeSharedMemory();
}

void signalHandler(int signum)
{
    finalizeSharedMemory();
    exit(signum);
}

#ifdef _WIN32
#include <processthreadsapi.h>

std::wstring mainName = L"mosaic.exe";
int createChild(std::vector<std::string> &args)
{
    std::wstring command = mainName;
    for (auto &arg : args)
    {
        command += L" " + std::wstring(arg.begin(), arg.end());
    }

    STARTUPINFOW si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessW((LPWSTR)mainName.c_str(), (LPWSTR)command.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("CreateProcess failed (%d).\n", (int)GetLastError());
        return 1;
    }
    return 0;
}

#else
#include <unistd.h>
#include <sys/wait.h>

std::string mainName = "./mosaic";
int createChild(std::vector<std::string> &args)
{
    std::vector<char *> cargs;
    cargs.push_back((char *)mainName.c_str());
    for (auto &arg : args)
    {
        cargs.push_back((char *)arg.c_str());
    }
    cargs.push_back(nullptr);

    pid_t pid = fork();
    if (pid == 0)
    {
        execv(mainName.c_str(), cargs.data());
        return 1;
    }
    else if (pid < 0)
    {
        return 1;
    }
    return 0;
}

#endif

class ImageBox : public Fl_Box
{
private:
    Fl_RGB_Image *image;

public:
    ImageBox(int x, int y, int w, int h, const char *label = 0) : Fl_Box(x, y, w, h, label)
    {
        image = nullptr;
    }

    void setImage(Fl_RGB_Image *image)
    {
        if (this->image)
        {
            delete this->image;
        }
        this->image = image;
        redraw();
    }

    void draw()
    {
        Fl_Box::draw();
        if (image)
        {
            image->draw(x(), y(), w(), h());
        }
    }
};

int main()
{
    initializeSharedMemory();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGABRT, signalHandler);

    sharedMemory mem;

    constexpr int WIDTH = 1024;
    constexpr int HEIGHT = 768;
    Fl_Window *window = new Fl_Window(WIDTH, HEIGHT, "Mosaic Generator");

    // choose a file through a file chooser
    Fl_File_Chooser *chooser = new Fl_File_Chooser(".", "*", Fl_File_Chooser::SINGLE, "Choose an image");
    Fl_Button *fileButton = new Fl_Button(WIDTH - 20 - 100, 100, 100, 40, "Choose file");
    const int wMax = WIDTH - 260;
    const int hMax = HEIGHT - 120;
    ImageBox *fileLabel = new ImageBox(20, 40, wMax + 20, hMax + 20, "No file chosen...");
    fileLabel->align(FL_ALIGN_INSIDE | FL_ALIGN_CENTER);
    fileLabel->box(FL_FLAT_BOX);
    std::string filename = "";

    unsigned char *data = new unsigned char[wMax * hMax * 4];
    memset(data, 10, wMax * hMax * 4);

    struct RGBA
    {
        unsigned char r, g, b, a;
    };

    RGBA *pixels = (RGBA *)data;

    for (int i = 0; i < wMax; i++)
    {
        pixels[0 * wMax + i] = {0, 0, 0, 255};
        pixels[(hMax - 1) * wMax + i] = {0, 0, 0, 255};
    }

    for (int i = 0; i < hMax; i++)
    {
        pixels[i * wMax + 0] = {0, 0, 0, 255};
        pixels[i * wMax + (wMax - 1)] = {0, 0, 0, 255};
    }

    Fl_RGB_Image *image = new Fl_RGB_Image(data, wMax, hMax, 4);

    fileLabel->setImage(image);

    Fl_Input *resInput = new Fl_Input(WIDTH - 20 - 100, 170, 100, 40, "Resolution");
    Fl_Choice *colorSpace = new Fl_Choice(WIDTH - 20 - 100, 240, 100, 40, "Color Space");
    colorSpace->add("RGB");
    colorSpace->add("HSL");
    colorSpace->add("HWB");
    colorSpace->add("LRGB");
    colorSpace->add("XYZD50");
    colorSpace->add("LAB");
    colorSpace->add("LCH");
    colorSpace->add("XYZD65");
    colorSpace->add("OKLAB");
    colorSpace->add("OKLCH");
    colorSpace->add("SRGB");
    colorSpace->add("DISPLAYP3");
    colorSpace->add("A98RGB");
    colorSpace->add("PROPHOTORGB");
    colorSpace->add("REC2020");

    Fl_Choice *technique = new Fl_Choice(WIDTH - 20 - 100, 310, 100, 40, "Technique");
    technique->add("DIFF");
    technique->add("L1DIFF");
    technique->add("L2DIFF");
    technique->add("L3DIFF");
    technique->add("AVG");

    Fl_Button *generateButton = new Fl_Button(WIDTH - 20 - 100, 380, 100, 40, "Generate");

    Fl::visual(FL_RGB);
    window->end();
    window->show();

    std::vector<std::string> args;

    bool generating = false;
    while (true)
    {
        Fl::wait(0.05);
        if (
            (Fl::event() == FL_SHORTCUT && Fl::event_key() == FL_Escape) ||
            (Fl::event() == FL_CLOSE))
        {
            break;
        }
        if (fileButton->value())
        {
            chooser->show();
            while (chooser->visible())
            {
                Fl::wait();
            }
            if (chooser->value() == 0)
            {
                continue;
            }
            filename = chooser->value();
            fileLabel->copy_label(filename.c_str());
            fileButton->value(0);

            int width, height, channels;
            unsigned char *data = stbi_load(filename.c_str(), &width, &height, &channels, 3);

            if (data)
            {
                // resize so that it fits in the window, keeping the aspect ratio
                float aspect = (float)width / (float)height;

                int w = wMax;
                int h = hMax;

                if (width > wMax || height > hMax)
                {
                    if (width > height)
                    {
                        w = wMax;
                        h = wMax / aspect;
                    }
                    else
                    {
                        h = hMax;
                        w = hMax * aspect;
                    }
                }

                Fl_RGB_Image *imageBase = new Fl_RGB_Image(data, width, height, 3);
                Fl_RGB_Image *image = (Fl_RGB_Image *)imageBase->copy(w, h);
                fileLabel->setImage(image);

                // delete[] data;
            }
        }

        if (generateButton->value() && !filename.empty())
        {
            args.push_back(filename);
            args.push_back(resInput->value());
            args.push_back(colorSpace->text());
            args.push_back(technique->text());
            args.push_back("tmp.png");
            args.push_back("--subprocess");

            mem.status = true;
            mem.progress = 0;
            mem.total = 100;
            writeSharedMemory(mem);

            createChild(args);
            args.clear();
            generateButton->value(0);
            generating = true;
        }

        if (generating)
        {
            readSharedMemory(mem);
            if (mem.status)
            {
                if (mem.total == 0)
                {
                    std::cout << "Progress: 0/0\r" << std::flush;
                }
                else
                    std::cout << "Progress: " << mem.progress << "/" << mem.total << "\r" << std::flush;
            }
            else
            {
                generating = false;
                std::cout << "                                         \r" << std::flush;

                int width, height, channels;
                unsigned char *data = stbi_load("tmp.png", &width, &height, &channels, 0);
                if (data)
                {
                    // resize so that it fits in the window, keeping the aspect ratio
                    float aspect = (float)width / (float)height;

                    int w = wMax;
                    int h = hMax;

                    if (width > wMax || height > hMax)
                    {
                        if (width > height)
                        {
                            w = wMax;
                            h = wMax / aspect;
                        }
                        else
                        {
                            h = hMax;
                            w = hMax * aspect;
                        }
                    }

                    Fl_RGB_Image *imageBase = new Fl_RGB_Image(data, width, height, 3);
                    Fl_RGB_Image *image = (Fl_RGB_Image *)imageBase->copy(w, h);
                    fileLabel->setImage(image);
                }
            }
        }
    }

    delete[] data;

    _atexit();
    return 0;
}