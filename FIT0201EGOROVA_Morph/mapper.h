#ifndef MAPPER_H
#define MAPPER_H

#include <QImage>
#include <QColor>
#include "converteruv.h"

enum Layer
{
    NEAREST,
    LINEAR
};
enum Mip
{
    NONE,
    NEAREST_MIP,
    LINEAR_MIP
};
enum Type
{
    SHOW_IMAGE,
    SHOW_TEXTURE,
    SHOW_NEAREST_MIP
};
class Mapper
{   
public:
    Mapper();
    void map(QImage &image, QPoint leftTop, QPoint rightBottom, QPoint leftBottom, QPoint rightTop,
             const QVector<QPointF> &uvTop);
    QPointF getMirror(QPointF point);
    /*Filters*/
    QRgb getPixelNearest(QPointF point);
    QRgb getPixelBilinearFilter(QPointF point);
    QRgb getPixelTrilinearFilter(const QVector<QPointF> &pointRect, QPointF point);
    QRgb bilinearInterpolation(QRgb rgb1, QRgb rgb2, qreal alpha);
    QImage getNearestMip(const QVector<QPointF> &pointRect);
    bool validatePoint(QPoint point);
    void setLayerType(Layer layer);
    void setMipType(Mip mip);
    void setShowType(Type type);
    QImage getNextMipLevel(QImage source);
    void generateMipLevels(QImage image);
    QImage testMip(int number);
    qreal countMaxPixelRectSideLenght(const QVector<QPointF> &pointRect);
private:
    QImage mapImage;
    QImage texture;
    ConverterUV converter;
    QVector<QImage> mipLevels;
    QVector<QPointF> uvTops;
    QVector<QPoint> tops;
    Layer layer;
    Mip mip;
    Type showType;
    int mipNumber; // to show mip numbers
    qreal countDist(QPointF point1, QPointF point2);
    void drawLine(QImage &image, int pointFrom, int pointTo, int pointFix);
};

#endif // MAPPER_H
