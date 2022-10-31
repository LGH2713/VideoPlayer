#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

static double r2d(AVRational r) {
    return r.den == 0 ? 0 : static_cast<double>(r.num) / static_cast<double>(r.den);
}

void XSleep(int ms) {
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}
