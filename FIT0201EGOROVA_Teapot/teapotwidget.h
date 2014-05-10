#ifndef TEAPOTWIDGET_H
#define TEAPOTWIDGET_H

#include <QWidget>
#include "teapothandler.h"

namespace Ui {
class TeapotWidget;
}

class TeapotWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit TeapotWidget(QWidget *parent = 0);
    ~TeapotWidget();
    void readBPTFile(QString fileName);
    void repaintImage(bool isRecountNeeded);
    void setBPTFile(QString fileName, int number);
    void setDrawingType(FigureDrawingType type);
    void setPartsNumber(int value);
    void setAxisVisibility(bool value);
    void setBoundingBoxVisibility(int value);
    void setPainting(int value);
    void setGridVisibility(int value);
    void setConnection(bool value);
protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);
    void resizeEvent(QResizeEvent *);
private:
    Ui::TeapotWidget *ui;
    QImage image;
    TeapotHandler handler;
    QPoint curViewPoint;
    QVector<QVector3D> rect;
    QPoint mouseStep;
    int partsNumber;
    int scale;
    bool axisIsVisible;
    bool boundingBoxIsVisible;
    bool paintingIsVisible;
    bool gridIsVisible;
    bool connectVisibility;
    FigureDrawingType type;
    QVector< QVector<QVector3D> > controlPoints;
};

#endif // TEAPOTWIDGET_H
