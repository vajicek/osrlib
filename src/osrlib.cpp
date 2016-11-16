#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <GL/glew.h>
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "osrlib.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

TOffScreenWindowClass::TOffScreenWindowClass() {
    WNDCLASSEX wincl;
    wincl.hInstance = GetModuleHandle(0);
    wincl.lpszClassName = L"OffscreenRendererWindow";
    wincl.lpfnWndProc = WndProc;
    wincl.style = CS_OWNDC;
    wincl.cbSize = sizeof(WNDCLASSEX);
    wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    if (!RegisterClassEx(&wincl)) {
        throw std::exception();
    }
}

void TOffScreenWindowClass::CreateIfNotExists() {
    if (!s_instance) {
        s_instance = new TOffScreenWindowClass;
    }
}

TOffScreenWindowClass *TOffScreenWindowClass::s_instance = nullptr;

TOffScreenRenderer::TOffScreenRenderer(int width, int height) : width_(width), height_(height) {
    TOffScreenWindowClass::CreateIfNotExists();
    CreateOffScreenContext();
    PrepareOffScreenBuffers();
}

TOffScreenRenderer::~TOffScreenRenderer() {
    ReleaseBuffers();
    wglMakeCurrent (NULL, NULL) ;
    wglDeleteContext (hglrc_);
}

void TOffScreenRenderer::DumpToFile(const char *filename) {
    std::vector<unsigned char> color(width_ * height_ * 4);
    ReadFromBuffer(color.data(), nullptr);
    std::ofstream of(filename, std::ios_base::binary);
    of.write((char*)color.data(), color.size());
}

void TOffScreenRenderer::CreateOffScreenContext() {
    HWND hwnd = CreateWindowEx (
        0, L"OffscreenRendererWindow", 0,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width_, height_,
        HWND_DESKTOP, NULL,
        GetModuleHandle(0), NULL);

    HDC hdc = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof( pfd ));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    int format = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, format, &pfd);

    hglrc_ = wglCreateContext(hdc);
    wglMakeCurrent(hdc, hglrc_);
    glewInit();
}

void TOffScreenRenderer::CheckFramebufferStatus() {
    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    switch( status ) {
    case GL_FRAMEBUFFER_COMPLETE_EXT:
        break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
        throw std::exception("GL_FRAMEBUFFER_UNSUPPORTED_EXT!: ERROR");
        break;
    default:
        throw std::exception("Unknown exception!");
    }
}

void TOffScreenRenderer::PrepareOffScreenBuffers() {
    CheckFramebufferStatus();

    // Create buffers.
    glGenFramebuffersEXT(1, &fbo_);
    glGenRenderbuffersEXT(1, &rbo_);

    // Create textures.
    glGenTextures(1, &render_target_);
    glBindTexture(GL_TEXTURE_2D, render_target_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Set the render target.
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fbo_);
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rbo_);
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, width_, height_);

    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, rbo_, 0 );
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rbo_);
}

void TOffScreenRenderer::ReadFromBuffer(unsigned char* color_rgba, float* depth_buffer) {
    if (color_rgba) {
        glReadBuffer(GL_COLOR_ATTACHMENT0_EXT);
        glReadPixels(0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, color_rgba);
    }
    if (depth_buffer) {
        glReadBuffer(GL_DEPTH_ATTACHMENT_EXT);
        glReadPixels(0, 0, width_, height_, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer);
    }
}

void TOffScreenRenderer::ReleaseBuffers() {
    glDeleteTextures(1, &render_target_);
    glDeleteFramebuffersEXT(1, &fbo_);
    glDeleteRenderbuffersEXT(1, &rbo_);
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0 );
}
