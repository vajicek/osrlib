#ifndef IO_H_
#define IO_H_

#include <string>

#include "core.h"

void dumpBufferToRGBFile(const ImageBuffer &img, const std::string &filename);
void dumpBufferToPpmFile(const ImageBuffer &img, const std::string &filename);
void dumpTextureToPpmFile(GLuint renderedTextureId, const std::string &filename);
ImageBuffer dumpTextureToImageBuffer(GLuint renderedTextureId);
void loadObj(Mesh *mesh, const std::string &filename);

#endif  // IO_H_
