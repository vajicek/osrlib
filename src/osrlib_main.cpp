#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <GL/glew.h>
#include <windows.h>

#include "osrlib.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "glew32s.lib")
#pragma comment(lib, "glew32sd.lib")
#pragma comment(lib, "glew32d.lib")
#pragma comment(lib, "osrlib.lib")


int RenderExample(int width, int height,
    const double xMax =  10, const double xMin = -10, const double yMax =  10, const double yMin = -10,
    const double nearDist = 0, const double farDist  = 100,
    const double camPosX = 0, const double camPosY = 0, const double camPosZ = 0,
    const double lookAtX = 0, const double lookAtY = 0, const double lookAtZ = -1,
    const double upX = 0, const double upY = 1, const double upZ = 0) {

    // Set the render states.
    glEnable( GL_DEPTH_TEST );
    glDepthMask( 1 );
    glDepthFunc( GL_LEQUAL );

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.0);

    // Draw on the buffer.
    glClearColor( 0.f, 0.f, 0.f, 1.f );
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport( 0, 0, width, height);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho( xMin, xMax, yMin, yMax, nearDist, farDist );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( camPosX, camPosY, camPosZ, lookAtX, lookAtY, lookAtZ, upX, upY, upZ );

    glColor3f(1,1,0); // yellow
    glTranslatef( 0, 0, -60 );
    glBegin( GL_QUADS );
    glVertex3f( xMin, yMin, 0 );
    glVertex3f( xMax, yMin, 0 );
    glVertex3f( xMax, yMax, 0 );
    glVertex3f( xMin, yMax, 0 );
    glEnd();

    return 0;
}

int main(int argc, char **argv) {
    TOffScreenRenderer renderer(2000, 2000);
    RenderExample(2000, 2000);
    renderer.DumpToFile("d:/output.pbm");
    return 0;
}