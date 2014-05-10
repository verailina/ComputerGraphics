#ifndef CURVEWIDGET_H
#define CURVEWIDGET_H

#include <QWidget>
#include "curveplotter.h"

namespace Ui {
class CurveWidget;
}

class CurveWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit CurveWidget(QWidget *parent = 0);
    void setDepth(int value);
    ~CurveWidget();
signals:
    void scaleChaged(QPointF scale, QPointF tickSize);
public slots:
    void setImage(QImage image);
    void resetScale();
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
private:
    Ui::CurveWidget *ui;
    QImage image;
    QImage tmpImage;
    int depth;
    CurvePlotter plotter;
    QPoint rectLeft;
    QPoint rectRight;
    double xScale;
    double yScale;
    double xTick;
    double yTick;
    int mode;
};

#endif // CURVEWIDGET_H
