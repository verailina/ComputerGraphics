#include "curveplotter.h"
#include <QColor>

const qreal X_INTERVAL = 20.0;
const qreal Y_INTERVAL = 15.0;
const qreal EPS = 10e-10;

CurvePlotter::CurvePlotter(QObject *)
{
    firstDrawing = true;
    xScale = 1;
    yScale = 1;
    mode = 0;
}

void CurvePlotter::drawAxis(QImage &image)
{
    QPoint point = getPixel(image, 0, 0);
    int centerY = point.y();
    int centerX = point.x();
    qreal unit = X_INTERVAL / image.width();
    int size = static_cast<int>(1 / unit);
    int axisDepth = 1;
    for(int i = 0; i < image.width(); i++)
    {
        /*draw the x axis*/
        setPixel(image, QPoint(i, centerY), axisDepth, QColor(0, 0, 0).rgba());
        /*draw ticks on the x axis*/
        if(((i - centerX) % size == 0 && i >= centerX) || ((centerX - i) % size == 0 && i <= centerX))
        {
            setPixel(image, QPoint(i, centerY - 1), axisDepth, QColor(0, 0, 0).rgba());
            setPixel(image, QPoint(i, centerY + 1), axisDepth, QColor(0, 0, 0).rgba());
        }
    }
    unit = (Y_INTERVAL / image.height());
    size = static_cast<int>(1 / unit);
    for(int i = 0; i < image.height(); i++)
    {
        /*draw yaxis*/
        setPixel(image, QPoint(centerX, i), axisDepth, QColor(0, 0, 0).rgba());
        if(((i - centerY) % size == 0 && i >= centerY) || ((centerY - i) % size == 0 && centerY >= i))
        {
            /*draw ticks on the y axis*/
           setPixel(image, QPoint(centerX - 1, i), axisDepth, QColor(0, 0, 0).rgba());
           setPixel(image, QPoint(centerX + 1, i), axisDepth, QColor(0, 0, 0).rgba());
        }
    }
}

void CurvePlotter::drawRect(QImage &image, QPoint rectLeft, QPoint rectRight)
{
    QPoint left = QPoint(qMin(rectLeft.x(), rectRight.x()), qMin(rectLeft.y(), rectRight.y()));
    QPoint right = QPoint(qMax(rectLeft.x(), rectRight.x()), qMax(rectLeft.y(), rectRight.y()));
    validatePoint(left, QPoint(image.width(), image.height()));
    validatePoint(right, QPoint(image.width(), image.height()));
    int rectDepth = 1;
    for(int i = left.x(); i <= right.x(); i++)
    {
        setPixel(image, QPoint(i, left.y()), rectDepth, QColor(0, 0, 255).rgba());
        setPixel(image, QPoint(i, right.y()), rectDepth, QColor(0, 0, 255).rgba());
    }
    for(int j = left.y(); j <= right.y(); j++)
    {
        setPixel(image, QPoint(left.x(), j), rectDepth, QColor(0, 0, 255).rgba());
        setPixel(image, QPoint(right.x(), j), rectDepth, QColor(0, 0, 255).rgba());
    }
}

qreal CurvePlotter::x(qreal t)
{
    return t * t / (t * t - 1 );
}

qreal CurvePlotter::y(qreal t)
{
    return (t * t + 1) / (t + 1);
}

qreal CurvePlotter::xDerivative(qreal t)
{
    return -2 * t / qPow((t * t - 1), 2);
}

qreal CurvePlotter::yDerivative(qreal t)
{
    return (t * t + 2 * t - 1) / qPow((t + 1), 2);
}

qreal CurvePlotter::Tx(qreal x)
{
    return qSqrt(x / (x - 1));
}

qreal CurvePlotter::Ty(qreal y)
{
    return qSqrt(y * y - 4 * (1 - y)) / 2;
}

void CurvePlotter::setPixel(QImage &image, QPoint pixel, int depth, QRgb color)
{
    int width = image.width();
    int height = image.height();
    /*replace one pixel on rectangel (depth * depth) with this pixel in the center (or closer to lest up corner*/
    int start = (depth % 2 == 0) ? static_cast<int>(-depth / 2 + 1) : static_cast<int>(-depth / 2);
    int end = depth + start;
    for(int i = start; i < end; i++)
    {
        for(int j = start; j < end; j++)
        {
            QPoint newPixel = QPoint(i,j) + pixel;
            if(newPixel.x() < width && newPixel.y() < height && newPixel.x() > 0 && newPixel.y() > 0)
            {
                QColor newColor = QColor(color);
                image.setPixel(newPixel, newColor.rgba());
            }
        }
    }
}

/*count value the next step along t. Try to get the smallest h that we can*/
qreal CurvePlotter::countStep(qreal t, qreal eps, qreal unitX, qreal unitY, qreal tMax)
{
    qreal h = 1;
    while(h > eps)
    {
        if((t + eps) > tMax)
        {
            return tMax - t;
        }
        qreal maxX = qMax(xDerivative(t),xDerivative(t + eps));
        qreal maxY = qMax(yDerivative(t),yDerivative(t + eps));
        h = (qMin((unitX), unitY) / qSqrt(maxX * maxX + maxY * maxY)) ;
        eps *= 2;
    }
    return h;
}

/*translate real values to canvas pixels*/
QPoint CurvePlotter::getPixel(QImage & image, qreal x, qreal y)
{
    int width = image.width();
    int height = image.height();
    qreal scaleX = qAbs(X_INTERVAL / image.width()) / xScale;
    qreal scaleY = qAbs(Y_INTERVAL / image.height()) / yScale;
    int X = static_cast<int>((x - center.x()) / scaleX + width / 2);
    int Y = static_cast<int>(-(y  - center.y()) / scaleY + height / 2);
    return QPoint(X, Y);
}

/*draw interval of monotony of x and y*/
void CurvePlotter::drawInterval(QImage & image, qreal t0, qreal t1, QVector<qreal> vt, bool flag)
{
    int w = image.width();
    int h = image.height();
    qreal xUnit = X_INTERVAL / image.width();
    qreal yUnit = Y_INTERVAL / image.height();
    QVector<qreal> interval = belong(vt, t0, t1);
    int size = interval.size();
    qreal start = t0;
    qreal finish = t1;
    if(!interval.isEmpty() || firstDrawing  || mode)
    {
        if(size == 2 && !firstDrawing && !mode && t0 != 0)
        {
            start = qMin(interval[0], interval[1]);
            finish = qMax(interval[0], interval[1]);
        }
        if(size == 1 && !firstDrawing && !mode && t0 != 0)
        {
            start = (!flag) ? t0 : interval[0];
            finish = (flag) ? t1 : interval[0];
        }
        QPointF tmp_center = center;

        qreal tmp_xScale = xScale;
        qreal tmp_yScale = yScale;
        /*if zoom in we need to chage scale here*/
        if(!mode && w != W)
        {
            center = getReal(image, QPoint(leftPoint.x() + W / 2, leftPoint.y() + H / 2));
            qreal x_Scale = (mode == 0) ? (qreal)w / W : (qreal)W / w;
            qreal y_Scale = (mode == 0) ? (qreal)h / H : (qreal)H / h;
            xScale = (w != W) ? xScale * x_Scale : 1;
            yScale = (w != W) ? yScale * y_Scale : 1;
        }
        for(qreal t = start; t < finish;)
        {
            qreal step = countStep(t, EPS, (xUnit * yUnit) / xScale, yUnit / yScale , finish);
            t += step;
            QPoint point = getPixel(image, x(t), y(t)) ;
            setPixel(image, point - QPoint(depth - 1, 0), depth, QColor(255, 0, 0).rgba());
        }
        /*back to previous values of scale*/
        if(!mode && w != W)
        {
            center = tmp_center;
            xScale = tmp_xScale;
            yScale = tmp_yScale;
        }
    }
}

/*translate canvas coordinates to real values*/
QPointF CurvePlotter::getReal(QImage & image, QPoint point)
{
    int width = image.width();
    int height = image.height();
    qreal scaleX = X_INTERVAL / image.width() / xScale;
    qreal scaleY = Y_INTERVAL / image.height() / yScale;
    qreal x = static_cast<qreal>((point.x() - width / 2) * scaleX) + center.x();
    qreal y = static_cast<qreal>((point.y() - height / 2) * (-scaleY)) + center.y();
    return QPointF(x, y);
}

/*find vector of points belong to the interval [t0, t1]*/
QVector<qreal> CurvePlotter::belong(QVector<qreal> t, qreal t0, qreal t1)
{
    QVector<qreal> newT = QVector<qreal>();
    for(int i = 0; i < t.size(); i++)
    {
        if(t[i] >= t0 && t[i] <= t1)
        {
            newT.push_back(t[i]);
        }
    }
    return newT;
}

/*if point is out of windows changes it value*/
void CurvePlotter::validatePoint(QPoint &point, QPoint maxPoint)
{
    if(point.x() <= 0)
    {
        point.rx() = 0;
    }
    if(point.y() <= 0)
    {
        point.ry() = 0;
    }
    if(point.x() >= maxPoint.x())
    {
        point.rx() = maxPoint.x();
    }
    if(point.y() >= maxPoint.y())
    {
        point.ry() = maxPoint.y();
    }
}

void CurvePlotter::drawCurve(QImage &image, QPoint left, QPoint right, int depth, bool mode)
{
    this->depth = depth;
    leftPoint = left;
    rightPoint = right;
    this->mode = mode; // 0 - zoomIn 1 - zoomOt
    WPrev = W;
    HPrev = H;
    leftPoint = QPoint(qMin(leftPoint.x(), rightPoint.x()), qMin(leftPoint.y(), rightPoint.y()));
    rightPoint = QPoint(qMax(leftPoint.x(), rightPoint.x()), qMax(leftPoint.y(), rightPoint.y()));
    validatePoint(leftPoint, QPoint(image.width(), image.height()));
    validatePoint(rightPoint, QPoint(image.width(), image.height()));
    W = qAbs(rightPoint.x() - leftPoint.x());
    H = qAbs(rightPoint.y() - leftPoint.y());
    int imageWidth = image.width();
    int imageHeight = image.height();
    if(firstDrawing)
    {
        center = QPoint(0, 0);
        WPrev = W;
        HPrev = H;
    }
    firstDrawing = false;
    if(mode && imageWidth != W)
    {
        center = getReal(image, QPoint(leftPoint.x() + W / 2, leftPoint.y() + H / 2));
        xScale = (imageWidth != W) ? xScale * W / imageWidth : 1;
        yScale = (imageWidth != W) ? yScale * H / imageHeight : 1;
    }
    QPointF f0 = getReal(image, leftPoint);
    QPointF f1 = getReal(image, rightPoint);
    /*find t wich get in this rectangle*/
    QVector<qreal> vt = QVector<qreal>();
    qreal t1 = Tx(f0.x());
    qreal yy = y(t1);
    if(yy <= f0.y() && yy >= f1.y())
        vt.push_back(t1);
    if(y(-t1) <= f0.y() && y(-t1) >= f1.y())
        vt.push_back(-t1);
    qreal t2 = Tx(f1.x());
    if(y(t2) <= f0.y() && y(t2) >= f1.y())
        vt.push_back(t2);
    if(y(-t2) <= f0.y() && y(-t2) >= f1.y())
        vt.push_back(-t2);
    qreal t3 = Ty(f1.y()) + f1.y() / 2;
    if(x(t3) >= f0.x() && x(t3) <= f1.x())
        vt.push_back(t3);
    t3 = -Ty(f1.y()) + f1.y() / 2;
    if(x(t3) >= f0.x() && x(t3) <= f1.x())
        vt.push_back(t3);
    qreal t4 = Ty(f0.y()) + f0.y() / 2;
    if(x(t4) >= f0.x() && x(t4) <= f1.x())
        vt.push_back(t4);
    t4 = -Ty(f0.y()) + f0.y() / 2;
    if(x(t4) >= f0.x() && x(t4) <= f1.x())
        vt.push_back(t4);
    qreal root1 = -1 - qSqrt(2);
    qreal root2 = -1 + qSqrt(2);
    QPoint point1 = getPixel(image, x(root1), y(root1));
    /*try if interval of  [-1 - sqrt(2), 0] get in this rectangle*/
    if(point1.x() > leftPoint.x() && point1.x() < rightPoint.x() && point1.y() > leftPoint.y() && point1.y() < rightPoint.y())
    {
        vt.push_back(root1);
    }
    QPoint point2 = getPixel(image, x(0), y(0));
    if(point2.x() > leftPoint.x() && point2.x() < rightPoint.x() && point2.y() > leftPoint.y() && point2.y() < rightPoint.y())
    {
        vt.push_back(0);
    }
    image.fill(QColor(255,255,255).rgba());
    /*draw axis in the nessesary scale (need to get previous scale for Zoom Out mode)*/
    if(!mode && imageWidth != W)
    {
        QPointF tmp_center = center;
        qreal tmp_xScale = xScale;
        qreal tmp_yScale = yScale;
        center = getReal(image, QPoint(leftPoint.x() + W / 2, leftPoint.y() + H / 2));
        qreal x_Scale = (mode == 0) ? (qreal)imageWidth / W : (qreal)W / imageWidth;
        qreal y_Scale = (mode == 0) ? (qreal)imageHeight / H : (qreal)H / imageHeight;
        xScale = (imageWidth != W) ? xScale * x_Scale : 1;
        yScale = (imageWidth != W) ? yScale * y_Scale : 1;
        drawAxis(image);
        center = tmp_center;
        xScale = tmp_xScale;
        yScale = tmp_yScale;
    }
    else
    {
        drawAxis(image);
    }
    /*draw six intervals*/
    QPointF p = getReal(image,QPoint(0, imageHeight));
    qreal val = -Ty(p.y()) + p.y() / 2;
    drawInterval(image, val, root1, vt, 1);
    val = Tx(getReal(image, QPoint(imageWidth, 0)).x());
    drawInterval(image, root1, -val, vt,0);
    drawInterval(image, 0, root2, vt, 0);
    p = getReal(image, QPoint(0, 0));
    val = Tx(p.x());
    drawInterval(image,root2, val, vt, 0);
    p = getReal(image, QPoint(imageWidth, 0));
    val = Tx(p.x());
    drawInterval(image, val, Ty(p.y()) + p.y()/2,vt, 0);
    p = getReal(image,QPoint(0, 0));
    val = Tx(p.x());
    drawInterval(image, -val, 0, vt, 1);
    /*update scale valuse for the ZoomIn mode*/
    if(imageWidth != W && !mode)
    {
        center = getReal(image, QPoint(leftPoint.x() + W / 2, leftPoint.y() + H / 2));
        xScale = (imageWidth != W) ? xScale * imageWidth / W : 1;
        yScale = (imageWidth != W) ? yScale * imageHeight / H : 1;
    }

}

void CurvePlotter::resetScale()
{
    xScale = 1;
    yScale = 1;
    center = QPointF(0, 0);
}

QPointF CurvePlotter::getScale()
{
    return QPointF(xScale, yScale);
}

QPointF CurvePlotter::getTickSize()
{
    qreal xTick = 1.0 / xScale;
    qreal yTick = 1.0 / yScale;
    return QPointF(xTick, yTick);
}

CurvePlotter::~CurvePlotter()
{
}
