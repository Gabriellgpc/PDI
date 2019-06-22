#ifndef QLABELIMAGE_H
#define QLABELIMAGE_H

#include <QLabel>
#include <QMouseEvent>

class QLabelImage : public QLabel
{
    Q_OBJECT
public:
    explicit QLabelImage(QWidget *parent = nullptr);
private:
    void mouseMoveEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
//    void fromImage(uchar);
signals:
    void MouseMove(int x, int y);
    void MousePress(int x, int y);
public slots:

};

#endif // QLABELIMAGE_H
