#pragma once

struct sharedMemory
{
    bool status;
    bool writing;
    int progress;
    int total;
};

void initializeSharedMemory();
void finalizeSharedMemory();
void writeSharedMemory(sharedMemory mem);
void readSharedMemory(sharedMemory &mem);

#ifdef SHM_UTILS_IMPLEMENTATION
#ifdef _WIN32
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#else
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#endif

#ifdef _WIN32
HANDLE hMapFile;
LPCTSTR pBuf;

void initializeSharedMemory()
{
    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,             // use paging file
        NULL,                             // default security
        PAGE_READWRITE,                   // read/write access
        0,                                // maximum object size (high-order DWORD)
        4096,                             // maximum object size (low-order DWORD)
        _T("Local\\Projet-Compression")); // name of mapping object
    if (hMapFile == NULL)
    {
        printf("Could not create file mapping object (%d).\n", (int)GetLastError());
        return;
    }
    pBuf = (LPTSTR)MapViewOfFile(hMapFile,            // handle to map object
                                 FILE_MAP_ALL_ACCESS, // read/write permission
                                 0,
                                 0,
                                 4096);
    if (pBuf == NULL)
    {
        printf("Could not map view of file (%d).\n", (int)GetLastError());
        CloseHandle(hMapFile);
        return;
    }
}

void finalizeSharedMemory()
{
    UnmapViewOfFile(pBuf);
    CloseHandle(hMapFile);
}

void writeSharedMemory(sharedMemory mem)
{
    CopyMemory((PVOID)pBuf, &mem, sizeof(mem));
}

void readSharedMemory(sharedMemory &mem)
{
    CopyMemory(&mem, (PVOID)pBuf, sizeof(mem));
}

#else
int fd;
char *pBuf;

void initializeSharedMemory()
{
    fd = shm_open("/projet-compression", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        perror("shm_open");
        return;
    }
    if (ftruncate(fd, 4096) == -1)
    {
        perror("ftruncate");
        return;
    }
    pBuf = (char *)mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pBuf == MAP_FAILED)
    {
        perror("mmap");
        return;
    }
}

void finalizeSharedMemory()
{
    munmap(pBuf, 4096);
    close(fd);
    shm_unlink("/myregion");
}

void writeSharedMemory(sharedMemory mem)
{
    memcpy(pBuf, &mem, sizeof(mem));
}

void readSharedMemory(sharedMemory &mem)
{
    memcpy(&mem, pBuf, sizeof(mem));
}
#endif
#endif