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
    static const char *WINDOW_TITLE;
    static const int WINDOW_WIDTH = 480;  // fixed sizes of the window
    static const int WINDOW_HEIGHT = 520;
    static const int CANVAS_WIDTH = 460; // sizes of the canvas
    static const int CANVAS_HEIGHT = 460;
    static const int CANVAS_SHIFT = 10; // the canvas shift from the window's borders
    explicit MainWindow(QWidget *parent = 0);
    void paintEvent(QPaintEvent *);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    QImage canvas; // the image contains graphic of sinus
private slots:
   void drawSin();
   void drawAxis();
   void clear();
};

#endif // MAINWINDOW_H
