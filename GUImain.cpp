#define SHM_UTILS_IMPLEMENTATION
#include "shmUtils.hpp"
#include <vector>
#include <string>
#include <thread>
#include <signal.h>
#include <iostream>
#include <glm/glm.hpp>
using namespace std;

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

// temporary until we have an actual GUI
// basically just a proof of concept,
// calls the main program with subprocess flag
// and reads the shared memory to display progress
// the actual GUI app will essentially do the same thing but with a GUI
// allowing the user to select the input file, the settings and preview the output
int main()
{
    initializeSharedMemory();

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGABRT, signalHandler);

    sharedMemory mem;
    mem.status = true;
    mem.progress = 0;
    mem.total = 100;
    writeSharedMemory(mem);
#ifdef _WIN32
    std::vector<std::string> args = {".\\data\\in\\cat.png", "64", "RGB", "L3DIFF", ".\\out.png", "--subprocess"};
#else
    std::vector<std::string> args = {"./data/in/cat.png", "64", "RGB", "L3DIFF", "./out.png", "--subprocess"};
#endif
    int rslt = createChild(args);
    if (rslt)
    {
        printf("Error creating child process\n");
        return rslt;
    }

    while (mem.status)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        readSharedMemory(mem);
        int progress = mem.progress;
        int iSize = mem.total;
        std::cout << "\r[";
        for (int i = 0; i < 50; i++)
        {
            std::cout << (i < (progress - 1) * 50 / iSize ? "=" : (i == progress * 50 / iSize ? ">" : " "));
        }

        std::cout << "] " << progress * 100 / iSize << "%" << std::flush;
    }

    std::cout << "\r[==================================================] 100%\n";

    _atexit();
    return 0;
}