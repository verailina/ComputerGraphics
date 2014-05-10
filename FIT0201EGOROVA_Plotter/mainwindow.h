#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setRValue(int value);
    void setCassiniDepth(int value);
    void setCurveDepth(int value);
    void setScaleValues(QPointF scale, QPointF tickSize);
    void resetScale();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
