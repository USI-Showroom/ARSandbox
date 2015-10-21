#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include "IKinectProcessor.hpp"

class MainView : public QOpenGLWidget
{
	Q_OBJECT
private:
	typedef QOpenGLWidget  super;
public:
	MainView(QWidget *parent = NULL);
	~MainView();

public slots:
void newKinectData(const UINT16 *data, int w, int h);

private:
    void checkGLError();

protected:
	void paintGL();
	void initializeGL();

private:
	QOpenGLShaderProgram _shader;
	//QOpenGLTexture _texture;
	GLuint _txt;
};
