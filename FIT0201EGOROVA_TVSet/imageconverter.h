#ifndef IMAGECONVERTER_H
#define IMAGECONVERTER_H
#include <QImage>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QVector>

enum Standart
{
    BT601,
    BT709
};
class ImageConverter : public QThread
{
    Q_OBJECT
public:
    static const int MAX_MACROPIXEL_NUMBER = 3;
    ImageConverter(QObject *parent = 0);
    ~ImageConverter();
    void render(const QImage image, double gamma, Standart st, int scale, int unpackType, const QVector<int> &coeffs);
    void unpackFromOneMacropixel();
    void unpackFromTwoMacropixels(const QVector<int> &coeffs);
    void unpackFromThreeMacropixels(const QVector<int> &coeffs);
    void gammaCorrection(double gamma);
    void toRGB(Standart standart);
    void resize(int scale);
signals:
    void renderedImage(QImage image);
protected:
        void run();
private:
        QImage sourceImage;
        QImage processImage;
        QMutex mutex;
        QWaitCondition condition;
        double gamma;
        Standart standart;
        int unpackType;
        int scale;
        QVector<int> unpackCoeffs;
        bool restart;
        bool abort;
};
#endif // IMAGECONVERTER_H

