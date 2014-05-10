#ifndef CASSINIWIDGET_H
#define CASSINIWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPoint>
#include "cassiniplotter.h"

namespace Ui
{
    class CassiniWidget;
}

class CassiniWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CassiniWidget(QWidget *parent = 0);
    void paintEvent(QPaintEvent *event);
    void setRValue(int value);
    void setDepth(int value);
    void setImage(QImage newImage);
    QPoint getPixel(QPoint coordinates);
    ~CassiniWidget();
protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);
private:
    Ui::CassiniWidget *ui;
    CassiniPlotter plotter;
    QImage image;
    QImage imageTemp;
    QPoint focus1;
    QPoint focus2;
    bool gotFirstFocus;
    int rValue;
    int depth;
};

#endif // CASSINIWIDGET_H
