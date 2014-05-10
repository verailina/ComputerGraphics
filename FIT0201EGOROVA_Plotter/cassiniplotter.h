#ifndef CASSINIPlotter_H
#define CASSINIPlotter_H

#include <QPoint>
#include <QImage>

class CassiniPlotter : public QObject
{
    Q_OBJECT
public:
    explicit CassiniPlotter(QObject *parent = 0);
    ~CassiniPlotter();
    void drawAxis(QImage &image);
    void drawFocus(QImage &image, QPoint focus);
    void drawLine(QImage &image, QPoint point1, QPoint point2);
    long long getError(QPoint focus1, QPoint focus2, int r, int x, int y);
    void drawCassini(QImage &image, QPoint focus1, QPoint focus2, int R, int depth);
    void setPixel(QImage &image, int x, int y, int depth, QRgb color);
private:
    QPoint center1;
    QPoint center2;
    int r;
    int height;
    int width;
};
#endif // CASSINIPlotter_H
