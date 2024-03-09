#include <fstream>
#include <iostream>

int main(int argc, const char* argv[])
{
    if(argc != 3) return EXIT_FAILURE;
    
    

    int labelSize;

    sscanf(argv[2], "%d", &labelSize);

    std::cout << argc << "\n";
    std::cout << argv[1] << "\n";
    std::cout << labelSize << "\n";


    std::fstream file(argv[1], std::ios::in | std::ios::binary);

    std::fstream out(std::string(argv[1])+"labelless", std::ios::out | std::ios::binary);

    if(!file || !out) return EXIT_FAILURE;

    char tmp[labelSize];

    char pixels[3072];

    while(!file.eof())
    {
        file.read(tmp, labelSize);
        
        file.read(pixels, 3072);

        out.write(pixels, 3072);
    }

    file.close();
    out.close();

    return EXIT_SUCCESS;
}