#pragma once
#include "BaseContentWriter.h"
#include <fstream>
#include <iostream>
#include <string>

class FileContentWriter: public BaseContentWriter
{
public:
    FileContentWriter(std::string file_name): fileName(file_name) {}
    bool WriteToMedia(std::string content) override {
        std::ofstream sourceStream(fileName, std::ios::app); // Open file in append mode

        if (!sourceStream.is_open())
        {
            std::cerr << "Error opening file: " << fileName << std::endl;
            return false;
        }

        sourceStream << content << "\r\n"; // Write content to the file
        sourceStream.close();              // Close the file

        return true;
    }

private:
    std::string fileName;
};
