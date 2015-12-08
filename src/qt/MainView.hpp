#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

#include <QKeyEvent>

#include "Point2.hpp"

#include "IKinectProcessor.hpp"

class QImage;

class MainView : public QOpenGLWidget
{
	Q_OBJECT
private:
	typedef QOpenGLWidget  super;
public:
	MainView(QWidget *parent = NULL);
	~MainView();


	void keyPressEvent(QKeyEvent *e);

signals:
	void keyPress(const int keyCode);
	void toggleSetupMode(const bool isSetup, const int minH, const int maxH);

public slots:
	void newKinectData(const UINT16 *data, int w, int h);
	void newGameImage(const QImage &img);

private:
	void checkGLError(const std::string &msg);
	void setUniforms();

	void createTexture(const QString &path, GLuint &txtId);
	void activeTexture(GLenum texture);
	void textureCoords(GLenum texture, float u, float v, int i);


protected:
	void paintGL();
	void initializeGL();





private:
	QOpenGLShaderProgram _shader;

	bool _setupMode;

	float _minH, _maxH;

	int _currentCorner;

	GLuint _txt;
	GLuint _level0, _level1, _level2, _level3, _level4;
	QOpenGLFunctions _funs;
	QOpenGLFunctions_2_0 _funs2;

	Point2d p0,p1,p2,p3;

	GLuint _gameTexture;
	bool _initialized;

	const QImage *_tmpGameImage;
};
