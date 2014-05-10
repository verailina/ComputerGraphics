#include "mainwindow.h"
#include "ui_mainwindow.h"
#include"imageconverter.h"
#include <QMimeData>
static const char TITLE[] = "FITEGOROVA0201_TVSet";
static const int FILES_NUMBER = 8;
static const QString FILE_NAMES[FILES_NUMBER] = {"aliceA.png","aliceB.png", "greenA.png", "greenB.png",
                                                "helloA.png", "helloB.png","spiderA.png", "spiderB.png"};
static const QString PREFIX = "://images//prefix//";
static const int GAMMA_SCALE = 10;
static const char SEPARATOR[] = ":/";
static const char FILE_NAME_PATTERN[] = "text/uri-list";
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    unpackCoeff = QVector<int>(ImageConverter::MAX_MACROPIXEL_NUMBER);
    for(int i = 0; i < ImageConverter::MAX_MACROPIXEL_NUMBER; i++)
    {
        unpackCoeff[i] = 1;
    }
    setWindowTitle(TITLE);
    qRegisterMetaType<QImage>("QImage");
    imageLabel.setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    ui->imageArea->setWidget(&imageLabel);
    setGammaValue(GAMMA_SCALE);
    for(int i = 0; i < FILES_NUMBER; i++)
    {
        ui->filesBox->addItem(FILE_NAMES[i]);
    }
    connect(ui->filesBox, SIGNAL(currentTextChanged(QString)), this, SLOT(repaintImage(QString)));
    connect(ui->gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(setGammaValue(int)));
    connect(ui->radioButtonBT601, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->radioButtonBT709, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->radioButton1x, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->radioButton2x, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->radioButton3x, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->radioButton4x, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->type1, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->type2, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->type3, SIGNAL(clicked()), this, SLOT(repaintImage()));
    connect(ui->gammaSlider, SIGNAL(valueChanged(int)), this, SLOT(repaintImage()));
    connect(&converter, SIGNAL(renderedImage(QImage)), this, SLOT(updatePixmap(QImage)));
    connect(ui->coeff1spinBox, SIGNAL(valueChanged(int)), this, SLOT(setCoeff1Value(int)));
    connect(ui->coeff2spinBox, SIGNAL(valueChanged(int)), this, SLOT(setCoeff2Value(int)));
    connect(ui->coeff3spinBox, SIGNAL(valueChanged(int)), this, SLOT(setCoeff3Value(int)));
    repaintImage();
}

bool MainWindow::convertImage(QString fileName)
{
    QImage image = QImage();
    bool result = false;
    if(fileName.isEmpty())
    {
        fileName = ui->filesBox->currentText();
    }
    /*If fileName contains substring ":/" it the file was loaded using Drag'n'Drop*/
    if(fileName.contains(SEPARATOR))
    {
        result = image.load(fileName);
    }
    else
    {
        result = image.load(PREFIX + fileName);
    }
    if(result)
    {
        double gamma = static_cast<double>(ui->gammaSlider->value());
        gamma /= GAMMA_SCALE;
        /*Send data to converter thread to recount and repaint image*/
        converter.render(image, gamma, getStandart(), getScale(), getUnpackingType(), unpackCoeff);
    }
    return result;
}

void MainWindow::repaintImage(QString fileName)
{
    convertImage(fileName);
}
/*Set coefficient value recieved from the spinBox*/

void MainWindow::setCoeff1Value(int coeff)
{
    unpackCoeff[0] = coeff;
    convertImage();
}

void MainWindow::setCoeff2Value(int coeff)
{
    unpackCoeff[1] = coeff;
    convertImage();
}

void MainWindow::setCoeff3Value(int coeff)
{
    unpackCoeff[2] = coeff;
    convertImage();
}

/*Set value of gamma to the text field*/
void MainWindow::setGammaValue(int value)
{
    double gamma = static_cast<double>(value) / GAMMA_SCALE;
    QString fieldValue = QString("%2").arg(gamma);
    ui->gammaField->setText(fieldValue);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasFormat(FILE_NAME_PATTERN))
        event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event)
{
    QList<QUrl> urls = event->mimeData()->urls();
    if(urls.isEmpty())
    {
        return;
    }
    QString fileName = urls.first().toLocalFile();
    if(fileName.isEmpty())
    {
        return;
    }
    bool result = convertImage(fileName);
    /*If the file got from Drag'n'Drop is the image add its name to the filesBox*/
    if(result)
    {
        if(ui->filesBox->findText(fileName) == -1)
        {
            ui->filesBox->addItem(fileName);
            ui->filesBox->setCurrentText(fileName);
        }
        else
        {
            ui->filesBox->setCurrentText(fileName);
        }
    }
}

int MainWindow::getScale()
{
    if(ui->radioButton1x->isChecked())
    {
        return 1;
    }
    if(ui->radioButton2x->isChecked())
    {
        return 2;
    }
    if(ui->radioButton3x->isChecked())
    {
        return 3;
    }
    if(ui->radioButton4x->isChecked())
    {
        return 4;
    }
    return 1;
}

/*Get type of the unpacking algorithm (number of macropixels) and block unnessesary coefficients*/
int MainWindow::getUnpackingType()
{
    if(ui->type1->isChecked())
    {
        ui->coeff1spinBox->setEnabled(false);
        ui->coeff2spinBox->setEnabled(false);
        ui->coeff3spinBox->setEnabled(false);
        return 1;
    }
    if(ui->type2->isChecked())
    {
        ui->coeff1spinBox->setEnabled(true);
        ui->coeff2spinBox->setEnabled(true);
        ui->coeff3spinBox->setEnabled(false);
        return 2;
    }
    if(ui->type3->isChecked())
    {
        ui->coeff1spinBox->setEnabled(true);
        ui->coeff2spinBox->setEnabled(true);
        ui->coeff3spinBox->setEnabled(true);
        return 3;
    }
    return 1;
}

Standart MainWindow::getStandart()
{
    if(ui->radioButtonBT601->isChecked())
    {
       return BT601;
    }
    return BT709;
}

/*Set the current image to the image got from the converter thread*/
void MainWindow::updatePixmap(QImage image)
{
    imageLabel.setPixmap(QPixmap::fromImage(image));
}

MainWindow::~MainWindow()
{
    delete ui;
}
