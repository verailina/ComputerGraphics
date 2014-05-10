#include "teapothandler.h"
#include <QColor>
#include <qmath.h>

static const int AXIS_EXT = 20;
TeapotHandler::TeapotHandler(QObject *parent) :
    QObject(parent)
{
}

void TeapotHandler::changeEyeView(QPoint delta)
{
    graphics.rotate(delta);
}

void TeapotHandler::zoom(QPoint delta)
{
    graphics.zoom(delta.y());
}

/*Drawing rectangles of the bounding box*/
void TeapotHandler::drawRectPtojection(const QVector<QVector3D> &points, QImage &image, QColor color)
{
    QVector3D maxFigurePoint = plotter.getMaxPoint();
    QVector3D minFigurePoint = plotter.getMinPoint();
    QVector3D shift = 0.5 * (maxFigurePoint + minFigurePoint);
    for(int i = 0; i + 1 < points.size(); i++)
    {
        QVector3D pointFrom3D = points[i] - shift;
        QVector3D pointTo3D = points[i + 1] - shift;
        graphics.drawLine(image, pointFrom3D, pointTo3D, color, true);
    }
}

/*Counting figures points*/
void TeapotHandler::countFigure(QImage &image, const QVector< QVector<QVector3D> > &controlPoints,                             FigureDrawingType type, int partsNumber, int scale)
{
    plotter.countFigure(image, controlPoints, graphics, type, partsNumber, scale);
}

void TeapotHandler::drawFigure(QImage &image, FigureDrawingType type, bool axisIsVisible, bool boundingBoxIsVisible,
                               bool paintIsVisibile, bool gridIsVisibile, bool connect)
{
    if(axisIsVisible)
    {
        drawAxis(image);
    }
    if(boundingBoxIsVisible)
    {
        drawBoundingBox(image);
    }
    plotter.setConnectVisibility(connect);
    if(gridIsVisibile)
    {
        if(type == BEZIER_CURVES || type == BEZIER_LINES)
        {
            plotter.drawFigureWithLines(image, graphics);
        }
        else if(type == BEZIER_CURVES_ADAPT)
        {
            plotter.drawFigure(image, graphics);
        }
    }
    if(type == BEZIER_LINES && paintIsVisibile)
    {
        plotter.paintFigure(image, graphics);
    }
}

void TeapotHandler::drawBoundingBox(QImage &image)
{
    QVector<QVector3D> vector = QVector<QVector3D>();
    QVector3D minFigurePoint = plotter.getMinPoint();
    QVector3D maxFigurePoint = plotter.getMaxPoint();
    /*bottom*/
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), maxFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), maxFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    drawRectPtojection(vector, image, QColor(0, 0, 0));
    vector.clear();
    /*top*/
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), maxFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), maxFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), minFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), maxFigurePoint.z()));
    drawRectPtojection(vector, image, QColor(0, 0, 0));
    vector.clear();
    /*left*/
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), maxFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), maxFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(maxFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    drawRectPtojection(vector, image, QColor(0, 0, 0));
    vector.clear();
    /*right*/
    vector.push_back(QVector3D(minFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), minFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), maxFigurePoint.y(), maxFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), maxFigurePoint.y(), minFigurePoint.z()));
    vector.push_back(QVector3D(minFigurePoint.x(), minFigurePoint.y(), minFigurePoint.z()));
    drawRectPtojection(vector, image, QColor(0, 0, 0));
    vector.clear();
}

void TeapotHandler::drawAxis(QImage &image)
{
    QVector3D minFigurePoint = plotter.getMinPoint();
    QVector3D maxFigurePoint = plotter.getMaxPoint();
    QVector3D shift = 0.5 * (maxFigurePoint - minFigurePoint) + QVector3D(AXIS_EXT, AXIS_EXT, AXIS_EXT);
    QVector3D pointFrom = QVector3D(-shift.x(), 0 , 0);
    QVector3D pointTo = QVector3D(shift.x(), 0 , 0);
    graphics.drawLine(image, pointFrom, pointTo, QColor(255, 0, 0), false);
    pointFrom = QVector3D(0, -shift.y(), 0);
    pointTo = QVector3D(0, shift.y(), 0);
    graphics.drawLine(image, pointFrom, pointTo, QColor(0, 255, 0), false);
    pointFrom = QVector3D(0, 0, -shift.z());
    pointTo = QVector3D(0, 0, shift.z());
    graphics.drawLine(image, pointFrom, pointTo, QColor(0, 0, 255), false);
}
