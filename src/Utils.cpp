#include <algorithm>

#include "Utils.hpp"

std::string getFileNameFromPath(const char *path)
{
    std::string name;
    int i = 0;

    for(; path[i] != '\0'; i++);

    for(; i >= 0 && path[i] != '/' && path[i] != '\\'; i--)
        name.push_back(path[i]);
    
    std::reverse(name.begin(), name.end());

    return name;
}

std::string getNameOnlyFromPath(const char *path)
{
    std::string name = getFileNameFromPath(path);
    std::string res;

    int i = 0;
    for(; name[i] != '\0' && name[i] != '.' ; i++)
        res += name[i];

    return res;
}

std::string composeOutputName(
    const char* inputName,
    const int scalex,
    const int scaley,
    const int datSize,
    const char* technique
)
{
    std::string name = "data/out/" + (getNameOnlyFromPath(inputName)) + "_" + technique;
    name += "_" + std::to_string(scalex) + "x" + std::to_string(scaley) + "_" + std::to_string(datSize/1000) + "K.png";
    return name;
}

std::string getExtension(const char* path)
{
    std::string name;
    int i = 0;

    for(; path[i] != '\0'; i++);

    for(; i >= 0 && path[i] != '.'; i--)
        name.push_back(path[i]);
    
    std::reverse(name.begin(), name.end());

    return name;
};