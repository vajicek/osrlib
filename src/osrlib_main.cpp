#include "osrlib.h"
#include "osrlib_test.h"
#include "osrlib_io.h"

int main(void) {
    //const Rendering rendering {{1 * 512, 1 * 512}};
    //renderToOpenGLWindowLoop(rendering, renderExample);
    //renderToOpenGlWindowOffScreenPass(rendering, "filename.ppm", renderExample);
    //renderToOpenGlWindowOffScreenPass({{1 * 512, 1 * 512}}, "filename1.ppm", renderExample);
    //renderToOpenGlWindowOffScreenPass({{2 * 512, 2 * 512}}, "filename2.ppm", renderExample);
    //renderToOpenGlWindowOffScreenPass({{4 * 512, 4 * 512}}, "filename4.ppm", renderExample);
    //renderToOpenGlWindowOffScreenPass({{2048, 2048}}, "filename4.ppm", renderExample);
    //renderToOpenGlWindowOffScreenPass({{8 * 512, 8 * 512}}, "filename8.ppm", renderExample);

    //renderToOpenGLWindowLoop({{4 * 512, 4 * 512}}, RenderExample);

    //renderToTextureBufferPass({{1 * 512, 1 * 512}}, "filename1.ppm", renderExample);
    // renderToTextureBufferPass({{2 * 512, 2 * 512}}, "filename2.ppm", renderExample);
    // renderToTextureBufferPass({{4 * 512, 4 * 512}}, "filename4.ppm", renderExample);
    // renderToTextureBufferPass({{8 * 512, 8 * 512}}, "filename8.ppm", renderExample);

    Mesh mesh;
    loadObj(&mesh, "../bunny.obj");

    renderToTextureBufferPass({{2 * 512, 2 * 512}}, "bunny.ppm",
        [&mesh](const Rendering &rendering){ renderMesh(rendering, mesh); } );

    return 0;
}

/*

Refs:

    Stackoverflow:
    https://stackoverflow.com/questions/2896879/windowless-opengl/48947103#48947103

    Offscreen rendering:
    https://sidvind.com/wiki/Opengl/windowless


    https://blog.sigsegowl.xyz/opengl-offscreen-software-rendering-on-a-server/
*/