#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QPaintEvent>
#include <QLabel>
#include"imageconverter.h"
namespace Ui
{
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    int getScale();
    int getUnpackingType();
    Standart getStandart();
    bool convertImage(QString fileName = "");
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    ImageConverter converter; //Starts in the sepate thread to provide computations
    QLabel imageLabel;
    QVector<int> unpackCoeff;
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
private slots:
    void repaintImage(QString fileName = "");
    void updatePixmap(QImage image);
    void setGammaValue(int gamma);
    void setCoeff1Value(int coeff);
    void setCoeff2Value(int coeff);
    void setCoeff3Value(int coeff);
};
#endif // MAINWINDOW_H
