#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mapper.h"
#include <QColor>

static const char TITLE[] = "FIT0201EGOROVA_Morph";
static const int COLOR_SIZE = 8;
static const QColor COLORS[] = {QColor(255, 255, 255), QColor(255, 0, 0), QColor(255, 165, 0), QColor(255, 255, 0),
                                QColor(0, 255, 0), QColor(166, 202, 240), QColor(0, 0, 255), QColor(238, 82, 238)};
static const QString COLORS_NAME[] = {"White", "Red", "Orange", "Yellow",
                                      "Green", "Light Blue", "Blue", "Violet"};
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(TITLE);
    for(int i = 0; i < COLOR_SIZE; i++)
    {
        ui->colorComboBox->addItem(COLORS_NAME[i]);
    }
    ui->widget->setBackgroundColor(COLORS[0]);
    connect(ui->pointC_slider, SIGNAL(valueChanged(int)), this, SLOT(setPointC(int)));
    connect(ui->pointF_slider, SIGNAL(valueChanged(int)), this, SLOT(setPointF(int)));
    connect(ui->layerNearesRadioButton, SIGNAL(clicked()), this, SLOT(setLayerType()));
    connect(ui->layerLinearRadioButton, SIGNAL(clicked()), this, SLOT(setLayerType()));
    connect(ui->mipLinearRadioButton, SIGNAL(clicked()), this, SLOT(setMipType()));
    connect(ui->mipNearestRadioButton, SIGNAL(clicked()), this, SLOT(setMipType()));
    connect(ui->mipNoneRadioButton, SIGNAL(clicked()), this, SLOT(setMipType()));
    connect(ui->colorComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changeColor(int)));
    connect(ui->imageRadioButton, SIGNAL(clicked()), this, SLOT(setShowType()));
    connect(ui->textureRadioButton, SIGNAL(clicked()), this, SLOT(setShowType()));
    connect(ui->nearestMipRadioButton, SIGNAL(clicked()), this, SLOT(setShowType()));
}

void MainWindow::changeColor(int color)
{
    ui->widget->setBackgroundColor(COLORS[color]);
}

void MainWindow::setPointC(int value)
{
    ui->widget->setPointC(value);
}

void MainWindow::setPointF(int value)
{
    ui->widget->setPointF(value);
}

void MainWindow::setLayerType()
{
    if(ui->layerLinearRadioButton->isChecked())
    {
        ui->widget->setLayerType(LINEAR);
    }
    if(ui->layerNearesRadioButton->isChecked())
    {
        ui->widget->setLayerType(NEAREST);
    }
}

void MainWindow::setMipType()
{
    if(ui->mipLinearRadioButton->isChecked())
    {
        ui->widget->setMipType(LINEAR_MIP);
    }
    if(ui->mipNearestRadioButton->isChecked())
    {
        ui->widget->setMipType(NEAREST_MIP);
    }
    if(ui->mipNoneRadioButton->isChecked())
    {
        ui->widget->setMipType(NONE);
    }
}

void MainWindow::setShowType()
{
    if(ui->imageRadioButton->isChecked())
    {
        ui->widget->setShowType(SHOW_IMAGE);
    }
    if(ui->textureRadioButton->isChecked())
    {
        ui->widget->setShowType(SHOW_TEXTURE);
    }
    if(ui->nearestMipRadioButton->isChecked())
    {
        ui->widget->setShowType(SHOW_NEAREST_MIP);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
