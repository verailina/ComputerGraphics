#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "teapothandler.h"

static const char TITLE[] = "FIT0201EGOROVA_Teapot";
static const int FILES_NUMBER = 5;
static const QString FILE_NAMES[FILES_NUMBER] = {"teapot.txt", "teacup.txt", "teaspoon.txt",
                                                 "sphere.txt", "heart.txt"};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(TITLE);
    setKValue(0);
    setDrawingType();
    for(int i = 0; i < FILES_NUMBER; i++)
    {
        ui->bptComboBox->addItem(FILE_NAMES[i]);
    }
    ui->paintingCheckBox->setDisabled(true);
    connect(ui->bezierCurveRadioButton, SIGNAL(clicked()), this, SLOT(setDrawingType()));
    connect(ui->bezierCurveAdaptRadioButton, SIGNAL(clicked()), this, SLOT(setDrawingType()));
    connect(ui->bezierLinesRadioButton, SIGNAL(clicked()), this, SLOT(setDrawingType()));
    connect(ui->KhorizontalSlider, SIGNAL(valueChanged(int)), this, SLOT(setKValue(int)));
    connect(ui->axisCheckBox, SIGNAL(clicked(bool)), this, SLOT(setAxisVisibility(bool)));
    connect(ui->boundingCheckBox, SIGNAL(clicked(bool)), this, SLOT(setBoundingBoxVisibility(bool)));
    connect(ui->bptComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(setBptFile(QString)));
    connect(ui->paintingCheckBox, SIGNAL(clicked(bool)), this, SLOT(setPaintingVisibility(bool)));
    connect(ui->gridCheckBox, SIGNAL(clicked(bool)), this, SLOT(setGridVisibility(bool)));
    connect(ui->connectCheckBox, SIGNAL(clicked(bool)), this, SLOT(setConnection(bool)));
}

void MainWindow::setConnection(bool value)
{
    ui->teapotWidget->setConnection(value);
}

void MainWindow::setDrawingType()
{
    if(ui->bezierCurveRadioButton->isChecked())
    {
        ui->teapotWidget->setDrawingType(BEZIER_CURVES);
        ui->paintingCheckBox->setDisabled(true);
    }
    if(ui->bezierCurveAdaptRadioButton->isChecked())
    {
        ui->teapotWidget->setDrawingType(BEZIER_CURVES_ADAPT);
        ui->paintingCheckBox->setDisabled(true);
    }
    if(ui->bezierLinesRadioButton->isChecked())
    {
        ui->teapotWidget->setDrawingType(BEZIER_LINES);
        ui->paintingCheckBox->setEnabled(true);
    }
}

void MainWindow::setBptFile(QString fileName)
{
    int index = ui->bptComboBox->currentIndex();
    ui->teapotWidget->setBPTFile(fileName, index);
}

void MainWindow::setKValue(int value)
{
    ui->KlineEdit->setText(QString("%2").arg(value));
    ui->teapotWidget->setPartsNumber(value);
}

void MainWindow::setPaintingVisibility(bool value)
{
    ui->teapotWidget->setPainting(value);
}

void MainWindow::setGridVisibility(bool value)
{
    ui->teapotWidget->setGridVisibility(value);
}

void MainWindow::setAxisVisibility(bool value)
{
    ui->teapotWidget->setAxisVisibility(value);
}

void MainWindow::setBoundingBoxVisibility(bool value)
{
    ui->teapotWidget->setBoundingBoxVisibility(value);
}

MainWindow::~MainWindow()
{
    delete ui;
}
