#include "cassiniplotter.h"
#include <qmath.h>
#include <QVector>
#include <QColor>

const int UNIT_SIZE = 40;

CassiniPlotter::CassiniPlotter(QObject*)
{
}

void CassiniPlotter::drawFocus(QImage &image, QPoint focus)
{
    int x = focus.x();
    int y = focus.y();
    int axisDepth = 1;
    setPixel(image, x, y, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x, y + 1, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x, y - 1, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x + 1, y, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x - 1, y, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x + 1, y - 1, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x - 1, y - 1, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x + 1, y + 1, axisDepth, QColor(0, 0, 255).rgba());
    setPixel(image, x - 1, y + 1, axisDepth, QColor(0, 0, 255).rgba());
}

/*Bresenham's line algorithm*/
void CassiniPlotter::drawLine(QImage &image, QPoint point1, QPoint point2)
{
    int x1 = point1.x();
    int x2 = point2.x();
    int y1 = point1.y();
    int y2 = point2.y();
    int deltaX = qAbs(x2 - x1);
    int deltaY = qAbs(y2 - y1);
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int lineDepth = 1;
    int error = deltaX - deltaY;
    setPixel(image, x2, y2, lineDepth, QColor(255, 0, 0).rgba());
    while(x1 != x2 || y1 != y2)
    {
        setPixel(image, x1, y1, lineDepth, QColor(255, 0, 0).rgba());
        const int error2 = error * 2;
        if(error2 > -deltaY)
        {
            error -= deltaY;
            x1 += signX;
        }
        if(error2 < deltaX)
        {
            error += deltaX;
            y1 += signY;
        }
    }
}

void CassiniPlotter::drawAxis(QImage &image)
{
    /*find center of the axis system */
    int centerY = static_cast<int>(image.height() / 2 + 0.5);
    int centerX = static_cast<int>(image.width() / 2 + 0.5);
    for(int i = 0; i < image.width(); i++)
    {
        /*draw the x axis*/
        image.setPixel(i, centerY, QColor(0, 0, 0).rgba());
        /*draw ticks on the x axis*/
        if((i - centerX) % UNIT_SIZE == 0 && i >= centerX)
        {
            image.setPixel(i, centerY - 1, QColor(0, 0, 0).rgba());
            image.setPixel(i, centerY + 1, QColor(0, 0, 0).rgba());
            image.setPixel(2 * centerX - i, centerY - 1, QColor(0, 0, 0).rgba());
            image.setPixel(2 * centerX - i, centerY + 1, QColor(0, 0, 0).rgba());
        }
    }
    for(int i = 0; i < image.height(); i++)
    {
        /*draw y axis*/
        image.setPixel(centerX, i, QColor(0, 0, 0).rgba());
        /*draw ticks on the y axis*/
        if((i - centerY) % UNIT_SIZE == 0 && i >= centerY)
        {
            image.setPixel(centerX - 1, i, QColor(0, 0, 0).rgba());
            image.setPixel(centerX + 1, i, QColor(0, 0, 0).rgba());
            image.setPixel(centerX - 1, 2 * centerY - i, QColor(0, 0, 0).rgba());
            image.setPixel(centerX + 1, 2 * centerY - i, QColor(0, 0, 0).rgba());
        }
    }
}

/*count distance between the Cassini ovals and the point (x, y)*/
long long CassiniPlotter::getError(QPoint focus1, QPoint focus2, int r, int x, int y)
{
   return ((qPow((x - focus1.x()), 2) + qPow((y - focus1.y()), 2))) *
           ((qPow((x - focus2.x()), 2) + qPow((y - focus2.y()), 2))) - qPow(r, 4);
}

void CassiniPlotter::setPixel(QImage &image, int x, int y, int depth, QRgb color)
{
    int width = image.width();
    int height = image.height();
    QPoint pixel = QPoint(x, y);
    int start = (depth % 2 == 0) ? -static_cast<int>(depth / 2 + 0.5) + 1 :
                                   -static_cast<int>(depth / 2 + 0.5) - 1;
    int end = depth + start;
    for(int i = start; i < end; i++)
    {
        for(int j = start; j < end; j++)
        {
            QPoint newPixel = QPoint(i,j) + pixel;
            if(newPixel.x() < width && newPixel.y() < height && newPixel.x() > 0 && newPixel.y() > 0)
            {
                image.setPixel(newPixel, color);
            }
        }
    }
}

void CassiniPlotter::drawCassini(QImage &image, QPoint focus1, QPoint focus2, int R, int depth)
{
    int width = image.width();
    int height = image.height();
    int centerY = static_cast<int>(image.height() / 2 + 0.5);
    int centerX = static_cast<int>(image.width() / 2 + 0.5);
    long long delta = -10000000;
    long long errorX = 0;
    long long errorY = 0;
    /*to find center of symmetry*/
    if((focus1.x() + focus2.x()) % 2 != 0)
    {
        errorX = 1;
    }
    if((focus1.y() + focus2.y()) % 2 != 0)
    {
        errorY = 1;
    }
    int symmetryCenterX = static_cast<int>((focus1.x() + focus2.x() + errorX) / 2);
    int symmetryCenterY = static_cast<int>((focus1.y() + focus2.y() + errorY) / 2);
    long long beginPointX = focus1.x();
    QPoint shift = QPoint(width, height);
    QPoint F1 = focus1;
    QPoint F2 = focus2;
    F1.ry() *= -1;
    F1 += shift;
    F2.ry() *= -1;
    F2 += shift;
    /*go to the left along x axis while sign of the delta doesn't change.
    If delta get positive value then we have found the begin point*/
    while(delta < 0)
    {
        beginPointX--;
        delta = getError(focus1, focus2, R, beginPointX, focus1.y());
    }
    /*choose the nearest point to the Cassini oval from the last to points*/
    if(qAbs(delta) > qAbs(getError(focus1, focus2, R, beginPointX + 1, focus1.y())))
    {
        beginPointX++;
    }
    /*r is not enough big*/
    if(qAbs(beginPointX - focus1.x()) < 3)
    {
        return;
    }
    QPoint currentPoint = QPoint(beginPointX, focus1.y());
    long long counter = 0;
    QPoint prevPosition = QPoint(-2, -2);
    QPoint curPosition = QPoint(-2, -2);
    QPoint finalPoint = QPoint(2 * symmetryCenterX - errorX - currentPoint.x(),
                               2 * symmetryCenterY - errorY - currentPoint.y());
    QVector<QPoint> lastPoint = QVector<QPoint>();
    int currentSignX = 0;
    int currentSignY = 0;
    /*Start from beginPointX. On each step try four directions. And choose the one whith the smalest delta.
    Besides see that that the cosinus of the angle between previous and current directions is greater than zero*/
    do
    {
        int currentPointChanged = false;
        int X = currentPoint.x();
        int Y = currentPoint.y();
        setPixel(image, currentPoint.x() + centerX, -currentPoint.y() + centerY , depth, QColor(255, 0, 0).rgb());
        QPoint symmetryPoint = QPoint(2 * symmetryCenterX - errorX - currentPoint.x(),
                                      2 * symmetryCenterY - errorY - currentPoint.y());
        QPoint symmetryPixel = QPoint(symmetryPoint.x() + centerX, -symmetryPoint.y() + centerY);
        if(symmetryPixel == finalPoint)
        {
            break;
        }
        setPixel(image, symmetryPoint.x() + centerX, -symmetryPoint.y() + centerY, depth, QColor(255, 0, 0).rgb());
        delta = 10e10;
        for(int signX = -1; signX < 2; signX++)
        {
            for(int signY = -1; signY < 2; signY++)
            {
                if((signX == 0 || signY == 0) && (signX + signY) != 0)
                {

                    long long newDelta = qAbs(getError(focus1, focus2, R, X + signX, Y + signY));
                    if(delta > newDelta)
                    {
                        QPoint newPosition = QPoint(signX, signY);
                        QPoint prevPoint = currentPoint - curPosition;
                        QPoint curPoint = currentPoint + newPosition;
                        int angle = prevPosition.x() * newPosition.x() + prevPosition.y() * newPosition.y();
                        if((prevPoint != curPoint) && angle >= 0)
                        {
                            delta = newDelta;
                            currentSignX = signX;
                            currentSignY = signY;
                            currentPointChanged = true;
                        }
                    }
                }
            }
        }
        if(currentPointChanged)
        {
            prevPosition = curPosition;
            curPosition = QPoint(currentSignX, currentSignY);
            currentPoint += curPosition;
            lastPoint.push_back(currentPoint);
        };
        counter++;
    }while(!(currentPoint == finalPoint) && counter < 5000);
}

CassiniPlotter::~CassiniPlotter()
{
}
