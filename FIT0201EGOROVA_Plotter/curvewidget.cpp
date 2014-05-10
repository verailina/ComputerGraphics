#include "curvewidget.h"
#include "ui_curvewidget.h"
#include <QPainter>
#include <QMouseEvent>

CurveWidget::CurveWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CurveWidget)
{
    ui->setupUi(this);
    image = QImage(width(), height(), QImage::Format_RGB888);
    depth = 1;
    xScale = 1;
    yScale = 1;
    xTick = 0;
    yTick = 0;
    mode = 0;
}

void CurveWidget::setImage(QImage newImage)
{
    image = newImage;
    update();
}

void CurveWidget::resetScale()
{
    plotter.resetScale();
    plotter.drawCurve(image, QPoint(0,0), QPoint(image.width(), image.height()), depth, mode);
    update();
    emit scaleChaged(plotter.getScale(), plotter.getTickSize());
}

void CurveWidget::setDepth(int value)
{
    depth = value;
    plotter.drawCurve(image, QPoint(0,0), QPoint(image.width(), image.height()), depth, mode);
    update();
}

void CurveWidget::resizeEvent(QResizeEvent *)
{
    image = QImage(width(), height(), QImage::Format_ARGB32);
    image.fill(QColor(255,255,255).rgba());
    plotter.drawCurve(image, QPoint(0,0), QPoint(image.width(), image.height()), depth, mode);
    emit scaleChaged(plotter.getScale(), plotter.getTickSize());
    update();
}

void CurveWidget::mousePressEvent(QMouseEvent *event)
{
    if((event->buttons() == Qt::LeftButton))
    {
        rectLeft = event->pos();
        tmpImage = image;
        mode = 0;
    }
    if((event->buttons() == Qt::RightButton))
    {
        rectLeft = event->pos();
        tmpImage = image;
        mode = 1;
    }
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() == Qt::LeftButton))
    {
        image = tmpImage;
        rectRight = event->pos();
        plotter.drawRect(image, rectLeft, rectRight);
        update();
    }
    if((event->buttons() == Qt::RightButton))
    {
        image = tmpImage;
        rectRight = event->pos();
        plotter.drawRect(image, rectLeft, rectRight);
        update();
    }
}

void CurveWidget::mouseReleaseEvent(QMouseEvent *event)
{
    image = tmpImage;
    rectRight = event->pos();
    plotter.drawCurve(image, rectLeft, rectRight, depth, mode);
    emit scaleChaged(plotter.getScale(), plotter.getTickSize());
    update();
}

void CurveWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

CurveWidget::~CurveWidget()
{
    delete ui;
}
