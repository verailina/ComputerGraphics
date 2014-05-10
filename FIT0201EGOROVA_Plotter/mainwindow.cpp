#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QBitmap>
static char WINDOW_TITLE[] = "FIT0201EGOROVA_Plotter";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(WINDOW_TITLE);
    setRValue(ui->rSlider->value());
    connect(ui->rSlider, SIGNAL(valueChanged(int)), this, SLOT(setRValue(int)));
    connect(ui->cassiniDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCassiniDepth(int)));
    connect(ui->curveDepthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setCurveDepth(int)));
    connect(ui->Curve, SIGNAL(scaleChaged(QPointF, QPointF)), this, SLOT(setScaleValues(QPointF, QPointF)));
    connect(ui->resetScaleButton, SIGNAL(clicked()), this, SLOT(resetScale()));

}
void MainWindow::setRValue(int value)
{
    ui->rEdit->setText(QString("%1").arg(value));
    ui->Cassini->setRValue(value);
}

void MainWindow::setCassiniDepth(int value)
{
    ui->Cassini->setDepth(value);
}

void MainWindow::setCurveDepth(int value)
{
    ui->Curve->setDepth(value);
}

void::MainWindow::setScaleValues(QPointF scale, QPointF tickSize)
{
    ui->xScaleEdit->setText(QString("%3").arg(scale.x()));
    ui->yScaleEdit->setText(QString("%3").arg(scale.y()));
    ui->xTickEdit->setText(QString("%3").arg(tickSize.x()));
    ui->yTickEdit->setText(QString("%3").arg(tickSize.y()));
}

void MainWindow::resetScale()
{
    ui->Curve->resetScale();
}

MainWindow::~MainWindow()
{
    delete ui;
}
