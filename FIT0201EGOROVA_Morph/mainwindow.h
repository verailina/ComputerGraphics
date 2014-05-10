#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void setPointC(int value);
    void setPointF(int value);
    void setLayerType();
    void setMipType();
    void setShowType();
    void changeColor(int);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
