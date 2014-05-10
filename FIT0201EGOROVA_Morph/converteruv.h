#ifndef CONVERTERUV_H
#define CONVERTERUV_H
#include <QVector>
#include <QPoint>
#include <QPointF>

class ConverterUV
{
public:
    ConverterUV();
    void createMatrix(const QVector<QPoint> &xyzTops, const QVector<QPointF> &uvTops);
    QPointF getTextureCoordinates(QPointF imagePoint);
    QVector<QPointF> getTextureRect(QPointF imagePoint);
private:
    QVector<QVector<qreal> > matrix;
    QVector<qreal> rightPart;
    QVector<qreal> uvConvertion;    // vector (a1, a2, a3, b1, b2, b3, d1, d2) assumed that d3 = 1
    void diagonalized();
    void sortColumn(int number);
    void reversePass();
};
#endif // CONVERTERUV_H
