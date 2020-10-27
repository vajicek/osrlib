#include "core.h"

#ifndef OSRLIB_IO_H
#define OSRLIB_IO_H

void dumpBufferToRGBFile(const ImageBuffer &img, const std::string &filename);
void dumpToPpmFile(const ImageBuffer &img, const std::string &filename);
void dumpTextureToFile(GLuint renderedTextureId, const std::string &filename);
ImageBuffer dumpTextureToImageBuffer(GLuint renderedTextureId);
void loadObj(Mesh *mesh, const std::string &filename);

#endif // OSRLIB_IO_H
