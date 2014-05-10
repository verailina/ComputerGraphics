#include "teapotwidget.h"
#include "ui_teapotwidget.h"
#include <QPainter>
#include <QMouseEvent>

static const QString PREFIX = "://BPT_Files//";
static const QString TEAPOT_BPT_FILE = "teapot.txt";
static const int FILE_SCALES[5] = {100, 200, 350, 200, 70}; //scale coefficients of each figure

TeapotWidget::TeapotWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TeapotWidget)
{
    ui->setupUi(this);
    gridIsVisible = true;
    axisIsVisible = true;
    boundingBoxIsVisible = true;
    paintingIsVisible = true;
    partsNumber = 0;
    connectVisibility = true;
    scale = FILE_SCALES[0];
    type = BEZIER_CURVES;
    readBPTFile(TEAPOT_BPT_FILE);
    repaintImage(true);
}

void TeapotWidget::setDrawingType(FigureDrawingType type)
{
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgb());
    this->type = type;
    repaintImage(true);
}

void TeapotWidget::setConnection(bool value)
{
    connectVisibility = value;
    repaintImage(true);
}

/*Reading contorl points of the figure into control points*/
void TeapotWidget::readBPTFile(QString fileName)
{
    QFile file(PREFIX + fileName);
    file.open(QIODevice::ReadOnly);
    QTextStream in(&file);
    int patchesNumber = 0;
    int uDimension = 0;
    int vDimension = 0;
    controlPoints = QVector< QVector<QVector3D> >();
    QVector<QVector3D> tempControlPoints = QVector<QVector3D>();
    in >> patchesNumber;
    for(int k = 0; k < patchesNumber; k++)
    {
        in >> uDimension;
        in >> vDimension;
        for(int i = 0; i <= uDimension; i++)
        {
            for(int j = 0; j <= vDimension; j++)
            {
                qreal x = 0;
                qreal y = 0;
                qreal z = 0;
                in >> x;
                in >> y;
                in >> z;
                QVector3D newPoint = QVector3D(x, y, z);
                tempControlPoints.push_back(newPoint);
            }
        }
        controlPoints.push_back(tempControlPoints);
        tempControlPoints.clear();
    }
}

void TeapotWidget::repaintImage(bool isRecountNeeded)
{
    image = QImage(width(), height(), QImage::Format_RGB888);
    image.fill(QColor(255, 255, 255).rgb());
    if(isRecountNeeded)
    {
        handler.countFigure(image, controlPoints, type, partsNumber, scale);
    }
    handler.drawFigure(image, type, axisIsVisible, boundingBoxIsVisible, paintingIsVisible, gridIsVisible, connectVisibility);
    update();
}

void TeapotWidget::setPartsNumber(int value)
{
    partsNumber = value;
    repaintImage(true);
}

void TeapotWidget::setBPTFile(QString fileName, int number)
{
    controlPoints.clear();
    scale = FILE_SCALES[number];
    readBPTFile(fileName);
    repaintImage(true);
}

void TeapotWidget::setAxisVisibility(bool value)
{
    axisIsVisible = value;
    repaintImage(false);
}

void TeapotWidget::setBoundingBoxVisibility(int value)
{
    boundingBoxIsVisible = value;
    repaintImage(false);
}

void TeapotWidget::setPainting(int value)
{
    paintingIsVisible = value;
    repaintImage(false);
}

void TeapotWidget::setGridVisibility(int value)
{
    gridIsVisible = value;
    repaintImage(false);
}

void TeapotWidget::resizeEvent(QResizeEvent *)
{
    repaintImage(false);
}

void TeapotWidget::mousePressEvent(QMouseEvent * event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        curViewPoint = event->pos();
    }
}

void TeapotWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons() == Qt::LeftButton)
    {
        QPoint finishViewPoint = event->pos();
        handler.changeEyeView(QPoint((finishViewPoint.x() - curViewPoint.x()),
                                     (finishViewPoint.y() - curViewPoint.y())));
        repaintImage(false);
        curViewPoint = finishViewPoint;
    }
}

void TeapotWidget::wheelEvent(QWheelEvent *event)
{
    handler.zoom(event->angleDelta());
    repaintImage(false);
}

void TeapotWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

TeapotWidget::~TeapotWidget()
{
    delete ui;
}
