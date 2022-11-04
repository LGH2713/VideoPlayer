#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <XDemuxThread.h>
#include <QMessageBox>

static XDemuxThread dt;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    dt.Start();
    startTimer(40);
}

MainWindow::~MainWindow()
{
    delete ui;
    dt.Close();
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    long long total = dt.totalMs;
    if(total > 0)
    {
        double pos = static_cast<double>(dt.pts) / static_cast<double>(total);
        int v = ui->playPos->maximum() * pos;
        ui->playPos->setValue(v);
    }
}

void MainWindow::resizeEvent(QResizeEvent *e)
{
    ui->playPos->move(50, this->height() - 100);
    ui->playPos->resize(this->width() - 100, ui->playPos->height());
    ui->openFile->move(100, this->height() - 150);
    ui->isPlay->move(ui->openFile->x() + ui->openFile->width() + 10, ui->openFile->y());
    //    ui->video->resize(this->size());
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(isFullScreen())
        this->showNormal();
    else
        this->showFullScreen();
}

void MainWindow::SetPause(bool isPause)
{
    if(isPause)
        ui->isPlay->setText("播放");
    else
        ui->isPlay->setText("暂停");
}

void MainWindow::on_openFile_clicked()
{
    QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
    if(name.isEmpty())
        return ;

    this->setWindowTitle(name);

    if(!dt.Open(name.toLocal8Bit(), this->ui->video))
    {
        QMessageBox::information(0, "error", "open file filed!");
        return ;
    }

    SetPause(false);
}


void MainWindow::on_isPlay_clicked()
{
    bool isPause = !dt.isPause;
    SetPause(isPause);
    dt.SetPause(isPause);
}

