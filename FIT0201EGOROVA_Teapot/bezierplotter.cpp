#include "bezierplotter.h"
#include "qmath.h"

static const int BIG_VALUE = 1000;
static const qreal START_PLOT_STEP = 0.001;
static const int DIMENSION = 3;

BezierPlotter::BezierPlotter(QObject *parent) :
    QObject(parent)
{
    minFigurePoint = QVector3D(BIG_VALUE, BIG_VALUE, BIG_VALUE);
    maxFigurePoint = QVector3D(-BIG_VALUE, -BIG_VALUE, -BIG_VALUE);
    binomialCoeff = QVector<qreal>();
    connectVisibility = true;
    for(int k = 0; k <= DIMENSION; k++)
    {
        binomialCoeff.push_back(factorial(DIMENSION) / (factorial(k) * factorial(DIMENSION - k)));
    }
}
void BezierPlotter::setConnectVisibility(bool value)
{
    connectVisibility = value;
}

/*Drawing Bezier Curve by connecting points with lines. In this case the curve consist of pattsNumber + 1 parts*/
void BezierPlotter::drawBezierCurveWithLines(QImage &image, const QVector<QVector3D> &points, Graphics &graphics, QColor color)
{
    QVector3D shift = 0.5 * (maxFigurePoint + minFigurePoint);
    for(int i = 0; i + 1 < points.size(); i++)
    {
        QVector3D pointFrom3D = points[i] - shift;
        QVector3D pointTo3D = points[i + 1] - shift;
        if(connectVisibility)
        {
            graphics.drawLine(image, pointFrom3D, pointTo3D, color, true);
        }
        else
        {
            if(graphics.isVisible(pointTo3D))
            {
                QPoint p1 = graphics.getPointProjection(pointFrom3D,image.width(), image.height());
                if(p1.x() >= 0 && p1.y() >= 0 && p1.x() < image.width() && p1.y() < image.height())
                {

                    image.setPixel(p1, QColor(0, 0, 0).rgba());
                }
            }
            if(graphics.isVisible(pointFrom3D))
            {
                QPoint p1 = graphics.getPointProjection(pointTo3D,image.width(), image.height());
                if(p1.x() >= 0 && p1.y() >= 0 && p1.x() < image.width() && p1.y() < image.height())
                {
                    image.setPixel(p1, QColor(0, 0, 0).rgba());
                }
            }
        }
    }
}

/*Drawing Bezier Curve by points (without connecting them with lines)*/
void BezierPlotter::drawBezierCurve(QImage &image, const QVector<QVector3D> &points, Graphics &graphics, QColor color)
{
    QVector3D shift = 0.5 * (maxFigurePoint + minFigurePoint);
    for(int j = 0; j < points.size(); j++)
    {
        QVector3D point3D = points[j] - shift;
        if(graphics.isVisible(point3D))
        {
            QPoint point = graphics.getPointProjection(point3D, image.width(), image.height());
            if(point.x() > 0 && point.y() > 0 && point.x() < image.width() && point.y() < image.height())
            {
                image.setPixel(point, color.rgb());
            }
        }
    }
}

/*Recounting control points of each figure's patch*/
void BezierPlotter::countFigure(QImage &image, const QVector<QVector<QVector3D> > &controlPoints, Graphics &graphics,
                                FigureDrawingType type, int partsNumber, int scale)
{
    this->partsNumber = partsNumber;
    this->scale = scale;
    controlPointsHorizontal.clear();
    controlPointsVertical.clear();
    minFigurePoint = QVector3D(1000, 1000, 1000);
    maxFigurePoint = QVector3D(-1000, -1000, -1000);
    for(int i = 0; i < controlPoints.size(); i++)
    {
        switch(type)
        {
            case BEZIER_LINES :
            {
                countBezierSurfaceWithLines(controlPoints[i]);
                break;
            }
            case BEZIER_CURVES :
            {
                countBezierSurface(controlPoints[i]);
                break;
            }
            case BEZIER_CURVES_ADAPT :
            {
                countBezierSurfaceAdapt(image, controlPoints[i], graphics);
                break;
            }
        }
    }
}

/*Drawing figure by connecting points with lines*/
void BezierPlotter::drawFigureWithLines(QImage &image, Graphics &graphics)
{
    for(int i = 0; i < controlPointsHorizontal.size(); i++)
    {
        drawBezierCurveWithLines(image, controlPointsHorizontal[i], graphics, QColor(0, 0, 0));
    }
    for(int i = 0; i < controlPointsVertical.size(); i++)
    {
        drawBezierCurveWithLines(image, controlPointsVertical[i], graphics, QColor(0, 0, 0));
    }
}

/*Drawing figure by points*/
void BezierPlotter::drawFigure(QImage &image, Graphics &graphics)
{
    for(int i = 0; i < controlPointsVertical.size(); i++)
    {
        drawBezierCurve(image, controlPointsVertical[i], graphics, QColor(0, 0, 0));
    }
    for(int i = 0; i < controlPointsHorizontal.size(); i++)
    {
        drawBezierCurve(image, controlPointsHorizontal[i], graphics, QColor(0, 0, 0));
    }
}

/*Painting a rectangle by separating it with to triangles*/
void BezierPlotter::paintRectangle(QImage &image, const QVector<QVector3D> &top, const QVector<QVector3D> &bottom,
                                   int i, Graphics &graphics)
{
    QVector3D shift = 0.5 * (maxFigurePoint + minFigurePoint);
    QVector3D vertex1 = top[i] - shift;
    QVector3D vertex2 = top[i + 1] - shift;
    QVector3D vertex3 = bottom[i] - shift;
    QVector3D vertex4 = bottom[i + 1] - shift;
    graphics.paintTriangle(image, vertex1, vertex2, vertex3, partsNumber, QColor(255, 0, 0));
    graphics.paintTriangle(image, vertex2, vertex4, vertex3, partsNumber, QColor(255, 0, 0));
}

/*Painting each patch of figure*/
void BezierPlotter::paintFigure(QImage &image, Graphics &graphics)
{
    for(int i = 0; i < controlPointsHorizontal.size() - 1; i++)
    {
        /*If the excpression gets 0 value than we have finised drawing of this patch*/
        if((i + 1) % controlPointsHorizontal[i].size() != 0)
        {
            for(int j = 0; j < controlPointsHorizontal[i].size() - 1; j++)
            {
                paintRectangle(image, controlPointsHorizontal[i], controlPointsHorizontal[i + 1], j, graphics);
            }
        }
    }
}

qreal BezierPlotter::countBernsteinPolynomial(int dimension, int polynomNumber, qreal u)
{
    return binomialCoeff[polynomNumber] * qPow(u, polynomNumber) * qPow(1 - u, dimension - polynomNumber);
}

/*Count points of Bezier curve by using fixed step*/
QVector<QVector3D> BezierPlotter::countBezierCurve(const QVector<QVector3D> &uControlPoints, qreal step)
{
    QVector<QVector3D> vector = QVector<QVector3D>();
    for(qreal v = 0; v <= 1.0 + step / 2 ; v += step)
    {        
        qreal x = 0;
        qreal y = 0;
        qreal z = 0;
        for(int i = 0; i <= DIMENSION; i++)
        {
            x += uControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, v);
            y += uControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, v);
            z += uControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, v);
        }
        QVector3D new3DPoint = QVector3D(x * scale, y * scale, z * scale);
        vector.push_back(new3DPoint);
        countMinMaxFigurePoints(new3DPoint);
    }
    return vector;
}

/*Counting points of the Bezier surface by using the algorithm with a fixed step for drawing BezierCurve*/
void BezierPlotter::countBezierSurface(const QVector<QVector3D> &controlPoints)
{
    qreal gridStep = 1.0 / (partsNumber + 1);
    qreal curveStep = 0.1;
    QVector<QVector3D> uControlPoints = QVector<QVector3D>();
    QVector<QVector3D> vControlPoints = QVector<QVector3D>();
    /*Counting points of horizontal curves*/
    for(qreal u = 0; u <= 1 + gridStep / 2; u += gridStep)
    {
        uControlPoints = QVector<QVector3D>();
        for(int t = 0; t <= DIMENSION; t++)
        {
            qreal x = 0;
            qreal y = 0;
            qreal z = 0;
            for(int p = 0; p <= DIMENSION; p++)
            {
                x += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].x();
                y += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].y();
                z += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].z();
            }
            uControlPoints.push_back(QVector3D(x, y, z));
        }
        controlPointsHorizontal.push_back(countBezierCurve(uControlPoints, curveStep));
    }
    /*Counting points of vertical curves*/
    for(qreal v = 0; v <= 1 + gridStep / 2; v += gridStep)
    {
       vControlPoints = QVector<QVector3D>();
       for(int t = 0; t <= DIMENSION; t++)
       {           
           qreal x = 0;
           qreal y = 0;
           qreal z = 0;
           for(int p = 0; p <= DIMENSION; p++)
           {
               x += countBernsteinPolynomial(DIMENSION, p, v)
                       * controlPoints[p * (DIMENSION + 1) + t].x();
               y += countBernsteinPolynomial(DIMENSION, p, v)
                       * controlPoints[p * (DIMENSION + 1) + t].y();
               z += countBernsteinPolynomial(DIMENSION, p, v)
                       * controlPoints[p * (DIMENSION + 1) + t].z();
           }
           vControlPoints.push_back(QVector3D(x, y, z));
       }
       controlPointsHorizontal.push_back(countBezierCurve(vControlPoints, curveStep));
    }
}

/*Countig adaptive step for drawing the Bezier Curve*/
qreal BezierPlotter::countStep(qreal startStep, qreal v, QPoint lastPoint, QImage &image, const QVector<QVector3D> &uControlPoints, Graphics &graphics)
{
    qreal step = startStep;
    while(step > START_PLOT_STEP * 10000.0)
    {
        qreal x = 0;
        qreal y = 0;
        qreal z = 0;
        for(int i = 0; i <= DIMENSION; i++)
        {
            x += uControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, v + step);
            y += uControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, v + step);
            z += uControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, v + step);
        }
        QVector3D new3DPoint = QVector3D(x * scale, y * scale, z * scale);
        QPoint newPoint = graphics.getPointProjection(new3DPoint, image.width(), image.height());
        /*If the distanse to the next pixel less then 1 we got step*/
        if(qAbs(newPoint.x() - lastPoint.x()) <= 1 && qAbs(newPoint.y() - lastPoint.y()) <= 1)
        {
            return step;
        }
        else
        {
            step /= 2;
        }
        x = 0;
        y = 0;
        z = 0;
    }
    return START_PLOT_STEP;
}

/*Counting control points of Bezier surface by using the algorithm with a adaptive step for drawing BezierCurve*/
void BezierPlotter::countBezierSurfaceAdapt(QImage &image, const QVector<QVector3D> &controlPoints, Graphics &graphics)
{
    QVector<QVector3D> vector = QVector<QVector3D>();
    QPoint lastPoint = QPoint();
    qreal step = 0;
    qreal gridStep = 1.0 / (partsNumber + 1);
    QVector<QVector3D> uControlPoints = QVector<QVector3D>();
    QVector<QVector3D> vControlPoints = QVector<QVector3D>();
    /*Counting  points of the horizontal curves*/
    for(qreal u = 0; u <= 1 + gridStep / 2; u += gridStep)
    {        
        qreal x = 0;
        qreal y = 0;
        qreal z = 0;
        /*Counting contol points for the curve defined by parameter u*/
        uControlPoints = QVector<QVector3D>();
        for(int t = 0; t <= DIMENSION; t++)
        {
            for(int p = 0; p <= DIMENSION; p++)
            {
                x += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].x();
                y += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].y();
                z += countBernsteinPolynomial(DIMENSION, p, u)
                        * controlPoints[t * (DIMENSION + 1) + p].z();
            }
            uControlPoints.push_back(QVector3D(x, y, z));
            x = 0;
            y = 0;
            z = 0;
        }
        /*Counting the first point of the curve*/
        for(int i = 0; i <= 1; i++)
        {
            x += uControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, 0);
            y += uControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, 0);
            z += uControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, 0);
        }        
        QVector3D last3DPoint = QVector3D(x * scale, y * scale, z * scale);
        vector.push_back(last3DPoint);
        countMinMaxFigurePoints(last3DPoint);
        lastPoint = graphics.getPointProjection(last3DPoint, image.width(), image.height());
        x = 0;
        y = 0;
        z = 0;        
        /*Counting points of the curve*/
        step = START_PLOT_STEP;
        for(qreal v = 0; v <= 1.0 + START_PLOT_STEP / 2;)
        {
            step = countStep(step, v, lastPoint, image, uControlPoints, graphics);
            for(int i = 0; i <= DIMENSION; i++)
            {
                x += uControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, v + step);
                y += uControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, v + step);
                z += uControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, v + step);
            }
            QVector3D new3DPoint = QVector3D(x * scale, y * scale, z * scale);
            QPoint newPoint = graphics.getPointProjection(new3DPoint, image.width(), image.height());
            vector.push_back(new3DPoint);
            countMinMaxFigurePoints(new3DPoint);
            lastPoint = newPoint;
            v += step;
            x = 0;
            y = 0;
            z = 0;
        }
        controlPointsHorizontal.push_back(vector);
        vector.clear();
    }
    /*Counting  points of the vertival curves*/
    for(qreal v = 0; v <= 1 + gridStep / 2; v += gridStep)
    {
        qreal x = 0;
        qreal y = 0;
        qreal z = 0;
        vControlPoints = QVector<QVector3D>();
        /*Counting control points of the curve defined by parameter v*/
        for(int t = 0; t <= DIMENSION; t++)
        {
            for(int p = 0; p <= DIMENSION; p++)
            {
                x += countBernsteinPolynomial(DIMENSION, p, v) * controlPoints[p * (DIMENSION + 1) + t].x();
                y += countBernsteinPolynomial(DIMENSION, p, v) * controlPoints[p * (DIMENSION + 1) + t].y();
                z += countBernsteinPolynomial(DIMENSION, p, v) * controlPoints[p * (DIMENSION + 1) + t].z();
            }
            vControlPoints.push_back(QVector3D(x, y, z));
            x = 0;
            y = 0;
            z = 0;
        }
        /*Counting the first point of the curve*/
        for(int i = 0; i <= 1; i++)
        {
            x += vControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, 0);
            y += vControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, 0);
            z += vControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, 0);
        }
        QVector3D last3DPoint = QVector3D(x * scale, y * scale, z * scale);
        vector.push_back(last3DPoint);
        countMinMaxFigurePoints(last3DPoint);
        lastPoint = graphics.getPointProjection(last3DPoint, image.width(), image.height());
        x = 0;
        y = 0;
        z = 0;
        /*Counting points of the curve*/
        step = START_PLOT_STEP;
        for(qreal u = 0; u <= 1.0 + START_PLOT_STEP / 2;)
        {
            step = countStep(step, u, lastPoint, image, vControlPoints, graphics);
            for(int i = 0; i <= DIMENSION; i++)
            {
                x += vControlPoints[i].x() * countBernsteinPolynomial(DIMENSION, i, u + step);
                y += vControlPoints[i].y() * countBernsteinPolynomial(DIMENSION, i, u + step);
                z += vControlPoints[i].z() * countBernsteinPolynomial(DIMENSION, i, u + step);
            }
            QVector3D new3DPoint = QVector3D(x * scale, y * scale, z * scale);
            QPoint newPoint = graphics.getPointProjection(new3DPoint, image.width(), image.height());
            vector.push_back(new3DPoint);
            countMinMaxFigurePoints(new3DPoint);
            lastPoint = newPoint;
            u += step;
            x = 0;
            y = 0;
            z = 0;
        }
        controlPointsVertical.push_back(vector);
        vector.clear();
    }
}

/*Counting control points of Bezier surface by connecting Bezier curves' points with lines*/
void BezierPlotter::countBezierSurfaceWithLines(const QVector<QVector3D> &controlPoints)
{
    QVector<QVector3D> vector = QVector<QVector3D>();
    qreal gridStep = 1.0 / (partsNumber + 1);
    /*Counting points of the horizontal curves*/
    for(qreal u = 0; u <= 1 + gridStep / 2; u += gridStep)
    {
        for(qreal v = 0; v <= 1 + gridStep / 2; v += gridStep)
        {
            qreal x = 0;
            qreal y = 0;
            qreal z = 0;
            for(int i = 0; i <= DIMENSION; i++)
            {
                for(int j = 0; j <= DIMENSION; j++)
                {
                    x += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1) + j].x();
                    y += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1) + j].y();
                    z += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1)+ j].z();
                }
            }
            vector.push_back(QVector3D(x * scale, y * scale, z * scale));
            countMinMaxFigurePoints(QVector3D(x * scale, y * scale, z * scale));
        }
        controlPointsHorizontal.push_back(vector);
        vector.clear();
    }
    /*Counting points of the vertical curves*/
    for(qreal v = 0; v <= 1 + gridStep / 2; v += gridStep)
    {
        for(qreal u = 0; u <= 1 + gridStep / 2; u += gridStep)
        {
            qreal x = 0;
            qreal y = 0;
            qreal z = 0;
            for(int j = 0; j <= DIMENSION; j++)
            {
                for(int i = 0; i <= DIMENSION; i++)
                {
                    x += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1) + j].x();
                    y += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1) + j].y();
                    z += countBernsteinPolynomial(DIMENSION, i, u)
                         * countBernsteinPolynomial(DIMENSION, j, v)
                         * controlPoints[i * (DIMENSION + 1)+ j].z();;
                }
            }
            vector.push_back(QVector3D(x * scale, y * scale, z * scale));
            countMinMaxFigurePoints(QVector3D(x * scale, y * scale, z * scale));
        }
        controlPointsVertical.push_back(vector);
        vector.clear();
    }
}

/*Count the left bootom point and the righr upper points of the bounding box*/
void BezierPlotter::countMinMaxFigurePoints(QVector3D point)
{
    if(point.x() < minFigurePoint.x())
    {
        minFigurePoint.setX(point.x());
    }
    if(point.y() < minFigurePoint.y())
    {
        minFigurePoint.setY(point.y());
    }
    if(point.z() < minFigurePoint.z())
    {
        minFigurePoint.setZ(point.z());
    }
    if(point.x() > maxFigurePoint.x())
    {
        maxFigurePoint.setX(point.x());
    }
    if(point.y() > maxFigurePoint.y())
    {
        maxFigurePoint.setY(point.y());
    }
    if(point.z() > maxFigurePoint.z())
    {
        maxFigurePoint.setZ(point.z());
    }
}

QVector3D BezierPlotter::getMinPoint()
{
    return minFigurePoint;
}

QVector3D BezierPlotter::getMaxPoint()
{
    return maxFigurePoint;
}

qreal BezierPlotter::factorial(int n)
{
    int res = 1;
    if(n == 0)
    {
        return 1;
    }
    while(n != 1)
    {
        res *= n;
        n--;
    }
    return res;
}
