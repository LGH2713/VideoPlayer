#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>

class XVideoWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    XVideoWidget(QWidget *parent);
    ~XVideoWidget();

protected:
    // 刷新显示
    void paintGL();

    // 初始化GL
    void initializeGL();

    // 窗口尺寸变化
    void resizeGL(int width, int height);
};

#endif // XVIDEOWIDGET_H
