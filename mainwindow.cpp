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
}

