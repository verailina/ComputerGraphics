#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPainter>
#include <QtCore/qmath.h>

const char *MainWindow::WINDOW_TITLE = "FIT0201EGOROVA_Sinus";
static const double SCALE = (4*M_PI)/MainWindow::CANVAS_WIDTH;
static const double unit = M_PI/4; // size of unit intervals of X and Y axis

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(WINDOW_TITLE);
    setFixedSize(WINDOW_WIDTH,WINDOW_HEIGHT);
    canvas = QImage (CANVAS_WIDTH, CANVAS_HEIGHT, QImage::Format_RGB888);
    canvas.fill(QColor(255, 255, 255));
    connect(ui->drawSin,SIGNAL(clicked()),this,SLOT(drawSin()));
    connect(ui->drawAxis, SIGNAL(clicked()),this,SLOT(drawAxis()));
    connect(ui->clear,SIGNAL(clicked()),this,SLOT(clear()));
}
void MainWindow::paintEvent(QPaintEvent *) {
     QPainter painter(this);
     painter.drawImage(CANVAS_SHIFT, CANVAS_SHIFT, canvas);
 }

void MainWindow::drawSin()
{
    for(int x = 0; x < CANVAS_WIDTH; x++)
    {
        double y = qSin((x - CANVAS_WIDTH / 2) * SCALE);
        y = - (y / SCALE) + CANVAS_HEIGHT / 2;
        y = static_cast<int>(y + 0.5);
        if(y <= CANVAS_HEIGHT && y >= 0)
        {
            canvas.setPixel(x, y, QColor(255, 0, 0).rgba());
            canvas.setPixel(x, y, QColor(255, 0, 0).rgba());
            canvas.setPixel(x, y, QColor(255, 0, 0).rgba());
        }
     }
    QMainWindow::update();
}

void MainWindow::drawAxis()
{
    /* draws X and Y axis */
    for(int i = 0; i < CANVAS_WIDTH; i++)
    {
        canvas.setPixel(i, CANVAS_HEIGHT/2, QColor(0, 0, 0).rgba());
        canvas.setPixel(CANVAS_WIDTH/2, i, QColor(0, 0, 0).rgba());
    }
    for(double i = (unit / SCALE); i < CANVAS_WIDTH; i += (unit / SCALE))
    {
        /*draws devisions on X axis*/
        int x = static_cast<int>(i + 0.5);
        canvas.setPixel(x,CANVAS_WIDTH/2-1, QColor(0, 0, 0).rgba());
        canvas.setPixel(x,CANVAS_WIDTH/2+1, QColor(0, 0, 0).rgba());
        canvas.setPixel(x,CANVAS_WIDTH/2-2, QColor(0, 0, 0).rgba());
        canvas.setPixel(x,CANVAS_WIDTH/2+2, QColor(0, 0, 0).rgba());
        /*draws devisions on Y axis*/
        if(i < CANVAS_HEIGHT)
        {
            canvas.setPixel(CANVAS_WIDTH/2-1, x, QColor(0, 0, 0).rgba());
            canvas.setPixel(CANVAS_WIDTH/2+1, x, QColor(0, 0, 0).rgba());
            canvas.setPixel(CANVAS_WIDTH/2-2, x, QColor(0, 0, 0).rgba());
            canvas.setPixel(CANVAS_WIDTH/2+2, x, QColor(0, 0, 0).rgba());
        }
    }
    QMainWindow::update();
}
void MainWindow::clear()
{
    canvas.fill(QColor(255, 255, 255));
    QMainWindow::update();
}
MainWindow::~MainWindow()
{
    delete ui;
}
