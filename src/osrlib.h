#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <GL/glew.h>
#include <windows.h>

class TOffScreenWindowClass {
private:
    static TOffScreenWindowClass *s_instance;
    TOffScreenWindowClass();
public:
    static void CreateIfNotExists();
};

class TOffScreenRenderer {
public:
    TOffScreenRenderer(int width, int height);

    ~TOffScreenRenderer();

    void DumpToFile(const char *filename);

private:

    void CreateOffScreenContext();

    void CheckFramebufferStatus();

    void PrepareOffScreenBuffers();

    void ReadFromBuffer(unsigned char* color_rgba, float* depth_buffer);

    void ReleaseBuffers();

    HGLRC hglrc_;
    GLuint fbo_;
    GLuint rbo_;
    GLuint render_target_;
    int width_, height_;
};
