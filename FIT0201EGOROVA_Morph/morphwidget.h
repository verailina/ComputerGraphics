#ifndef MORPHWIDGET_H
#define MORPHWIDGET_H

#include <QWidget>
#include "mapper.h"

namespace Ui {
class MorphWidget;
}

class MorphWidget : public QWidget
{
    Q_OBJECT
    
public:
    explicit MorphWidget(QWidget *parent = 0);
    void drawLine(QPoint point1, QPoint point2);
    void drawRect();
    void drawPoint(QPoint point);
    void setPointC(int valueX);
    void setPointF(int valueX);
    void setLayerType(Layer layer);
    void setMipType(Mip mip);
    void setShowType(Type type);
    void setBackgroundColor(QColor newColor);
    void updateImage();
    ~MorphWidget();
protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
private:
    Ui::MorphWidget *ui;
    QImage image;
    QPoint m_pointC;
    QPoint m_pointF;
    Mapper mapper;
    Layer layerType;
    Mip mipType;
    QColor backgroundColor;
    int deltaC;
    int deltaF;

};

#endif // MORPHWIDGET_H
