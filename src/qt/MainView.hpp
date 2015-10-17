#include <QGLWidget>
#include <QPaintEvent>
#include <QOpenGLShaderProgram>

class MainView: public QGLWidget
{
    Q_OBJECT
private:
    typedef QGLWidget super;
public:
    MainView(QWidget *parent = NULL);
    ~MainView();
    

protected:
    void paintGL();
    void initializeGL();

private:
    QOpenGLShaderProgram _shader;
};
