#include <QOpenGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

#include <QOpenGLFunctions_2_0>
#include <QOpenGLFunctions>

#include <QKeyEvent>
#include <QMouseEvent>

#include <QImage>

#include "Point2.hpp"

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
	void mouseReleaseEvent(QMouseEvent *e);

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


	Point2d bilinInterp(const Point2d &p);


protected:
	// void paintGL();
	void initializeGL();
	void paintEvent(QPaintEvent *e);





private:
	QOpenGLShaderProgram _shader, _checkerboard;

	bool _setupMode;

	float _minH, _maxH;
	std::vector<QPoint> _positions;
	int _currentCorner;

	GLuint _txt;
	GLuint _level0, _level1, _level2, _level3, _level4;
	QOpenGLFunctions _funs;
	QOpenGLFunctions_2_0 _funs2;

	Point2d p0, p1, p2, p3;
	Point2d txt0, txt1, txt2, txt3;


	GLuint _gameTexture;
	bool _initialized;

	bool _moveTexture;

	const QImage *_tmpGameImage;

	QImage _zoom, _move, _corner0, _corner1, _corner2, _corner3;
};
