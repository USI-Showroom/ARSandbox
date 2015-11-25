#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#include <gl\GLU.h>
#endif
#include <QGLWidget>
#include <iostream>
#include "MainView.hpp"
#include <cmath>


MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _checkerboard(this), _txt(0)
{ 
    _txtMinX=0;
    _txtMinY=0;

    _txtMaxX=1;
    _txtMaxY=1;

    _minH=0;
    _maxH=1;

    _top=1;
    _bottom=1;

    _setupMode=true;
    _showCheckerboard=false;
}

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

    GLuint minHT = _shader.uniformLocation("minH");
    GLuint maxHT = _shader.uniformLocation("maxH");

    GLuint level0T = _shader.uniformLocation("level0");
    GLuint level1T = _shader.uniformLocation("level1");
    GLuint level2T = _shader.uniformLocation("level2");
    GLuint level3T = _shader.uniformLocation("level3");
    GLuint level4T = _shader.uniformLocation("level4");

    _shader.setUniformValue(heightT,0);

    _shader.setUniformValue(minHT,_minH);
    _shader.setUniformValue(maxHT,_maxH);

    _shader.setUniformValue(level0T,1);
    _shader.setUniformValue(level1T,2);
    _shader.setUniformValue(level2T,3);
    _shader.setUniformValue(level3T,4);
    _shader.setUniformValue(level4T,5);

    checkGLError();
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

    QOpenGLShader fragCheckerShader(QOpenGLShader::Fragment);
    QFileInfo fragCheckerFile(":/shaders/Checkerboard.frag");
    fragCheckerShader.compileSourceFile(fragCheckerFile.absoluteFilePath());

    _shader.addShader(&vertShader);
    _shader.addShader(&fragShader);

    _shader.link();


    _checkerboard.addShader(&vertShader);
    _checkerboard.addShader(&fragCheckerShader);

    _checkerboard.link();


    int M, m, maxTxt;

    glDisable(GL_BLEND);

    glGetIntegerv(GL_MAJOR_VERSION, &M);
    glGetIntegerv(GL_MINOR_VERSION, &m);

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTxt);

    std::cout << "OpenGL version " << M << "." << m<<std::endl;

    std::cout << "OpenGL version " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Max textures " << maxTxt << std::endl;

    createTexture(":/terrain/level0",_level0);
    createTexture(":/terrain/level1",_level1);
    createTexture(":/terrain/level2",_level2);
    createTexture(":/terrain/level3",_level3);
    createTexture(":/terrain/level4",_level4);

    _funs.initializeOpenGLFunctions();
    _funs2.initializeOpenGLFunctions();

    checkGLError();
}

void MainView::newKinectData(const UINT16 *data, int w, int h)
{
    if(_txt>0)
        glDeleteTextures(1, &_txt);

#ifdef WIN32
#ifndef NO_KINECT
    glGenTextures(1, &_txt);
    glBindTexture(GL_TEXTURE_2D, _txt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
#else
    createTexture(":/test/depth", _txt);
#endif
#else
    createTexture(":/test/depth",_txt);
#endif
    checkGLError();

    update();
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

void MainView::activeTexture(GLenum texture)
{
#ifdef WIN32
    _funs.glActiveTexture(texture);
#else
    glActiveTexture(texture);
#endif
}


void MainView::textureCoords(GLenum texture, float u, float v, int i)
{

    float scaling=1;
    const float interY= (_bottom - _top) / (_bottom + _top);
    const float opi=1+interY;
    const float omi=1-interY;

    switch(i)
    {
        case 0:
        case 3:
        scaling=sqrt(_bottom*_bottom+opi*opi)/sqrt(_top*_top+omi*omi)+1;
        break;

        case 1:
        case 2:
        scaling=sqrt(_top*_top+omi*omi)/sqrt(_bottom*_bottom+opi*opi)+1;
        break;
    }

    const float us=u*scaling;
    const float vs=v*scaling;

#ifdef WIN32
    _funs2.glMultiTexCoord3f(texture,us,vs,scaling);
#else
    glMultiTexCoord3f(texture,us,vs,scaling);
#endif
}

void MainView::keyPressEvent(QKeyEvent *e)
{
    static const float offset=0.01f;
#ifdef NO_KINECT
    static const float hoffset = offset;
#else
    static const float hoffset = 10.0f;
#endif

    const int key=e->key();

    if(key==Qt::Key_F5)
        _setupMode=!_setupMode;

    if(!_setupMode) return;

    const bool shift = e->modifiers() & Qt::ShiftModifier;
    const float step = offset / (shift ? 10.0 : 1.0);
    const float hstep = hoffset / (shift ? 10.0 : 1.0);



    switch (key)
    {
        case Qt::Key_W:
        {
            _txtMinY -= step;
            _txtMaxY -= step;
            break;
        }
        case Qt::Key_S:
        {
            _txtMinY += step;
            _txtMaxY += step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_A:
        {
            _txtMinX += step;
            _txtMaxX += step;
            break;
        }
        case Qt::Key_D:
        {
            _txtMinX -= step;
            _txtMaxX -= step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_E:
        {
            _txtMinX -= step;
            _txtMinY -= step;
            _txtMaxX += step;
            _txtMaxY += step;
            break;
        }
        case Qt::Key_Q:
        {
            _txtMinX += step;
            _txtMinY += step;
            _txtMaxX -= step;
            _txtMaxY -= step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_T:
        {
            _minH += hstep;
            break;
        }
        case Qt::Key_R:
        {
            _minH -= hstep;
            break;
        }
///////////////////////////////////////
        case Qt::Key_G:
        {
            _maxH += hstep;
            break;
        }
        case Qt::Key_F:
        {
            _maxH -= hstep;
            break;
        }
///////////////////////////////////////
        case Qt::Key_C:
        {
            _showCheckerboard = !_showCheckerboard;
            break;
        }
///////////////////////////////////////
        case Qt::Key_V:
        {
            _top -= step;
            break;
        }
        case Qt::Key_B:
        {
            _top += step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_N:
        {
            _bottom -= step;
            break;
        }
        case Qt::Key_M:
        {
            _bottom += step;
            break;
        }


    }

    std::cout <<"min h: "<< _minH << " max h: " << _maxH << std::endl;

    update();
}

void MainView::paintGL()
{
    static const double textSize = 2;
    static const double size = 1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);

    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    if(_showCheckerboard)
        _checkerboard.bind();
    else{
        _shader.bind();
        setUniforms();
    }

    activeTexture(GL_TEXTURE0);
    if (_txt > 0)
        glBindTexture(GL_TEXTURE_2D, _txt);


    activeTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, _level0);

    activeTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _level1);

    activeTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _level2);

    activeTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _level3);

    activeTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _level4);

    glBegin(GL_QUADS);

    textureCoords(GL_TEXTURE0, _txtMaxX, _txtMaxY,0);
    textureCoords(GL_TEXTURE1, 0, 0, 0);
    glVertex2d(-_bottom,-size);

    textureCoords(GL_TEXTURE0, _txtMaxX, _txtMinY,1);
    textureCoords(GL_TEXTURE1, 0, textSize, 1);
    glVertex2d(-_top,size);

    textureCoords(GL_TEXTURE0, _txtMinX, _txtMinY,2);
    textureCoords(GL_TEXTURE1, textSize, textSize, 2);
    glVertex2d(_top,size);

    textureCoords(GL_TEXTURE0, _txtMinX, _txtMaxY,3);
    textureCoords(GL_TEXTURE1, textSize, 0, 3);
    glVertex2d(_bottom,-size);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    checkGLError();
}