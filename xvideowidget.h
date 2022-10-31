#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QTimer>
#include <mutex>

#include "IVideoCall.h"

struct AVFrame;

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions, public IVideoCall
{
    Q_OBJECT
public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();

    virtual void Init(int width, int height);

    // 不管成功与否都释放frame空间
    virtual void Repaint(AVFrame *frame);

protected:
    // 刷新显示
    void paintGL();

    // 初始化GL
    void initializeGL();

    // 窗口尺寸变化
    void resizeGL(int width, int height);

private:
    // shader程序
    QOpenGLShaderProgram program;

    std::mutex mux;

    // shader中的yuv地址
    GLuint unis[3] = {0};
    // opengl的texture地址
    GLuint texs[3] = {0};

    // 材质的内存空间
    unsigned char *datas[3] = {0};

    int width = 240;
    int height = 128;
};

#endif // XVIDEOWIDGET_H
