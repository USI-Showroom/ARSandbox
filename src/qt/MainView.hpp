#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

#include <QKeyEvent>


#include "IKinectProcessor.hpp"

class MainView : public QOpenGLWidget
{
	Q_OBJECT
private:
	typedef QOpenGLWidget  super;
public:
	MainView(QWidget *parent = NULL);
	~MainView();


	void keyPressEvent(QKeyEvent *e);

public slots:
void newKinectData(const UINT16 *data, int w, int h);

private:
    void checkGLError();
    void setUniforms();

    void createTexture(const QString &path, GLuint &txtId);
	void activeTexture(GLenum texture);
	void textureCoords(GLenum texture, float u, float v, int i);


protected:
	void paintGL();
	void initializeGL();

    



private:
	QOpenGLShaderProgram _shader, _checkerboard;

	bool _setupMode, _showCheckerboard;

	float _txtMinX, _txtMinY, _txtMaxX, _txtMaxY;

	double _top, _bottom;

	float _minH, _maxH;

	GLuint _txt;
	GLuint _level0, _level1, _level2, _level3, _level4;
	QOpenGLFunctions _funs;
	QOpenGLFunctions_2_0 _funs2;
};
