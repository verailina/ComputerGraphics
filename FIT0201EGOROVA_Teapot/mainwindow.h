#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void setKValue(int value);
    void setDrawingType();
    void setAxisVisibility(bool value);
    void setBoundingBoxVisibility(bool value);
    void setBptFile(QString fileName);
    void setPaintingVisibility(bool value);
    void setGridVisibility(bool value);
    void setConnection(bool value);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
