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

#include <QImage>


MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _txt(0),
p0(-1,-1), p1(-1,1), p2(1,1), p3(1,-1),
_gameTexture(0)
{ 
#ifdef NO_KINECT
    _minH=0;
    _maxH=1;
#else
    _minH=1200;
    _maxH=1400;
#endif
    _setupMode=true;

    _currentCorner=0;

    _initialized=false;

    _tmpGameImage=NULL;
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

    if(_gameTexture>0)
        glDeleteTextures(1, &_gameTexture);
}

void MainView::setUniforms()
{
    GLuint heightT = _shader.uniformLocation("height");

    GLuint gameT = _shader.uniformLocation("gameTexture");

    GLuint minHT = _shader.uniformLocation("minH");
    GLuint maxHT = _shader.uniformLocation("maxH");

    GLuint level0T = _shader.uniformLocation("level0");
    GLuint level1T = _shader.uniformLocation("level1");
    GLuint level2T = _shader.uniformLocation("level2");
    GLuint level3T = _shader.uniformLocation("level3");
    GLuint level4T = _shader.uniformLocation("level4");

    

    _shader.setUniformValue(heightT,0);

    _shader.setUniformValue(gameT,1);

    _shader.setUniformValue(minHT,_minH);
    _shader.setUniformValue(maxHT,_maxH);

    _shader.setUniformValue(level0T,2);
    _shader.setUniformValue(level1T,3);
    _shader.setUniformValue(level2T,4);
    _shader.setUniformValue(level3T,5);
    _shader.setUniformValue(level4T,6);

    

    checkGLError("setUniforms");
}

void MainView::initializeGL() {
    _initialized=true;
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

    if(_tmpGameImage){
        newGameImage(*_tmpGameImage);
        _tmpGameImage=NULL;
    }

    checkGLError("init");
}

void MainView::newGameImage(const QImage &img)
{
    if(!_initialized)
    {
        _tmpGameImage=&img;
        return;
    }

    if(_gameTexture>0)
        glDeleteTextures(1, &_gameTexture);

    QImage tmp=QGLWidget::convertToGLFormat(img);
    glGenTextures(1, &_gameTexture);
    glBindTexture(GL_TEXTURE_2D, _gameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tmp.width(), tmp.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp.bits());

    checkGLError("newGameImage");
    update();
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
    checkGLError("kinect");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
#else
    createTexture(":/test/depth", _txt);
#endif
#else
    createTexture(":/test/depth",_txt);
#endif
    checkGLError("new data");

    update();
}

void MainView::checkGLError(const std::string &msg)
{
    GLenum error = GL_NO_ERROR;
    do {
        error = glGetError();
        if (error != GL_NO_ERROR)
            std::cerr<<"["<<msg<<"] "<<error<<std::endl;
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
    const Point2d inter(
        ( (p0.x()*p2.y()-p0.y()*p2.x())*(p1.x()-p3.x()) - (p0.x()-p2.x())*(p1.x()*p3.y()-p1.y()*p3.x()) )/
        ( (p0.x()-p2.x())*(p1.y()-p3.y()) - (p0.y()-p2.y())*(p1.x()-p3.y()) ),

        ( (p0.x()*p2.y()-p0.y()*p2.x())*(p1.y()-p3.y()) - (p0.y()-p2.y())*(p1.x()*p3.y()-p1.y()*p3.x()) )/
        ( (p0.x()-p2.x())*(p1.y()-p3.y()) - (p0.y()-p2.y())*(p1.x()-p3.y()) )
        );

    switch(i)
    {
        case 0:
        scaling=(p0-inter).norm()/(inter-p2).norm()+1; break;
        case 1:
        scaling=(p1-inter).norm()/(inter-p3).norm()+1; break;
        case 2:
        scaling=(p2-inter).norm()/(inter-p0).norm()+1; break;
        case 3:
        scaling=(p3-inter).norm()/(inter-p1).norm()+1; break;
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

    if(key==Qt::Key_F5){
        _setupMode=!_setupMode;
        emit toggleSetupMode(_setupMode,_minH,_maxH);
    }

    if(!_setupMode) 
    {
        emit keyPress(key);
        return;
    }

    const bool shift = e->modifiers() & Qt::ShiftModifier;
    const float step = offset / (shift ? 10.0 : 1.0);
    const float hstep = hoffset / (shift ? 10.0 : 1.0);

    Point2d dir;



    switch (key)
    {
        case Qt::Key_1: _currentCorner=0; break;
        case Qt::Key_2: _currentCorner=1; break;
        case Qt::Key_3: _currentCorner=2; break;
        case Qt::Key_4: _currentCorner=3; break;
////////////////////////////////////////////
        case Qt::Key_W:
        {
            dir.x() = -step;
            break;
        }
        case Qt::Key_S:
        {
            dir.x() = +step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_A:
        {
            dir.y() = +step;
            break;
        }
        case Qt::Key_D:
        {
            dir.y() = -step;
            break;
        }
///////////////////////////////////////
        case Qt::Key_Y:
        {
            _minH += hstep;
            break;
        }
        case Qt::Key_H:
        {
            _minH -= hstep;
            break;
        }
///////////////////////////////////////
        case Qt::Key_U:
        {
            _maxH += hstep;
            break;
        }
        case Qt::Key_J:
        {
            _maxH -= hstep;
            break;
        }
    }

    switch(_currentCorner)
    {
        case 0: p0+=dir; break;
        case 1: p1+=dir; break;
        case 2: p2+=dir; break;
        case 3: p3+=dir; break;
    }

    std::cout <<"min h: "<< _minH << " max h: " << _maxH << std::endl;

    update();
}

void MainView::paintGL()
{
    static const double nTiles = 10;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glDisable(GL_BLEND);

    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);

    
    _shader.bind();
    setUniforms();
    

    activeTexture(GL_TEXTURE0);
    if (_txt > 0)
        glBindTexture(GL_TEXTURE_2D, _txt);


    activeTexture(GL_TEXTURE1);
    if (_gameTexture > 0)
        glBindTexture(GL_TEXTURE_2D, _gameTexture);


    activeTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, _level0);

    activeTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, _level1);

    activeTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, _level2);

    activeTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, _level3);

    activeTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, _level4);

    glBegin(GL_QUADS);

    textureCoords(GL_TEXTURE0, 0, 0, 0);
    textureCoords(GL_TEXTURE1, 0, 0, 0);
    glVertex2d(p0.x(), p0.y());

    textureCoords(GL_TEXTURE0, 0, 1, 1);
    textureCoords(GL_TEXTURE1, 0, nTiles, 1);
    glVertex2d(p1.x(), p1.y());

    textureCoords(GL_TEXTURE0, 1, 1, 2);
    textureCoords(GL_TEXTURE1, nTiles, nTiles, 2);
    glVertex2d(p2.x(), p2.y());

    textureCoords(GL_TEXTURE0, 1, 0, 3);
    textureCoords(GL_TEXTURE1, nTiles, 0, 3);
    glVertex2d(p3.x(), p3.y());

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    checkGLError("draw");
}