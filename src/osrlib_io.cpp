#include "osrlib_io.h"

#include <GL/glew.h>
#include <boost/algorithm/string.hpp>

#include <cassert>
#include <iostream>
#include <fstream>
#include <sstream>

void dumpBufferToRGBFile(const ImageBuffer &img, const std::string &filename) {
    std::ofstream fout;
    fout.open(filename, std::ios::binary | std::ios::out);
    fout.write(reinterpret_cast<const char*>(img.data()), sizeof(u_int8_t) * img.size());
    fout.close();
}

void dumpToPpmFile(const ImageBuffer &img, const std::string &filename) {
    assert(img.width() * img.height() * 3 == img.size());
    std::ofstream fout;
    fout.open(filename, std::ios::out);
    fout << "P3\n";
    fout << img.width() << " " << img.height() << "\n";
    fout << "255\n";
    for (int i = 0; i < img.size() / 3; i++) {
        for (int j = 0; j < 3; j++) {
            fout << static_cast<int>(img.data()[i * 3 + j]) << " ";
        }
        fout << "\n";
    }
    fout.close();
}

void dumpPixelsToFile(const Rendering &rendering, const std::string &filename) {
    ImageBuffer img(rendering.width(), rendering.height());

    glReadPixels(0, 0, rendering.width(), rendering.height(),
        GL_RGB, GL_UNSIGNED_BYTE,
        img.data());

    dumpToPpmFile(img, filename);
}

void dumpTextureToFile(GLuint renderedTextureId, const std::string &filename) {
    GLint width, height;
    glBindTexture(GL_TEXTURE_2D, renderedTextureId);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

    ImageBuffer img(width, height);
    glActiveTexture(GL_TEXTURE0);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, img.data());

    dumpToPpmFile(img, filename);
}

float strToFloat(const std::string &token) {
    return atof(token.c_str());
}

uint32_t strToUInt32(const std::string &token) {
    return atoi(token.c_str()) - 1;
}

void loadObj(Mesh *mesh, const std::string &filename) {
    assert(mesh != nullptr);
    std::ifstream fin(filename);
    if (!fin.is_open()) {
        throw std::runtime_error("Cannot open file: " + filename);
    }
    std::string line;
    int line_no = 0;
    while (std::getline(fin, line)) {
        boost::trim(line);
        if (line.rfind("#") == 0) {
            continue;
        }
        std::vector<std::string> tokens;
        boost::split(tokens, line, [](char c){ return c == ' '; });
        if (tokens[0] == "v") {
            mesh->vertices.push_back(glm::fvec3(
                strToFloat(tokens[1]),
                strToFloat(tokens[2]),
                strToFloat(tokens[3])));
        } else if (tokens[0] == "f") {
            mesh->face_vertices.push_back(glm::ivec3(
                strToUInt32(tokens[1]),
                strToUInt32(tokens[2]),
                strToUInt32(tokens[3])));
        } else if (tokens[0] == "vt") {
        } else if (tokens[0] == "vn") {
        } else if (tokens[0] == "vp") {
        }
        line_no ++;
    }
}
