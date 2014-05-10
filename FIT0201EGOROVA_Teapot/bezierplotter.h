#ifndef BEZIERPLOTTER_H
#define BEZIERPLOTTER_H

#include <QObject>
#include <QImage>
#include <QColor>
#include <QVector3D>
#include "graphics.h"

enum FigureDrawingType
{
    BEZIER_LINES,
    BEZIER_CURVES,
    BEZIER_CURVES_ADAPT
};

class BezierPlotter : public QObject
{
    Q_OBJECT
public:
    explicit BezierPlotter(QObject *parent = 0);
    qreal countBernsteinPolynomial(int uDimension, int polynomNum, qreal u);
    void countMinMaxFigurePoints(QVector3D point);
    qreal countStep(qreal startStep, qreal v, QPoint lastPoint, QImage &image,
                    const QVector<QVector3D> &controlPoints, Graphics &graphics);
    QVector<QVector3D> countBezierCurve(const QVector<QVector3D> &controlPoints, qreal step);
    void countBezierSurface(const QVector<QVector3D> &controlPoints);
    void countBezierSurfaceAdapt(QImage &image, const QVector<QVector3D> &controlPoints, Graphics &graphics);
    void countBezierSurfaceWithLines(const QVector<QVector3D> &controlPoints);
    void countFigure(QImage &image, const QVector< QVector<QVector3D> > &controlPoints, Graphics &graphics,
                     FigureDrawingType type, int partsNumber, int scale);
    void drawBezierCurveWithLines(QImage &image, const QVector<QVector3D> &points, Graphics &graphics, QColor color);
    void drawBezierCurve(QImage &image, const QVector<QVector3D> &points, Graphics &graphics, QColor color);
    void drawFigureWithLines(QImage &image, Graphics &graphics);
    void drawFigure(QImage &image, Graphics &graphics);
    void paintRectangle(QImage &image, const QVector<QVector3D> &top, const QVector<QVector3D> &bottom, int i, Graphics &graphics);
    void paintFigure(QImage &image, Graphics &graphics);
    QVector3D getMinPoint();
    QVector3D getMaxPoint();
    void setConnectVisibility(bool value);
private:
    int partsNumber;
    int scale;                                              //scale coeff for this figure
    QVector<qreal> binomialCoeff;
    QVector3D minFigurePoint;                               //left bootom point of the bounding box
    QVector3D maxFigurePoint;                               //right upper pooint of the bounding box
    QVector< QVector<QVector3D> > controlPointsHorizontal;  //points of horizontal Bezier curves
    QVector< QVector<QVector3D> > controlPointsVertical;    //points of vertical Bezier curves
    qreal factorial(int n);
    bool connectVisibility;
};

#endif // BEZIERPLOTTER_H
