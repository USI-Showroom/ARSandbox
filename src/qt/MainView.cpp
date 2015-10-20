#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#endif

#include <iostream>
#include "MainView.hpp"


MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _txt(0)
{ }

MainView::~MainView()
{
    if(_txt>0)
        glDeleteTextures(1, &_txt);
}

void MainView::initializeGL() {
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);
    glClearColor(1, 1, 1, 1);

    QOpenGLShader shader(QOpenGLShader::Fragment);
    QFileInfo file(":/shaders/HeightMap");
    shader.compileSourceFile(file.absoluteFilePath());

    _shader.addShader(&shader);
    _shader.link();

    int M, m;

    glDisable(GL_BLEND);

    glGetIntegerv(GL_MAJOR_VERSION, &M);
    glGetIntegerv(GL_MINOR_VERSION, &m);

    std::cout << "OpenGL version " << M << "." << m<<std::endl;

    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

void MainView::newKinectData(const UINT16 *data, int w, int h)
{
/*_texture.destroy();
_texture.setSize(w, h);
_texture.setFormat(QOpenGLTexture::R16U);
_texture.allocateStorage(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16);

_texture.bind();
_texture.setData(QOpenGLTexture::Red_Integer, QOpenGLTexture::UInt16, data);
_texture.generateMipMaps();
_texture.create();*/

if(_txt>0)
    glDeleteTextures(1, &_txt);

glActiveTexture(GL_TEXTURE0);

glGenTextures(1, &_txt);
glBindTexture(GL_TEXTURE_2D, _txt);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
// glGenerateMipmap(GL_TEXTURE_2D);

checkGLError();
glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);


/*uchar *datac = new uchar[w*h];

for (int i = 0; i < w*h; ++i){
USHORT depth = data[i];
BYTE intensity = static_cast<BYTE>((depth >= 100) && (depth <= 4500) ? (depth % 256) : 0);
datac[i] = intensity;
}


QImage tmp(datac, w, h, QImage::Format_Grayscale8);
_texture.destroy();
_texture.setData(tmp);

delete[] datac;*/




checkGLError();
paintGL();

checkGLError();
}

void MainView::checkGLError()
{
    GLenum error = GL_NO_ERROR;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
            std::cerr<<error<<std::endl;
    } while (error != GL_NO_ERROR);
}

void MainView::paintGL()
{
    double size=1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);

    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    glActiveTexture(GL_TEXTURE0);
    if (_txt > 0)
        glBindTexture(GL_TEXTURE_2D, _txt);

    _shader.bind();

    glBegin(GL_QUADS);

    glTexCoord2f(0,1);
    glVertex2d(-size,-size);

    glTexCoord2f(0,0);
    glVertex2d(-size,size);

    glTexCoord2f(1,0);
    glVertex2d(size,size);

    glTexCoord2f(1,1);
    glVertex2d(size,-size);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}