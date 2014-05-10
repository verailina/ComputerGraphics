#ifndef CURVEPLOTTER_H
#define CURVEPLOTTER_H

#include <QImage>
#include <qmath.h>

class CurvePlotter : public QObject
{
    Q_OBJECT
public:
    explicit CurvePlotter(QObject *parent = 0);
    void drawAxis(QImage &image);
    void drawRect(QImage &image, QPoint left, QPoint right);
    void drawCurve(QImage &image, QPoint left, QPoint right, int depth, bool mode);
    QPointF getScale();
    QPointF getTickSize();
    void resetScale();
    ~CurvePlotter();
private:
   QPoint leftPoint;
   QPoint rightPoint;
   QPointF center;
   qreal xScale;
   qreal yScale;
   int depth;
   bool firstDrawing;
   int WPrev;
   int HPrev;
   int W;
   int H;
   int mode;
   qreal x(qreal t);
   qreal y(qreal t);
   qreal xDerivative(qreal t);
   qreal yDerivative(qreal t);
   qreal countStep(qreal t, qreal eps, qreal unitX, qreal unitY, qreal tMax);
   qreal Tx(qreal x);
   qreal Ty(qreal y);
   QVector<qreal> belong(QVector<qreal> t, qreal t0, qreal t1);
   QPoint getPixel(QImage &image, qreal x, qreal y);
   QPointF getReal(QImage &image, QPoint point);
   void drawInterval(QImage &image, qreal t0, qreal t1, QVector<qreal> t, bool flag);
   void setPixel(QImage &image, QPoint pixel, int depth, QRgb color);
   void validatePoint(QPoint &point, QPoint maxPoint);

};

#endif // CURVEPLOTTER_H
