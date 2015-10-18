#include <QFileInfo>

#include "MainView.hpp"


MainView::MainView(QWidget *parent)
	: super(parent), _shader(this), _texture(QOpenGLTexture::Target2D)
{ }

MainView::~MainView()
{ }

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
}

void MainView::newKinectData(const UINT16 *data, int w, int h)
{
#ifdef WIN32
	/*_texture.destroy();

	_texture.setSize(w, h);
	_texture.allocateStorage(QOpenGLTexture::Red, QOpenGLTexture::UInt16);

	_texture.setData(QOpenGLTexture::Depth, QOpenGLTexture::UInt16, data);
	_texture.generateMipMaps();
	_texture.create();*/

	uchar *datac = new uchar[w*h];

	for (int i = 0; i < w*h; ++i){
		USHORT depth = data[i];
		BYTE intensity = static_cast<BYTE>((depth >= 100) && (depth <= 4500) ? (depth % 256) : 0);
		datac[i] = intensity;
	}


	QImage tmp(datac, w, h, QImage::Format_Grayscale8);
	_texture.destroy();
	_texture.setData(tmp);

	delete[] datac;
#else
	_texture.setData(QImage(":/test/depth"));
#endif
	paintGL();
}

void MainView::paintGL()
{
    double size=1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
	glLoadIdentity();

    glEnable(GL_TEXTURE_2D); 
	if (_texture.isCreated())
		_texture.bind(0);

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