#include <QFileInfo>

#ifdef WIN32
#include <windows.h>
#include <gl\gl.h>
#include <gl\GLU.h>
#endif
#include <QGLWidget>
#include <QDateTime>
#include <iostream>
#include "MainView.hpp"
#include <cmath>
#include <fstream>
#include "Triangulator.hpp"


MainView::MainView(QWidget *parent)
: super(parent), _shader(this), _checkerboard(this), _gameTexture(0),

p0(-1,-1), p1(-1,1), p2(1,1), p3(1,-1),
txt0(0, 0), txt1(0, 1), txt2(1, 1), txt3(1, 0),

_zoom(":/interaction/zoom"), _move(":/interaction/move"), 
_corner0(":/interaction/0"), _corner1(":/interaction/1"), _corner2(":/interaction/2"), _corner3(":/interaction/3")
{ 
#ifdef NO_KINECT
    _minH=0;
    _maxH=255;
#else
    _minH=1200;
    _maxH=1340;
#endif
    _setupMode=true;
    _moveTexture = false;

    _saveNextMesh=false;

    _currentCorner=4;

    _initialized=false;

	_showSand = false;

    for(int i=0; i<nLevels; ++i) {
        _level[i]=0; 
    }
}

MainView::~MainView()
{
    for(int i=0; i<_txt.size();++i)
        glDeleteTextures(1, &_txt[i]);

    for(int i=0; i<nLevels; ++i)
        glDeleteTextures(1, &_level[i]);

    if(_gameTexture>0)
        glDeleteTextures(1, &_gameTexture);
}

void MainView::setUniforms()
{
    GLuint gameT = _shader.uniformLocation("gameTexture");

    GLuint minHT = _shader.uniformLocation("minH");
    GLuint maxHT = _shader.uniformLocation("maxH");


    GLuint heightT[nSmoothing];
    for(int i=0; i<nSmoothing; ++i) {
        QString tmp("height");
        tmp+=QString::number(i);
        heightT[i] = _shader.uniformLocation(tmp.toStdString().c_str());
    }


    GLuint levelT[nLevels];
    for(int i=0; i<nLevels; ++i) {
        QString tmp("level");
        tmp+=QString::number(i);
        levelT[i] = _shader.uniformLocation(tmp.toStdString().c_str());
    }


    for(int i=0; i<nSmoothing; ++i) {
        _shader.setUniformValue(heightT[i],i);
    }

    for(int i=0; i<nLevels; ++i) {
        _shader.setUniformValue(levelT[i],nSmoothing + i);
    }
    
    _shader.setUniformValue(gameT,nSmoothing+nLevels);

    _shader.setUniformValue(minHT,_minH);
    _shader.setUniformValue(maxHT,_maxH);

    checkGLError("setUniforms");
}

void MainView::initializeGL() {
    _initialized=true;
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);

    glShadeModel(GL_SMOOTH);
    glClearColor(0, 0, 0, 1);


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

    assert(nSmoothing+nLevels+1<=maxTxt);

    reloadTerainTextures(0);

    _funs.initializeOpenGLFunctions();
    _funs2.initializeOpenGLFunctions();

    checkGLError("init");
}

void MainView::reloadTerainTextures(const int terrainIndex)
{
    _currentGame=terrainIndex;
    for(int i=0; i<nLevels;++i){
        if(_level[i]>0)
            glDeleteTextures(1, &_level[i]);
    }

    for(int i=0; i<nLevels;++i){
        QString tmp(":/terrain/");
        tmp+=QString::number(terrainIndex)+QString::fromStdString("/level");
        tmp+=QString::number(i);

        createTexture(tmp,_level[i]);
    }
}

void MainView::newGameImage(const QImage &img)
{
    if(!_initialized) return;
    
    if(_gameTexture>0)
        glDeleteTextures(1, &_gameTexture);

	const QImage tmp = QGLWidget::convertToGLFormat(img);
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
    if(_txt.size()>=nSmoothing)
    {
        glDeleteTextures(1, &_txt.back());
        _txt.pop_back();
    }

    GLuint tmp;

#ifdef WIN32
#ifndef NO_KINECT
    glGenTextures(1, &tmp);
    glBindTexture(GL_TEXTURE_2D, tmp);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    checkGLError("kinect");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16UI, w, h, 0, GL_RED_INTEGER, GL_UNSIGNED_SHORT, data);
#else
    createTexture(":/test/depth", tmp);
#endif
#else
    createTexture(":/test/depth",tmp);
#endif

    _txt.push_front(tmp);
    checkGLError("new data");

    if(_saveNextMesh)
    {
        _saveNextMesh=false;
        saveMesh(data);
    }

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

    const double x1=p0.x(), y1=p0.y();
    const double x2=p2.x(), y2=p2.y();

    const double x3=p1.x(), y3=p1.y();
    const double x4=p3.x(), y4=p3.y();

    const Point2d inter(
        ( (x1*y2-y1*x2)*(x3-x4)-(x1-x2)*(x3*y4-y3*x4) )/
        ( (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4) ),

        ( (x1*y2-y1*x2)*(y3-y4)-(y1-y2)*(x3*y4-y3*x4) )/
        ( (x1-x2)*(y3-y4)-(y1-y2)*(x3-x4) )
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

	if (key == Qt::Key_Escape) {
		this->close();
		exit(0);
	}

    if(key == Qt::Key_P)
    {
        _saveNextMesh=true;
    }

	if (key == Qt::Key_Minus)
	{
		_showSand = !_showSand;
	}

    if(key==Qt::Key_F5){
        _setupMode=!_setupMode;
        UnitSquareMapping mapping(txt0,txt1,txt2,txt3);
        emit toggleSetupMode(_setupMode, _minH, _maxH, _currentGame, mapping);
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

    double mult=1;


    switch (key)
    {
        case Qt::Key_F1: reloadTerainTextures(0); break;
        case Qt::Key_F2: reloadTerainTextures(1); break;
        case Qt::Key_F3: reloadTerainTextures(2); break;

        case Qt::Key_1: _currentCorner=0; break;
        case Qt::Key_2: _currentCorner=1; break;
        case Qt::Key_3: _currentCorner=2; break;
        case Qt::Key_4: _currentCorner=3; break;
        case Qt::Key_5: _currentCorner=4; break;
        case Qt::Key_6: _currentCorner=5; break;

        case Qt::Key_9: saveState(); break;
        case Qt::Key_0: loadState(); break;

        case Qt::Key_Space: _moveTexture = !_moveTexture; break;
////////////////////////////////////////////
        case Qt::Key_A:
        {
            dir.x() = -step;
            mult=-1;
            break;
        }
        case Qt::Key_D:
        {
            dir.x() = +step;
            mult=-1;
            break;
        }
///////////////////////////////////////
        case Qt::Key_W:
        {
            dir.y() = +step;
            mult=1;
            break;
        }
        case Qt::Key_S:
        {
            dir.y() = -step;
            mult=1;
            break;
        }
///////////////////////////////////////
        case Qt::Key_U:
        {
            _minH += hstep;
            break;
        }
        case Qt::Key_J:
        {
            _minH -= hstep;
            break;
        }
///////////////////////////////////////
        case Qt::Key_I:
        {
            _maxH += hstep;
            break;
        }
        case Qt::Key_K:
        {
            _maxH -= hstep;
            break;
        }
    }

    if (_moveTexture)
    {
        switch (_currentCorner)
        {
            case 0: txt0 += dir; break;
            case 1: txt1 += dir; break;
            case 2: txt2 += dir; break;
            case 3: txt3 += dir; break;
            case 4: txt0 += mult*dir; txt2 -= mult*dir; txt1 -= dir; txt3 += dir; break;
            case 5: txt0 += dir; txt2 += dir; txt1 += dir; txt3 += dir; break;
        }
    }
    else{
        switch (_currentCorner)
        {
            case 0: p0 += dir; break;
            case 1: p1 += dir; break;
            case 2: p2 += dir; break;
            case 3: p3 += dir; break;
            case 4: p0 += mult*dir; p2 -= mult*dir; p1 -= dir; p3 += dir; break;
            case 5: p0 += dir; p2 += dir; p1 += dir; p3 += dir; break;
        }
    }

    std::cout <<"min h: "<< _minH << " max h: " << _maxH << std::endl;
    std::cout <<"min h: "<< txt0 << " max h: " << txt1 << std::endl;

    update();
}


Point2d MainView::bilinInterp(const Point2d &p)
{
    const double x = p.x(), y = p.y();

    const double x1 = p0.x(), y1 = p0.y();
    const double x2 = p2.x(), y2 = p2.y();

    Point2d res = 1.0 / ((x2 - x1)*(y2 - y1))*(txt0*(x2-x)*(y2-y)+txt3*(x-x1)*(y2-y)+txt1*(x2-x)*(y-y1)+txt2*(x-x1)*(y-y1));

    return res;
}

void MainView::mousePressEvent(QMouseEvent *e)
{
    if(!_setupMode) 
    {
        emit mousePress(e->x(),e->y(), this->width(), this->height());
        return;
    }
}

void MainView::mouseMoveEvent(QMouseEvent *e)
{
    if(!_setupMode) 
    {
        emit mouseMove(e->x(),e->y(), this->width(), this->height());
        return;
    }
}


void MainView::mouseReleaseEvent(QMouseEvent *e)
{ 
    if(!_setupMode) 
    {
        emit mouseRelease(e->x(),e->y(), this->width(), this->height());
        return;
    }
}


void MainView::paintEvent(QPaintEvent *e) 
{
    QPainter painter(this);
    static const double nTiles = 10;

	if (_showSand)
	{
		glClearColor(1, 1, 1, 1);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}

    if (_setupMode){
        glClearColor(1, 0, 0, 1);
    }
    else{
        glClearColor(0, 0, 0, 1);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glDisable(GL_BLEND);

    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);


    if (_moveTexture || !_setupMode)
    {
        _shader.bind();
        setUniforms();
    }
    else{
        _checkerboard.bind();
    }

    for(int i=0;i<nSmoothing;++i){
        activeTexture(GL_TEXTURE0+i);
        if (i<_txt.size())
            glBindTexture(GL_TEXTURE_2D, _txt[i]);
    }


    for(int i=0;i<nLevels;++i){
        activeTexture(GL_TEXTURE0+nSmoothing+i);
        glBindTexture(GL_TEXTURE_2D, _level[i]);
    }

    activeTexture(GL_TEXTURE0+nSmoothing+nLevels);
    if (_gameTexture > 0)
        glBindTexture(GL_TEXTURE_2D, _gameTexture);


    checkGLError("draw -> textures");

    glBegin(GL_QUADS);


    textureCoords(GL_TEXTURE0, txt0.x(), txt0.y(), 0);
    textureCoords(GL_TEXTURE1, txt0.x()*nTiles, txt0.y()*nTiles, 0);
	textureCoords(GL_TEXTURE2, txt0.x(), -txt0.y(), 0);
    glVertex2d(p0.x(), p0.y());

    textureCoords(GL_TEXTURE0, txt1.x(), txt1.y(), 1);
    textureCoords(GL_TEXTURE1, txt1.x()*nTiles, txt1.y()*nTiles, 1);
    glVertex2d(p1.x(), p1.y());

    textureCoords(GL_TEXTURE0, txt2.x(), txt2.y(), 2);
    textureCoords(GL_TEXTURE1, txt2.x()*nTiles, txt2.y()*nTiles, 2);
    glVertex2d(p2.x(), p2.y());

    textureCoords(GL_TEXTURE0, txt3.x(), txt3.y(), 3);
    textureCoords(GL_TEXTURE1, txt3.x()*nTiles, txt3.y()*nTiles, 3);
    glVertex2d(p3.x(), p3.y());
    glEnd();

    
    glPopMatrix();
    checkGLError("draw -> quads");

    activeTexture(GL_TEXTURE0);

    if(_setupMode){
        const double w=this->size().width();
        const double h=this->size().height();
        const QPoint corner(w/2-100,h/2-100);

        switch (_currentCorner)
        {
            case 0: painter.drawImage(corner, _corner0); break;
            case 1: painter.drawImage(corner, _corner1); break;
            case 2: painter.drawImage(corner, _corner2); break;
            case 3: painter.drawImage(corner, _corner3); break;
            case 4: painter.drawImage(corner, _zoom); break;
            case 5: painter.drawImage(corner, _move); break;
        }


    }
    painter.end();
    checkGLError("draw -> image");
}




void MainView::saveMesh(const UINT16 *data)
{
    UnitSquareMapping mapping(txt0,txt1,txt2,txt3);
    mapping.setData(data,_minH,_maxH);

    Point2d min, max;
    mapping.boundingBox(min, max);

    // Triangulator triangulator;


    std::ofstream file;
    const QDateTime date=QDateTime::currentDateTime();
	std::string fileName = "mesh_"+QString::number(date.toTime_t()).toStdString()+".obj";
#ifdef WIN32
	fileName = "C:\\meshes\\"+ fileName;
#endif
	std::cout << fileName << std::endl;
    file.open(fileName.c_str());

	if (!file.good())
	{
		std::cout << "problem" << std::endl;
		return;
	}

	const int maxx = floor(max.x()), maxy = floor(max.y());
	const int minx = ceil(min.x()),  miny = ceil(min.y());

    const int stride=maxx-minx;
    std::vector<int> indices((maxy-miny)*stride);

    int index=1;

	for (int y = miny; y<maxy; ++y)
    {
		const int indexY = y - miny;
		for (int x = minx; x<maxx; ++x)
        {
            const int indexX=x-minx;
			assert(indexX < stride);
            const int arrayIndex=indexX+stride*indexY;

            const Point2d p=mapping.toParameterization(x,y);
            if(!mapping.isInsideParam(p)){
                indices[arrayIndex]=-1;
                continue;
            }
            indices[arrayIndex]=index++;

            double h=1-mapping.getHeight(x,y);

            h *= 0.2;
            // triangulator.addPoint(Point2d(x,y),h);

            file<<"v "<<p.x()<<" "<<p.y()<<" "<<h<<"\n";
        }
    }

    file<<"\n\n";

	for (int y = miny; y<maxy - 1; ++y)
	{
		const int indexY = y - miny;
		for (int x = minx; x<maxx - 1; ++x)
		{
			const int indexX = x - minx;
			assert(indexX+1 < stride);

			const int i1 = indices[indexX + stride*indexY];
			const int i2 = indices[indexX + 1 + stride*indexY];
			const int i3 = indices[indexX + 1 + stride*(indexY + 1)];
			const int i4 = indices[indexX + stride*(indexY + 1)];

			if (i1>0 && i2>0 && i3>0 && i4>0)
            {
				file << "f " << i1 << " " << i2 << " " << i3 << " " << i4 << "\n";
            }

        }
    }

    file.close();
    // triangulator.save("example.obj");
}



void MainView::saveState()
{
    std::ofstream file;
    file.open ("settings.conf");

    file<<p0<<"\n";
    file<<p1<<"\n";
    file<<p2<<"\n";
    file<<p3<<"\n";

    file<<txt0<<"\n";
    file<<txt1<<"\n";
    file<<txt2<<"\n";
    file<<txt3<<"\n";

    file<<_minH<<" "<<_maxH<<"\n";

    file.close();

}

void MainView::loadState()
{
    std::ifstream file;
    file.open ("settings.conf");

    if(!file.good()) return;

    double x, y;
    
    file >> x >> y;
    p0.x()=x; p0.y()=y;

    file >> x >> y;
    p1.x()=x; p1.y()=y;

    file >> x >> y;
    p2.x()=x; p2.y()=y;

    file >> x >> y;
    p3.x()=x; p3.y()=y;


    file >> x >> y;
    txt0.x()=x; txt0.y()=y;

    file >> x >> y;
    txt1.x()=x; txt1.y()=y;

    file >> x >> y;
    txt2.x()=x; txt2.y()=y;

    file >> x >> y;
    txt3.x()=x; txt3.y()=y;

    file >> x >> y;
    _minH=x; _maxH=y;

    file.close();

}
