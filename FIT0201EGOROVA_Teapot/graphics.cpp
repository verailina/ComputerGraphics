#include "graphics.h"
#include "qmath.h"

static const int START_RADIUS = 1000;
static const int DISTANCE_STEP = 5;
static const int WHEEL_ANGLE = 120;
static const int AXIS_EXT = 20;
static const int MIN_DISTANCE = -300;
static const int MAX_DISTANCE = 600;

Graphics::Graphics(QObject *parent) :
    QObject(parent)
{
    /*seting start eye and projection plane positions*/
    eye = QVector3D(0, -1, 0);
    eyePos = -START_RADIUS * eye;    
    distance = 300;
    basePoint = -distance * eye;
    up = QVector3D(0, 0, 1);
    right = vectorProduct(up, eye);
}

void Graphics::rotate(QPoint delta)
{
    rotationMatrix = QMatrix4x4();
    rotationMatrix.rotate(delta.x(), up);
    rotationMatrix.rotate(delta.y(), right);
    eye = rotationMatrix.map(eye);
    eyePos = rotationMatrix.map(eyePos);
    basePoint = -distance * eye;
    up = rotationMatrix.map(up);
    right = vectorProduct(up, eye);
}

void Graphics::zoom(int delta)
{
    qreal zoomDelta = static_cast<qreal>((delta / WHEEL_ANGLE) * DISTANCE_STEP);
    if(distance - zoomDelta >= MIN_DISTANCE && distance - zoomDelta <= MAX_DISTANCE)
    {
        distance -= zoomDelta;
        eyePos -= zoomDelta * eye;
        basePoint = -distance * eye;
    }
}

qreal Graphics::scalarProduct(QVector3D point1, QVector3D point2)
{
    return point1.x() * point2.x() + point1.y() * point2.y() + point1.z() * point2.z();
}

QVector3D Graphics::vectorProduct(QVector3D point1, QVector3D point2)
{
    return QVector3D(point1.y() * point2.z() - point1.z() * point2.y(),
                     -(point1.x() * point2.z() - point1.z() * point2.x()),
                     point1.x() * point2.y() - point1.y() * point2.x());
}

QPoint Graphics::getCanvasPoint(QVector3D point, int width, int height)
{
    int x = static_cast<int>(scalarProduct(point, right) + width / 2 + 0.5);
    int y = static_cast<int>(-scalarProduct(point, up) + height / 2 + 0.5);
    return QPoint(x, y);
}

QPoint Graphics::getPointProjection(QVector3D point, int width, int height)
{
    qreal t = -(scalarProduct(eye, (eyePos - basePoint))) / scalarProduct(eye, (point - eyePos));
    QVector3D projection = QVector3D(eyePos + t * (point - eyePos));
    return getCanvasPoint(projection, width, height);
}

/*Checking if the point is behind of the projection plane*/
bool Graphics::isVisible(QVector3D point)
{
    qreal angle = scalarProduct(eyePos - basePoint, point - basePoint);
    if(angle < 0)
    {
        return true;
    }
    return false;
}

/*Cutting of the line. Getting visible part of this line*/
bool Graphics::cutOffLine(QVector3D *p_point1, QVector3D *p_point2)
{
    QVector3D pointFrom3D = *p_point1;
    QVector3D pointTo3D = *p_point2;
    qreal denominator = scalarProduct(eye, (pointTo3D - basePoint));
    if(denominator != 0)
    {
        /*Counting intersection line and plane*/
        qreal t = (scalarProduct(eye, (basePoint - pointFrom3D)) / scalarProduct(eye, (pointTo3D - basePoint)));
        /*The point of intersection is beеwen point1 and point2*/
        if(t >= 0 && t <= 1)
        {
            if(isVisible(pointFrom3D))
            {
                *p_point2 = QVector3D(pointFrom3D + t * (pointTo3D - pointFrom3D));
            }
            else if(isVisible(pointTo3D))
            {
                *p_point1 = QVector3D(pointFrom3D + t * (pointTo3D - pointFrom3D));
            }
        }
        /*Both points are situated by the one side of plane*/
        else if(t < 0 || t > 1)
        {
            if(!isVisible(pointFrom3D))
            {
                return false;
            }
        }
    }
    return true;
}

/*Drawing line with Brezenhems algorithm. Cutting invisible part of line if the value of cutOff is true*/
void Graphics::drawLine(QImage &image, QVector3D pointFrom3D, QVector3D pointTo3D, QColor color, bool cutOff)
{
    bool toDraw = (!cutOff) ? true : cutOffLine(&pointFrom3D, &pointTo3D);
    if(toDraw)
    {
        QPoint point1 = getPointProjection(pointFrom3D, image.width(), image.height());
        QPoint point2 = getPointProjection(pointTo3D, image.width(), image.height());
        int x1 = point1.x();
        int x2 = point2.x();
        int y1 = point1.y();
        int y2 = point2.y();
        int deltaX = qAbs(x2 - x1);
        int deltaY = qAbs(y2 - y1);
        int signX = x1 < x2 ? 1 : -1;
        int signY = y1 < y2 ? 1 : -1;
        int error = deltaX - deltaY;
        if(x2 > 0 && x2 < image.width() && y2 > 0 && y2 < image.height())
        {
            QColor zColor = image.pixel(x2, y2);
            /*Checking if this point is int he foreground*/
            if(zColor.red() >= color.red() || zColor.red() == 255 || zColor.red() == 0)
            {
                image.setPixel(x2, y2, color.rgb());
            }
        }
        while(x1 != x2 || y1 != y2)
        {
            if(x1 >=0 && x1 < image.width() && y1 > 0 && y1 < image.height())
            {
                QColor zColor = image.pixel(x1, y1);
                /*Checking if this point is int he foreground*/
                if(zColor.red() >= color.red() || zColor.red() == 255 || zColor.red() == 0)
                {
                    image.setPixel(x1, y1, color.rgb());
                }
            }
            int error2 = error * 2;
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
}

void Graphics::swap(QPoint &p1, QPoint &p2)
{
    QPoint tempPoint = p1;
    p1 = p2;
    p2 = tempPoint;
}

/*Pinting triangele by moving along two its side and drawing lines connecting points of tcis sides*/
void Graphics::paintTriangle(QImage &image, QVector3D vertex1, QVector3D vertex2, QVector3D vertex3, int partsNumber, QColor color)
{
    QPoint v1 = getPointProjection(vertex1, image.width(), image.height());
    QPoint v2 = getPointProjection(vertex2, image.width(), image.height());
    QPoint v3 = getPointProjection(vertex3, image.width(), image.height());
    /*regularize triangle's vertexes*/
    if(v2.y() < v1.y())
    {
        swap(v2, v1);
    }
    if(v3.y() < v1.y())
    {
        swap(v1, v3);
    }
    if(v2.y() > v3.y())
    {
        swap(v2, v3);
    }    
    qreal step = 0.01 * (partsNumber + 1) / (v3.y() - v1.y());
    /*Distanse between eye and triangle plane*/
    qreal eyeDistanse = qAbs(eyePos.distanceToPlane(vertex1, vertex2, vertex3));
    /*Distanse betwen eye and the point(0, 0, 0)*/
    qreal maxDistanse = qSqrt(eyePos.x() * eyePos.x() +
                              eyePos.y() * eyePos.y() +
                              eyePos.z() * eyePos.z());
    eyeDistanse = maxDistanse / eyeDistanse;
    /*Counting color considering distanse betwen eye and triangle plane*/
    QColor darkerColor = color.darker(100 * eyeDistanse);
    for(qreal t = 0; t <= 1; t += step)
    {
        QVector3D point1 = vertex1 + (vertex3 - vertex1) * t;
        QVector3D point2 = vertex2 + (vertex3 - vertex2) * t;
        drawLine(image, point1, point2, darkerColor, true);
    }
}
