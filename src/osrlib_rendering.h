#ifndef OSRLIB_RENDERING_H
#define OSRLIB_RENDERING_H

#include "osrlib.h"

GLuint createIndicesBuffer(const Mesh &mesh);
void renderMesh(const Rendering &rendering, const Mesh &mesh);
void renderNodes(const Rendering &rendering, const std::vector<RenderNode> &renderNode);

#endif // OSRLIB_RENDERING_H
