#ifndef OSRLIB_IO_H
#define OSRLIB_IO_H

#include "core.h"

void dumpBufferToRGBFile(const ImageBuffer &img, const std::string &filename);
void dumpBufferToPpmFile(const ImageBuffer &img, const std::string &filename);
void dumpTextureToPpmFile(GLuint renderedTextureId, const std::string &filename);
ImageBuffer dumpTextureToImageBuffer(GLuint renderedTextureId);
void loadObj(Mesh *mesh, const std::string &filename);

#endif // OSRLIB_IO_H
