#include "mapper.h"
#include <qmath.h>
#include <QColor>

static const int BIG_ERROR_VALUE = 100000;
static const char PREFIX[] = "://textures//";
static const char TEXTURE_FILE_NAME[] = "morph_src1024.png";

Mapper::Mapper()
{
    QImage firstMip = QImage();
    mipLevels = QVector<QImage>();
    firstMip.load(QString(PREFIX) + QString(TEXTURE_FILE_NAME));
    generateMipLevels(firstMip);
    texture = mipLevels[0];
    layer = NEAREST;
    mip = NONE;
    showType = SHOW_IMAGE;
}

/*Draw one line of pixels into the border area*/
void Mapper::drawLine(QImage &image, int pointFrom, int pointTo, int pointFix)
{
    for(int p = pointFrom; p < pointTo; p++)
    {
        int i = p;
        int j = pointFix;
        QPointF point = converter.getTextureCoordinates(QPoint(i, j));
        QVector<QPointF> pointRect = converter.getTextureRect(QPoint(i, j));
        point = getMirror(point);
        if(mip == NONE)
        {
            texture = mipLevels[0];
        }
        if(mip == NEAREST_MIP)
        {
            texture = getNearestMip(pointRect);
        }
        QRgb color = QColor(255, 255, 255).rgba();
        if(showType == SHOW_IMAGE)
        {
            if(mip == LINEAR_MIP)
            {
                color = getPixelTrilinearFilter(pointRect, point);
            }
            else
            {
                if(layer == NEAREST)
                {
                    color = getPixelNearest(point);
                }
                if(layer == LINEAR)
                {
                    color = getPixelBilinearFilter(point);
                }
            }
        }
        else
        {
            if(showType == SHOW_TEXTURE)
            {
                qreal red = point.x() * 200;
                qreal blue = point.y() * 200;
                qreal green = 0;
                color = qRgb(red, green, blue);
            }
            else if(showType == SHOW_NEAREST_MIP)
            {
                if(mip == LINEAR_MIP)
                {
                    color = getPixelTrilinearFilter(pointRect, point);
                }
                QColor mipColor = QColor(255, 255, 255);
                for(int i = 0; i <= mipNumber; i++)
                {
                    mipColor = mipColor.darker(130);
                }
                color = mipColor.rgba();
            }
        }
        int centerX = static_cast<int>(image.width() / 2 + 0.5) - 128;
        int centerY = static_cast<int>(image.height() / 2 + 0.5) - 128;
        if(i + centerX < image.width() && j + centerY < image.height() && i + centerX >= 0 && j + centerY >= 0)
        {
            image.setPixel(i + centerX, j + centerY, color);
        }
    }
}

/*Map the texture on the border area. Separate texture into lines*/
void Mapper::map(QImage &image, QPoint leftTop, QPoint rightBottom, QPoint leftBottom,
                 QPoint rightTop, const QVector<QPointF> &uvTops)
{
    tops = QVector<QPoint>();
    tops.push_back(leftTop);
    tops.push_back(rightTop);
    tops.push_back(leftBottom);
    tops.push_back(rightBottom);
    converter = ConverterUV();
    converter.createMatrix(tops, uvTops);
    int left = leftTop.x();
    int right = rightBottom.x();
    int top = leftTop.y();
    int bottom = rightBottom.y();
    qreal t = 0;
    while(top < bottom)
    {
        t = (qreal)(top - leftTop.y())/(qreal)(leftBottom.y() - leftTop.y());
        qreal leftF = static_cast<qreal>(leftBottom.x() - leftTop.x()) * t +
                      static_cast<qreal>(leftTop.x());
        left = (leftF >= 0) ? static_cast<int>(leftF + 0.5) : static_cast<int>(leftF - 0.5);
        qreal rightF = static_cast<qreal>(rightBottom.x() - rightTop.x()) * t +
                       static_cast<qreal>(rightTop.x());
        right = (rightF >= 0) ? static_cast<int>(rightF + 0.5) : static_cast<int>(rightF - 0.5);
        drawLine(image, qMin(left, right), qMax(left, right), top);
        top++;
    }
}

/*Get interpolation value by mixing rgb1 and rgb2 with the coeeficient alpha*/
QRgb Mapper::bilinearInterpolation(QRgb rgb1, QRgb rgb2, qreal alpha)
{
    qreal red = (1 - alpha) * qRed(rgb1) + alpha * qRed(rgb2);
    qreal green = (1 - alpha) * qGreen(rgb1) + alpha * qGreen(rgb2);
    qreal blue = (1 - alpha) * qBlue(rgb1) + alpha * qBlue(rgb2);
    qreal alphaChannel = (1 - alpha) * qAlpha(rgb1) + alpha * qAlpha(rgb2);
    int redInt = static_cast<int>(red + 0.5);
    int greenInt = static_cast<int>(green + 0.5);
    int blueInt = static_cast<int>(blue + 0.5);
    int alphaInt = static_cast<int>(alphaChannel + 0.5);
    return qRgba(redInt, greenInt, blueInt, alphaInt);
}

/*Count pixel components with using algorithm of bilinear interpolation*/
QRgb Mapper::getPixelBilinearFilter(QPointF point)
{
    point *= texture.width();
    point -= QPointF(0.5, 0.5);
    int x0 = static_cast<int>(point.x());
    int x1 = static_cast<int>(point.x() + 1);
    int y0 = static_cast<int>(point.y());
    int y1 = static_cast<int>(point.y() + 1);
    qreal alpha = countDist(point, QPointF(x0, point.y()));
    qreal beta = countDist(point, QPointF(point.x(), y0));
    if(validatePoint(QPoint(x0, y0)))
    {
        if(validatePoint(QPoint(x1, y0)))
        {
            if(validatePoint(QPoint(x0, y1)))
            {
                if(validatePoint(QPoint(x1, y1)))
                {
                    QRgb pixelA = texture.pixel(x0, y0);
                    QRgb pixelB = texture.pixel(x1, y0);
                    QRgb pixelC = texture.pixel(x0, y1);
                    QRgb pixelD = texture.pixel(x1, y1);
                    QRgb pixelM = bilinearInterpolation(pixelA, pixelB, alpha);
                    QRgb pixelN = bilinearInterpolation(pixelC, pixelD, alpha);
                    return bilinearInterpolation(pixelM, pixelN, beta);
                }
                else
                {
                    return texture.pixel(x0, y1);
                }
            }
            else
            {
                return texture.pixel(x1, y0);
            }
        }
        else
        {
            return texture.pixel(x0, y0);
        }
    }
    return qRgb(255, 255, 255);
}

/*Count pixel components with using algorithm of nearest value*/
QRgb Mapper::getPixelNearest(QPointF point)
{
    point *= texture.width();
    point += QPointF(-0.5, -0.5);
    int x0 = static_cast<int>(point.x());
    int x1 = static_cast<int>(point.x() + 1);
    int y0 = static_cast<int>(point.y());
    int y1 = static_cast<int>(point.y() + 1);
    int pointNum = -1;
    qreal curDist = BIG_ERROR_VALUE;
    QVector<QPoint> tops = QVector<QPoint>();
    tops.push_back(QPoint(x0, y0));
    tops.push_back(QPoint(x1, y0));
    tops.push_back(QPoint(x0, y1));
    tops.push_back(QPoint(x1, y1));
    for(int k = 0; k < tops.size(); k++)
    {
        qreal dist = countDist(point, tops[k]);
        if(curDist > dist)
        {
            curDist = dist;
            pointNum = k;
        }
    }
    if(validatePoint(tops[pointNum]))
    {
        return texture.pixel(tops[pointNum].x(), tops[pointNum].y());
    }
    else
    {
        return qRgb(255, 255, 255);
    }
}

/*Count the lenght of the maximum size of the rectangle into texture coordinates*/
qreal Mapper::countMaxPixelRectSideLenght(const QVector<QPointF> &pointRect)
{
    qreal maxDistance = -1;
    QVector<qreal> sideLenght = QVector<qreal>();
    sideLenght.push_back(countDist(pointRect[0], pointRect[1]));
    sideLenght.push_back(countDist(pointRect[1], pointRect[3]));
    sideLenght.push_back(countDist(pointRect[2], pointRect[3]));
    sideLenght.push_back(countDist(pointRect[0], pointRect[2]));
    for(int i = 0; i < sideLenght.size(); i++)
    {
        if(maxDistance < sideLenght[i])
        {
            maxDistance = sideLenght[i];
        }
    }
    return maxDistance;
}

/*Count pixel components with using algorithm of trilinear interpolation*/
QRgb Mapper::getPixelTrilinearFilter(const QVector<QPointF> &pointRect, QPointF point)
{
    qreal maxDistance = countMaxPixelRectSideLenght(pointRect);
    int maxMipLevelNumber = 0;
    int minMipLevelNumber = 0;
    qreal currentError = BIG_ERROR_VALUE;
    int sideSize = 1024;
    /*Find the most nearest mip level*/
    for(int i = 0; i < mipLevels.size(); i++)
    {
        qreal unit = maxDistance * sideSize;
        qreal error = qAbs(1.0 - unit);
        {
            if(currentError > error)
            {
                currentError = error;
                minMipLevelNumber = i;
            }
        }
        sideSize /= 2;
    }
    /*Count second mipLevel*/
    currentError = maxDistance * mipLevels[minMipLevelNumber].width();
    if(currentError >= 1)
    {
        maxMipLevelNumber = (minMipLevelNumber != mipLevels.size() - 1) ? minMipLevelNumber + 1 :
                             minMipLevelNumber;
    }
    else
    {
        maxMipLevelNumber = (minMipLevelNumber != 0) ? minMipLevelNumber - 1 :
                             minMipLevelNumber;
    }
    int k0 = qMin(minMipLevelNumber, maxMipLevelNumber);
    int k1 = qMax(minMipLevelNumber, maxMipLevelNumber);
    mipNumber = minMipLevelNumber;
    QRgb minRgb = QColor().rgba();
    QRgb maxRgb = QColor().rgba();
    /*Count pixels commponents in the each mip level*/
    if(layer == LINEAR)
    {
        texture = mipLevels[k0];
        maxRgb = getPixelBilinearFilter(point);
        texture = mipLevels[k1];
        minRgb = getPixelBilinearFilter(point);
    }
    if(layer == NEAREST)
    {
        texture = mipLevels[k0];
        maxRgb = getPixelNearest(point);
        texture = mipLevels[k1];
        minRgb = getPixelNearest(point);
    }
    if(k0 == k1)
    {
        return maxRgb;
    }
    qreal sideMax = maxDistance * (qreal)mipLevels[k0].width();
    qreal sideMin = maxDistance * (qreal)mipLevels[k1].width();
    /*Count coefficient of mixing values of pixel components in the aech level*/
    qreal d = (1 - qMin(sideMin, sideMax)) / qAbs(sideMin - sideMax);
    mipNumber = minMipLevelNumber;
    return bilinearInterpolation(minRgb, maxRgb, d);
}

QImage Mapper::getNearestMip(const QVector<QPointF> &pointRect)
{
    qreal maxDistance = countMaxPixelRectSideLenght(pointRect);
    int mipLevelNumber = 0;
    int currentError = 1000000;
    int sideSize = 1024;
    for(int i = 0; i < mipLevels.size(); i++)
    {
        qreal unit = maxDistance * sideSize;
        qreal error = qAbs(1.0 - unit);
        if(currentError > error)
        {
            currentError = error;
            mipLevelNumber = i;
        }
        sideSize /= 2;
    }
    mipNumber = mipLevelNumber;
    return mipLevels[mipLevelNumber];
}

QImage Mapper::getNextMipLevel(QImage source)
{
    QImage nextLevel = QImage(source.width() / 2, source.height() / 2, QImage::Format_ARGB32);
    nextLevel.fill(QColor(255, 255, 255).rgba());
    for(int i = 0; i < source.width() - 1; i += 2)
    {
        for(int j = 0; j < source.height() - 1; j += 2)
        {
            QRgb pixel1 = source.pixel(i, j);
            QRgb pixel2 = source.pixel(i + 1, j);
            QRgb pixel3 = source.pixel(i, j + 1);
            QRgb pixel4 = source.pixel(i + 1, j + 1);
            qreal red = static_cast<qreal>(qRed(pixel1) + qRed(pixel2) + qRed(pixel3) + qRed(pixel4)) / 4;
            qreal green = static_cast<qreal>(qGreen(pixel1) + qGreen(pixel2) + qGreen(pixel3) + qGreen(pixel4)) / 4;
            qreal blue = static_cast<qreal>(qBlue(pixel1) + qBlue(pixel2) + qBlue(pixel3) + qBlue(pixel4)) / 4;
            qreal alpha = static_cast<qreal>(qAlpha(pixel1) + qAlpha(pixel2) + qAlpha(pixel3) + qAlpha(pixel4)) / 4;
            int redInt = static_cast<int>(red + 0.5);
            int greenInt = static_cast<int>(green + 0.5);
            int blueInt = static_cast<int>(blue + 0.5);
            int alphaInt = static_cast<int>(alpha + 0.5);
            nextLevel.setPixel(i / 2, j / 2, qRgba(redInt, greenInt, blueInt, alphaInt));
        }
    }
    return nextLevel;
}

void Mapper::generateMipLevels(QImage image)
{
    int size = 1024;
    mipLevels.push_back(image);
    while(size != 1)
    {
        QImage newImage = getNextMipLevel(image);
        mipLevels.push_back(newImage);
        image = newImage;
        size /= 2;
    }
}

bool Mapper::validatePoint(QPoint point)
{
    if(point.x() >= 0 && point.y() >= 0 &&
       point.x() < texture.width() && point.y() < texture.height())
    {
        return true;
    }
    return false;
}

QPointF Mapper::getMirror(QPointF point)
{
    qreal x = point.x();
    qreal y = point.y();
    if(x < 0)
    {
        x *= -1;
    }
    if(y < 0)
    {
        y *= -1;
    }
    /*count x position in the square 2x2*/
    int xInt = qFloor(x / 2);
    x -= 2 * xInt;
    /*if x get into interval (1, 2) it need to be translated*/
    if(x >= 1)
    {
        int roundX = static_cast<int>(x + 0.5);
        x = qAbs(roundX - x);
    }
    /*count y position in the square 2x2*/
    int yInt = qFloor(y / 2);
    y -= 2 * yInt;
    /*if y get into the interval (1,2) it need to translated*/
    if(y >= 1)
    {
        int roundY = static_cast<int>(y + 0.5);
        y = qAbs(roundY - y);
    }
    return QPointF(x, y);
}

qreal Mapper::countDist(QPointF point1, QPointF point2)
{
    return qSqrt(qPow(point1.x() - point2.x(), 2) +
                 qPow(point1.y() - point2.y(), 2));
}

void Mapper::setLayerType(Layer _layer)
{
    layer = _layer;
}

void Mapper::setMipType(Mip _mip)
{
    mip = _mip;
}

void Mapper::setShowType(Type type)
{
    showType = type;
}

QImage Mapper::testMip(int number)
{
    return mipLevels[number];
}
