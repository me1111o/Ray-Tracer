#ifndef __PPM_IMAGELOADER_H_
#define __PPM_IMAGELOADER_H_

#include "ImageLoader.h"
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

class PPMImageLoader : public ImageLoader
{
public:
    PPMImageLoader() : image(nullptr), width(0), height(0) {}

    virtual ~PPMImageLoader()
    {
        if (image)
            delete[] image;
    }

    void load(const std::string &filename)
    {
        std::ifstream fp(filename.c_str());
        if (!fp.is_open())
            throw std::invalid_argument("File not found!");

        std::cout << "Image file opened" << std::endl;

        std::stringstream input;
        std::string line;
        while (std::getline(fp, line))
        {
            if (!line.empty() && line[0] != '#')
            {
                input << line << std::endl;
            }
        }
        fp.close();

        std::string format;
        input >> format;
        if (format != "P3")
        {
            throw std::runtime_error("Unsupported PPM format. Only ASCII PPM (P3) is supported.");
        }

        input >> width >> height;
        int factor;
        input >> factor;

        image = new GLubyte[3 * width * height];

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                int r, g, b;
                input >> r >> g >> b;
                int index = 3 * ((height - 1 - i) * width + j);
                image[index] = static_cast<GLubyte>(r);
                image[index + 1] = static_cast<GLubyte>(g);
                image[index + 2] = static_cast<GLubyte>(b);
            }
        }
    }

    GLubyte *getImageData() const { return image; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    GLubyte *image;
    int width;
    int height;
};

#endif
