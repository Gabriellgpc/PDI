#include "qlabelimage.h"

QLabelImage::QLabelImage(QWidget *parent) : QLabel(parent)
{

}

void QLabelImage::mouseMoveEvent(QMouseEvent *ev)
{
    emit MouseMove(ev->x(), ev->y());
}

void QLabelImage::mousePressEvent(QMouseEvent *ev)
{
    emit MousePress(ev->x(), ev->y());
}
