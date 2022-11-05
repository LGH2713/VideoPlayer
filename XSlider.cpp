#include "XSlider.h"

XSlider::XSlider(QWidget *parent)
    : QSlider{parent}
{

}

void XSlider::mousePressEvent(QMouseEvent *e)
{
    double pos = static_cast<double>(e->pos().x()) / static_cast<double>(width());
    setValue(pos * this->maximum());
    qDebug() << "press event";
    // 原有事件处理
    // QSlider::mousePressEvent(e);
    QSlider::sliderReleased();
}
