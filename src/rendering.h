#ifndef RENDERING_H_
#define RENDERING_H_

#include <vector>

#include "core.h"

void renderMesh(const Rendering &rendering, const Mesh &mesh);
void renderNodes(const Rendering &rendering, const std::vector<RenderNode> &renderNode);

#endif  // RENDERING_H_
