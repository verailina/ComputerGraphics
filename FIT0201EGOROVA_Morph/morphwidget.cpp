#include "morphwidget.h"
#include "ui_morphwidget.h"
#include "converteruv.h"
#include <QPainter>
#include <QPoint>
static const int HALF_RECT_SIDE = 129;
MorphWidget::MorphWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MorphWidget)
{
    ui->setupUi(this);
    m_pointC = QPoint(129, 0);
    m_pointF = QPoint(-129, 0);
    backgroundColor = QColor(255, 255, 255);
    layerType = NEAREST;
    mipType = NONE;
    deltaC = 255;
    deltaF = 0;
    mapper.setLayerType(layerType);
    mapper.setMipType(mipType);
    updateImage();
}

void MorphWidget::updateImage()
{
    image.fill(QColor(255, 255, 255, 0).rgba());
    QVector<QPointF> uvTops = QVector<QPointF>();
    uvTops.push_back(QPointF(0, 0));
    uvTops.push_back(QPointF(1, 0));
    uvTops.push_back(QPointF(0, 0.5));
    uvTops.push_back(QPointF(1, 0.5));
    mapper.map(image, QPoint(0, 0), QPoint(deltaC, 128), QPoint(deltaF, 128), QPoint(255, 0), uvTops);
    uvTops = QVector<QPointF>();
    uvTops.push_back(QPointF(0, 0.5));
    uvTops.push_back(QPointF(1, 0.5));
    uvTops.push_back(QPointF(0, 1));
    uvTops.push_back(QPointF(1, 1));
    mapper.map(image, QPoint(deltaF, 128), QPoint(255, 255), QPoint(0, 255), QPoint(deltaC, 128), uvTops);    
    drawRect();
    update();
}

void MorphWidget::setBackgroundColor(QColor newColor)
{
    backgroundColor = newColor;
    updateImage();
}

void MorphWidget::setPointC(int valueX)
{
    m_pointC = QPoint(129 + valueX, 0);
    deltaC = 255 + valueX;
    updateImage();
}

void MorphWidget::setPointF(int valueX)
{
    m_pointF = QPoint(-129 + valueX, 0);
    deltaF = valueX;
    updateImage();
}

void MorphWidget::setLayerType(Layer layer)
{
    mapper.setLayerType(layer);
    updateImage();
}

void MorphWidget::setMipType(Mip mip)
{
    mapper.setMipType(mip);
    updateImage();
}

void MorphWidget::setShowType(Type type)
{
    mapper.setShowType(type);
    updateImage();
}

void MorphWidget::drawLine(QPoint point1, QPoint point2)
{
    int x1 = point1.x();
    int x2 = point2.x();
    int y1 = point1.y();
    int y2 = point2.y();
    int deltaX = qAbs(x2 - x1);
    int deltaY = qAbs(y2 - y1);
    int signX = x1 < x2 ? 1 : -1;
    int signY = y1 < y2 ? 1 : -1;
    int error = deltaX - deltaY;
    if(x2 >= 0 && x2 < image.width() && y2 >= 0 && y2 < image.height())
    {
        image.setPixel(x2, y2, QColor(0, 0, 0).rgba());
    }
    while(x1 != x2 || y1 != y2)
    {
        if(x1 >= 0 && x1 < image.width() && y1 >= 0 && y1 < image.height())
        {
            image.setPixel(x1, y1, QColor(0, 0, 0).rgba());
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

void MorphWidget::drawPoint(QPoint point)
{
    for(int i = -2; i <= 2; i++)
    {
        for(int j = -2; j <= 2; j++)
        {
            if(point.x() + i > 0 && point.y() + j > 0 && point.x() + i < image.width() && point.y() + j < image.height())
            {
                image.setPixel(point + QPoint(i, j), QColor(0, 0, 255).rgba());
            }
        }
    }
}

void MorphWidget::drawRect()
{
    int centerX = static_cast<int>(width() / 2 + 0.5);
    int centerY = static_cast<int>(height() / 2 + 0.5);
    QPoint pointA = QPoint(-HALF_RECT_SIDE + centerX, -HALF_RECT_SIDE + centerY);
    QPoint pointB = QPoint(HALF_RECT_SIDE + centerX - 2, -HALF_RECT_SIDE + centerY);
    QPoint pointC = QPoint(m_pointC.x() + centerX - 2, m_pointC.y() + centerY - 1);
    QPoint pointD = QPoint(HALF_RECT_SIDE + centerX - 2, HALF_RECT_SIDE + centerY - 2);
    QPoint pointF = QPoint(m_pointF.x() + centerX, m_pointF.y() + centerY - 1);
    QPoint pointE = QPoint(-HALF_RECT_SIDE + centerX, HALF_RECT_SIDE + centerY - 2);
    drawLine(pointA, pointB);
    drawLine(pointE, pointD);
    drawLine(pointB, pointC);
    drawLine(pointC, pointD);
    drawLine(pointA, pointF);
    drawLine(pointF, pointE);
}
void MorphWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QImage background = QImage(width(), height(), QImage::Format_ARGB32);
    background.fill(backgroundColor);
    painter.drawImage(0, 0, background);
    painter.drawImage(0, 0, image);
}

void MorphWidget::resizeEvent(QResizeEvent *)
{
    image = QImage(width(), height(), QImage::Format_ARGB32);
    image.fill(QColor(255, 255, 255, 0).rgba());
    updateImage();
}

MorphWidget::~MorphWidget()
{
    delete ui;
}
