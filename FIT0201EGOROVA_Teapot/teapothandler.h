#ifndef TEAPOTHANDLER_H
#define TEAPOTHANDLER_H

#include <QObject>
#include <QVector3D>
#include <QVector>
#include <QImage>
#include "graphics.h"
#include "bezierplotter.h"

class TeapotHandler : public QObject
{
    Q_OBJECT
public:
    explicit TeapotHandler(QObject *parent = 0);
    void changeEyeView(QPoint delta);
    void zoom(QPoint delta);
    void countFigure(QImage &image, const QVector< QVector<QVector3D> > &controlPoints, FigureDrawingType type, int partsNumber, int scale);
    void drawFigure(QImage &image, FigureDrawingType type, bool axisIsVisible,
                    bool boundingBoxIsVisible, bool paintIsVisibile, bool gridIsVisibile, bool connect);
    void drawBoundingBox(QImage &image);
    void drawRectPtojection(const QVector<QVector3D> &points, QImage &image, QColor color);
    void drawAxis(QImage &image);
private:
    Graphics graphics;
    BezierPlotter plotter;
};

#endif // TEAPOTHANDLER_H
