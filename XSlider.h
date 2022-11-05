#ifndef XSLIDER_H
#define XSLIDER_H

#include <QSlider>
#include <QMouseEvent>

class XSlider : public QSlider
{
    Q_OBJECT
public:
    explicit XSlider(QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *e);

//signals:

};

#endif // XSLIDER_H
