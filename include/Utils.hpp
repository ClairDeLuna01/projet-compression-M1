#pragma once

#include <string>

extern bool subprocess;

std::string getFileNameFromPath(const char *path);

std::string getNameOnlyFromPath(const char *path);

std::string composeOutputName(
    const char *inputName,
    const int scalex,
    const int scaley,
    const int datSize,
    const char *technique);

std::string getExtension(const char *path);