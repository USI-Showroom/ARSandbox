#include <QFileInfo>
#include <QOpenGLTexture>

#include "MainView.hpp"


MainView::MainView(QWidget *parent)
: super(parent), _shader(this)
{ }

MainView::~MainView()
{ }

void MainView::initializeGL() {
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );
    glEnable(GL_TEXTURE_2D);
    
    glShadeModel(GL_SMOOTH);  
    glClearColor(1,1,1,1);

    QOpenGLShader shader(QOpenGLShader::Fragment);
    QFileInfo file(":/shaders/HeightMap");
    shader.compileSourceFile(file.absoluteFilePath());

    _shader.addShader(&shader);
    _shader.link();
}   

void MainView::paintGL()
{
    double size=1;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glEnable(GL_TEXTURE_2D); 
    glActiveTexture(GL_TEXTURE0);
    QOpenGLTexture texture(QOpenGLTexture::Target2D);
    texture.setData(QImage(":/test/depth"));
    texture.bind(0);
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