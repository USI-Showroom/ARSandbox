#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#endif
#include <QGLWidget>
#include <iostream>
#include "MainView.hpp"


MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _txt(0)
{ }

MainView::~MainView()
{
    if(_txt>0)
        glDeleteTextures(1, &_txt);


    glDeleteTextures(1, &_level0);
    glDeleteTextures(1, &_level1);
    glDeleteTextures(1, &_level2);
    glDeleteTextures(1, &_level3);
    glDeleteTextures(1, &_level4);
}

void MainView::setUniforms()
{
    GLuint heightT = _shader.uniformLocation("height");

    GLuint level0T = _shader.uniformLocation("level0");
    GLuint level1T = _shader.uniformLocation("level1");
    GLuint level2T = _shader.uniformLocation("level2");
    GLuint level3T = _shader.uniformLocation("level3");
    GLuint level4T = _shader.uniformLocation("level4");

    _shader.setUniformValue(heightT,0);

    _shader.setUniformValue(level0T,1);
    _shader.setUniformValue(level1T,2);
    _shader.setUniformValue(level2T,3);
    _shader.setUniformValue(level3T,4);
    _shader.setUniformValue(level4T,5);
}

void MainView::initializeGL() {
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);
    glClearColor(1, 1, 1, 1);


    QOpenGLShader vertShader(QOpenGLShader::Vertex);
    QFileInfo vertFile(":/shaders/HeightMap.vert");
    vertShader.compileSourceFile(vertFile.absoluteFilePath());


    QOpenGLShader fragShader(QOpenGLShader::Fragment);
    QFileInfo fragFile(":/shaders/HeightMap.frag");
    fragShader.compileSourceFile(fragFile.absoluteFilePath());

    _shader.addShader(&vertShader);
    _shader.addShader(&fragShader);

    _shader.link();

    setUniforms();


    _shader.bind();


    int M, m;

    glDisable(GL_BLEND);

    glGetIntegerv(GL_MAJOR_VERSION, &M);
    glGetIntegerv(GL_MINOR_VERSION, &m);

    std::cout << "OpenGL version " << M << "." << m<<std::endl;

    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    createTexture(":/terrain/level0",_level0);
    createTexture(":/terrain/level1",_level1);
    createTexture(":/terrain/level2",_level2);
    createTexture(":/terrain/level3",_level3);
    createTexture(":/terrain/level4",_level4);

    checkGLError();
}

void MainView::newKinectData(const UINT16 *data, int w, int h)
{

    if(_txt>0)
        glDeleteTextures(1, &_txt);

#ifdef WIN32
    glGenTextures(1, &_txt);
    glBindTexture(GL_TEXTURE_2D, _txt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
#else
    createTexture(":/test/depth",_txt);
#endif
    checkGLError();

    update();

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

void MainView::createTexture(const QString &path, GLuint &txtId)
{
    QImage img=QGLWidget::convertToGLFormat(QImage(path));

    glGenTextures(1, &txtId);
    glBindTexture(GL_TEXTURE_2D, txtId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
}

void MainView::paintGL()
{
    double size=1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);

    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    setUniforms();
    _shader.bind();
    
    glActiveTexture(GL_TEXTURE0);
    if (_txt > 0)
        glBindTexture(GL_TEXTURE_2D, _txt);


    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _level0);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _level1);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _level2);

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _level3);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _level4);

    glBegin(GL_QUADS);

    glTexCoord2f(0,0);
    glVertex2d(-size,-size);

    glTexCoord2f(0,1);
    glVertex2d(-size,size);

    glTexCoord2f(1,1);
    glVertex2d(size,size);

    glTexCoord2f(1,0);
    glVertex2d(size,-size);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
}