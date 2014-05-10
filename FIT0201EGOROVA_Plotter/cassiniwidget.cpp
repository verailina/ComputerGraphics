#include "cassiniwidget.h"
#include "ui_cassiniwidget.h"
#include <QPainter>
#include <QDebug>
#include <QMouseEvent>

const QPoint LEMNISCATE_FOCUS1 = QPoint(-1, 2);
const QPoint LEMNISCATE_FOCUS2 = QPoint(2, -2);
const int UNIT_SIZE = 40;
const int LEMNISCATE_R_VALUE = 10;

CassiniWidget::CassiniWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CassiniWidget)
{
    ui->setupUi(this);
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgb());
    plotter.drawAxis(image);
    focus1 = LEMNISCATE_FOCUS1 * UNIT_SIZE;
    focus2 = LEMNISCATE_FOCUS2 * UNIT_SIZE;
    rValue = LEMNISCATE_R_VALUE;
    gotFirstFocus = false;
    depth = 1;
}

CassiniWidget::~CassiniWidget()
{
    delete ui;
}

void CassiniWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

/*translate canvas cooddinates into coordinates in the axis system*/
QPoint CassiniWidget::getPixel(QPoint coordinates)
{
    int shiftX = static_cast<int>(width() / 2 + 0.5);
    int shiftY = static_cast<int>(height() / 2 + 0.5);
    QPoint shift = QPoint(shiftX, shiftY);
    QPoint point = QPoint(coordinates);
    point.ry()*= -1;
    point += shift;
    return point;
}

void CassiniWidget::setRValue(int value)
{
    this->rValue = value;
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgba());
    plotter.drawFocus(image, getPixel(focus1));
    plotter.drawFocus(image, getPixel(focus2));
    plotter.drawAxis(image);
    plotter.drawCassini(image, focus1, focus2, rValue, depth);
    update();
}

void CassiniWidget::setDepth(int value)
{
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgba());
    plotter.drawFocus(image, getPixel(focus1));
    plotter.drawFocus(image, getPixel(focus2));
    plotter.drawAxis(image);
    depth = value;
    plotter.drawCassini(image, focus1, focus2, rValue, depth);
    update();
}

void CassiniWidget::setImage(QImage newImage)
{
    image = newImage;
    update();
}

void CassiniWidget::resizeEvent(QResizeEvent *)
{
    imageTemp = image;
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgba());
    plotter.drawFocus(image, getPixel(focus1));
    plotter.drawFocus(image, getPixel(focus2));
    plotter.drawAxis(image);
    plotter.drawCassini(image, focus1, focus2, rValue, depth);
    update();
}

void CassiniWidget::mousePressEvent(QMouseEvent *event)
{
    if((event->buttons() == Qt::LeftButton))
    {
        focus1 = event->pos();
        plotter.drawFocus(image, focus1);
        gotFirstFocus = true;
        imageTemp = image;
        update();
    }
}

void CassiniWidget::mouseMoveEvent(QMouseEvent *event)
{
    if((event->buttons() & Qt::LeftButton))
    {
        image = imageTemp;
        focus2 = event->pos();
        plotter.drawLine(image, focus1, focus2);
    }
    update();
}

void CassiniWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if(gotFirstFocus)
    {
        focus2 = event->pos();
        plotter.drawFocus(imageTemp, focus2);
        image = imageTemp;
        int shiftX = static_cast<int>(width() / 2 + 0.5);
        int shiftY = static_cast<int>(height() / 2 + 0.5);
        QPoint shift = QPoint(shiftX, shiftY);
        image = QImage(width(), height(), QImage::Format_RGB888);
        image.fill(QColor(255, 255, 255).rgba());
        plotter.drawFocus(image, focus1);
        plotter.drawFocus(image, focus2);
        /*translate focuses coordinates into canvas coordinates*/
        focus1 -= shift;
        focus1.ry() *= -1;
        focus2 -= shift;
        focus2.ry() *= -1;
        plotter.drawAxis(image);
        plotter.drawCassini(image, focus1, focus2, rValue, depth);
        update();
        gotFirstFocus = false;
    }
}
