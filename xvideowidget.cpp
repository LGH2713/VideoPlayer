#include "XVideoWidget.h"

XVideoWidget::XVideoWidget(QWidget *parent)
{

}

XVideoWidget::~XVideoWidget()
{

}

void XVideoWidget::initializeGL()
{
    qDebug() << "initializeGL";
}

void XVideoWidget::paintGL()
{
    qDebug() << "paintGL";
}

void XVideoWidget::resizeGL(int width, int height)
{
    qDebug() << "paintGL";
}
