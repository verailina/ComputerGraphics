#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <QColor>
#include <QImage>

class Graphics : public QObject
{
    Q_OBJECT
public:
    explicit Graphics(QObject *parent = 0);
    void rotate(QPoint delta);
    void zoom(int delta);
    qreal scalarProduct(QVector3D point1, QVector3D point2);
    QVector3D vectorProduct(QVector3D point1, QVector3D point2);
    QPoint getCanvasPoint(QVector3D point, int width, int height);
    QPoint getPointProjection(QVector3D point, int width, int height);
    bool isVisible(QVector3D point);
    bool cutOffLine(QVector3D *p_point1, QVector3D *p_point2);
    void drawLine(QImage &image, QVector3D point1, QVector3D point2, QColor color, bool cutOff);
    void paintTriangle(QImage &image, QVector3D v1, QVector3D v2, QVector3D v3, int partsNumber, QColor color);
private:
    QVector3D eye;                     //vecor of the eye directon
    QVector3D eyePos;                  //eye position
    QVector3D right;                   //basis vector in the plane
    QVector3D up;                      //basis vector in the plane
    QVector3D basePoint;               //point of the intersection of the projections plane and line of eye direction
    qreal distance;                    //distanse between eye and the point(0, 0, 0)
    QMatrix4x4 rotationMatrix;
    void swap(QPoint &p1, QPoint &p2);
};

#endif // GRAPHICS_H
