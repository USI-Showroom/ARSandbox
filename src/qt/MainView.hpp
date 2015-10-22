#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

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
    void setUniforms();

    void createTexture(const QString &path, GLuint &txtId);
	void activeTexture(GLenum texture);
	void textureCoords(GLenum texture, float u, float v);

protected:
	void paintGL();
	void initializeGL();



private:
	QOpenGLShaderProgram _shader;

	GLuint _txt;
	GLuint _level0, _level1, _level2, _level3, _level4;
	QOpenGLFunctions _funs;
	QOpenGLFunctions_2_0 _funs2;
};
