#include "imageconverter.h"
#include <qmath.h>
#include <QColor>
static const qreal SCREEN_GAMMA = 2.2;
static const int Y_MAX = 235;
static const int Y_MIN = 16;
static const int P_MAX = 240;
static const int P_MIN = 16;
static const qreal P_SHIFT = 0.5;
static const qreal KR_BT601 = 0.299;
static const qreal KB_BT601 = 0.114;
static const qreal KR_BT709 = 0.2126;
static const qreal KB_BT709 = 0.0722;
ImageConverter::ImageConverter(QObject *parent) :
    QThread(parent)
{
    restart = false;
    abort = false;
}

ImageConverter::~ImageConverter()
{
    mutex.lock();
    abort = true;
    condition.wakeOne();
    mutex.unlock();
    wait();
}

void ImageConverter::run()
{
    forever
    {
        mutex.lock();
        double gammaCoeff = gamma;
        processImage = sourceImage;
        Standart st = standart;
        int scaleCoeff = scale;
        int type = unpackType;
        QVector<int> coeffs = unpackCoeffs;
        mutex.unlock();
        if(abort)
        {
            return;
        }
        switch(type)
        {
        case 1:
            unpackFromOneMacropixel();
            break;
        case 2:
            unpackFromTwoMacropixels(coeffs);
            break;
        case 3:
            unpackFromThreeMacropixels(coeffs);
        default:
            break;
        }
        gammaCorrection(gammaCoeff);
        toRGB(st);
        resize(scaleCoeff);
        if(!restart)
        {
            emit renderedImage(processImage);
        }
        mutex.lock();
        if(!restart)
        {
            condition.wait(&mutex);
        }
        restart = false;
        mutex.unlock();
    }
}

void ImageConverter::render(const QImage newImage, double newGamma, Standart newStandart, int newScale, int newUnpackType, const QVector<int> &newCoeffs)
{
    QMutexLocker locker(&mutex);
    sourceImage = newImage;
    gamma = newGamma;
    standart = newStandart;
    scale = newScale;
    unpackType = newUnpackType;
    unpackCoeffs = newCoeffs;
    if(!isRunning())
    {
        start(LowPriority);
    }
    else
    {
        restart = true;
        condition.wakeOne();
    }
}

void ImageConverter::unpackFromOneMacropixel()
{
    QImage newImage = QImage(processImage.width() * 2, processImage.height(), QImage::Format_RGB888);
    for(int i = 0; i < processImage.height(); i++)
    {
        for(int j = 0; j < processImage.width(); j++)
        {
            QRgb mp = processImage.pixel(j, i);
            /*Get components values of the macropixel*/
            int Y0 = qAlpha(mp);
            int U = qRed(mp);
            int Y1 = qGreen(mp);
            int V = qBlue(mp);
            /*Get two pixels from macropixel by duplicating values U and V*/
            newImage.setPixel(j * 2, i, qRgb(Y0, U, V));
            newImage.setPixel(j * 2 + 1, i, qRgb(Y1, U, V));
        }
        if(restart || abort)
        {
            break;
        }
    }
    processImage = newImage;
}

void ImageConverter::unpackFromTwoMacropixels(const QVector<int> &parts)
{
    QImage newImage = QImage(processImage.width() * 2, processImage.height(), QImage::Format_RGB888);
    for(int i = 0; i < processImage.height(); i++)
    {
        for(int j = 0; j < processImage.width(); j++)
        {
            QRgb macropixel1 = processImage.pixel(j, i);
            /*Gets color components of rigth macropixel, if it is absent it takes components values of the first pixel*/
            QRgb macropixel2 = (j + 1 < processImage.width()) ? processImage.pixel(j + 1, i) : processImage.pixel(j, i);
            int Y0 = qAlpha(macropixel1);
            int U0 = qRed(macropixel1);
            int Y1 = qGreen(macropixel1);
            int V0 = qBlue(macropixel1);
            int U1 = qRed(macropixel2);
            int V1 = qBlue(macropixel2);
            /*Count coefficients before components U and V of each macropixel.
            Parts array contains relations between the components U0 : U1 = V0 : V1 = part[0] : part[1]*/
            qreal coeffs[MAX_MACROPIXEL_NUMBER - 1];
            int partsNum = 0;
            partsNum = parts[0] + parts[1];
            for(int k = 0; k < MAX_MACROPIXEL_NUMBER - 1; k++)
            {
                coeffs[k] = static_cast<qreal>(parts[k]) / partsNum;
            }
            newImage.setPixel(j * 2, i, qRgb(Y0, U0, V0));
            /*Get two pixels from two macropixels using components U and V with coefficients of array coeff[]*/
            int U = static_cast<int>(coeffs[0] * U0 + coeffs[1] * U1);
            int V = static_cast<int>(coeffs[0] * V0 + coeffs[1] * V1);
            newImage.setPixel(j * 2 + 1, i, qRgb(Y1, U, V));
        }
        if(restart || abort)
        {
            break;
        }
    }
    processImage = newImage;
}

void ImageConverter::unpackFromThreeMacropixels(const QVector<int> &parts)
{
    QImage newImage = QImage(processImage.width() * 2, processImage.height(), QImage::Format_RGB888);
    for(int i = 0; i < processImage.height(); i++)
    {
        for(int j = 0; j < processImage.width(); j++)
        {
            /*Getting color components of 3 macropixels.
            If second or third pixel is absent it takes components values of the first pixel*/
            QRgb macropixel1 = processImage.pixel(j, i);
            QRgb macropixel2 = (j + 1 < processImage.width()) ? processImage.pixel(j + 1, i) : processImage.pixel(j, i);
            QRgb macropixel3 = (j + 2 < processImage.width()) ? processImage.pixel(j + 2, i) : processImage.pixel(j, i);
            int Y0 = qAlpha(macropixel1);
            int U0 = qRed(macropixel1);
            int Y1 = qGreen(macropixel1);
            int V0 = qBlue(macropixel1);
            int U1 = qRed(macropixel2);
            int V1 = qBlue(macropixel2);
            int U2 = qRed(macropixel3);
            int V2 = qBlue(macropixel3);
            /*Counting coefficients before components U and V of each macropixel.
            Parts array contains relations between the components U0 : U1 : U2 = V0 : V1 : V2= part[0] : part[1] : part[2]*/
            qreal coeffs[MAX_MACROPIXEL_NUMBER];
            int partsNum = 0;
            partsNum = parts[0] + parts[1] + parts[2];
            for(int k = 0; k < MAX_MACROPIXEL_NUMBER; k++)
            {
                coeffs[k] = static_cast<qreal>(parts[k]) / partsNum;
            }
            newImage.setPixel(j * 2, i, qRgb(Y0, U0, V0));
            /*Get two pixels from three macropixels using components U and V with coefficients of array coeff[]*/
            int U = static_cast<int>(coeffs[0] * U0 + coeffs[1] * U1 + coeffs[2] * U2);
            int V = static_cast<int>(coeffs[0] * V0 + coeffs[1] * V1 + coeffs[2] * V2);
            newImage.setPixel(j * 2 + 1, i, qRgb(Y1, U, V));
        }
        if(restart || abort)
        {
            break;
        }
    }
    processImage = newImage;
}

void ImageConverter::gammaCorrection(double gamma)
{
    for(int i = 0; i < processImage.height(); i++)
    {
        for(int j = 0; j < processImage.width(); j++)
        {
            QRgb rgb = processImage.pixel(j, i);
            QColor color(rgb);
            qreal Y = color.redF();
            /*Count new value of pixel brightness considering screen gamma*/
            Y = qPow(Y, gamma / SCREEN_GAMMA);
            color.setRedF(Y);
            processImage.setPixel(j, i, color.rgb());
        }
        if(restart || abort)
        {
            break;
        }
    }
}

void ImageConverter::toRGB(Standart st)
{
    qreal Kr = 0.0;
    qreal Kb = 0.0;
    switch(st)
    {
    case BT601:
        Kr = KR_BT601;
        Kb = KB_BT601;
        break;
    case BT709:
        Kr = KR_BT709;
        Kb = KB_BT709;
        break;
    default:
        break;
    }
    for(int i = 0; i < processImage.height(); i++)
    {
        for(int j = 0; j < processImage.width(); j++)
        {
            QRgb pixel = processImage.pixel(j, i);
            /*Normalize components Y, Pb, Pr*/
            qreal Y = static_cast<qreal>(qRed(pixel) - Y_MIN) / (Y_MAX - Y_MIN);
            qreal Pb = static_cast<qreal>(qGreen(pixel) - P_MIN) / (P_MAX - P_MIN) - P_SHIFT;
            qreal Pr = static_cast<qreal>(qBlue(pixel) - P_MIN) / (P_MAX - P_MIN) - P_SHIFT;
            /*Convert from YUV to RGB format*/
            qreal R = 2 * (Pr) * (1 - Kr) + Y;
            qreal B = 2 * (Pb) * (1 - Kb) + Y;
            qreal G = (Y - Kr * R - Kb * B) / (1 - Kr - Kb);
            /*Set R, G, B to range [0, 255]*/
            int normR = static_cast<int>(R * 255);
            int normG = static_cast<int>(G * 255);
            int normB = static_cast<int>(B * 255);
            normR = qMin(normR, 255);
            normG = qMin(normG, 255);
            normB = qMin(normB, 255);
            normR = qMax(0, normR);
            normG = qMax(0, normG);
            normB = qMax(0, normB);
            processImage.setPixel(j, i, QColor(normR, normG, normB).rgb());
        }
        if(restart || abort)
        {
            break;
        }
    }
}

void ImageConverter::resize(int scale)
{
    if(scale > 1 && scale < 5)
    {
        QImage newImage = QImage(processImage.width() * scale, processImage.height() * scale, QImage::Format_RGB888);
        /*Scale image by replace one pixel by scale * scale pixels with the same color components*/
        for(int i = 0; i < processImage.height(); i++)
        {
            for(int j = 0; j < processImage.width(); j++)
            {
                QRgb pixel = processImage.pixel(j, i);
                for(int k = j * scale; k < (j + 1) * scale; k++)
                {
                    for(int m = i * scale; m < (i + 1) * scale; m++)
                    {
                        newImage.setPixel(k, m, pixel);
                    }
                }
            }
        }
        processImage = newImage;
    }
}
