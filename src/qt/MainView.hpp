#include <deque>

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


#define MINIMUM(a,b,c,d) std::min(a,std::min(b,std::min(c,d)))
#define MAXIMUM(a,b,c,d) std::max(a,std::max(b,std::max(c,d)))



class MainView : public QOpenGLWidget
{
	Q_OBJECT
private:
	typedef QOpenGLWidget  super;

	static const int nLevels=5;
	static const int nSmoothing=4;
	
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
	void reloadTerainTextures(const std::string &terrainIndex);
	void reloadGameImage();

	Point2d bilinInterp(const Point2d &p);

	void saveMesh(const UINT16 *data);

protected:
	// void paintGL();
	void initializeGL();
	void paintEvent(QPaintEvent *e);





private:
	QOpenGLShaderProgram _shader, _checkerboard;

	bool _setupMode, _initialized;

	bool _moveTexture;
	bool _mustReloadGameTexture;
	bool _saveNextMesh;

	float _minH, _maxH;
	int _currentCorner;

	std::deque<GLuint> _txt;
	GLuint _level[nLevels];
	GLuint _gameTexture;

	QOpenGLFunctions _funs;
	QOpenGLFunctions_2_0 _funs2;

	Point2d p0, p1, p2, p3;
	Point2d txt0, txt1, txt2, txt3;



	const QImage *_gameImage;

	QImage _zoom, _move, _corner0, _corner1, _corner2, _corner3;
};
