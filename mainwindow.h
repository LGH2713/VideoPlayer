#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QOpenGLWidget>

#include <iostream>
#include <thread>
using namespace std;

extern "C" {
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
//#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    // 定时器 滑动条显示
    void timerEvent(QTimerEvent *e);

    // 窗口尺寸变化
    void resizeEvent(QResizeEvent *e);

    // 双击全屏
    void mouseDoubleClickEvent(QMouseEvent *e);

    void SetPause(bool isPause);

private:
    Ui::MainWindow *ui;

private slots:
    void on_openFile_clicked();
    void on_isPlay_clicked();
};

int running();

#endif // MAINWINDOW_H
