#include "converteruv.h"
#include <QPointF>

ConverterUV::ConverterUV()
{  matrix = QVector<QVector<qreal> >();
    rightPart = QVector<qreal>();
    uvConvertion = QVector<qreal>();
}

/*Form matrix and the right part ob the system of linear equations*/
void ConverterUV::createMatrix(const QVector<QPoint> &xyzTops, const QVector<QPointF> &uvTops)
{
    for(int i = 0; i < xyzTops.size(); i++)
    {
        /*equation with parameter u*/
        QVector<qreal> row = QVector<qreal>();
        row.push_back(xyzTops[i].x());
        row.push_back(xyzTops[i].y());
        row.push_back(1.0);
        row.push_back(0.0);
        row.push_back(0.0);
        row.push_back(0.0);
        row.push_back(-xyzTops[i].x() * uvTops[i].x());
        row.push_back(-xyzTops[i].y() * uvTops[i].x());
        rightPart.push_back(uvTops[i].x());
        matrix.push_back(row);
        /*equation with parametr v*/
        row = QVector<qreal>();
        row.push_back(0.0);
        row.push_back(0.0);
        row.push_back(0.0);
        row.push_back(xyzTops[i].x());
        row.push_back(xyzTops[i].y());
        row.push_back(1.0);
        row.push_back(-xyzTops[i].x() * uvTops[i].y());
        row.push_back(-xyzTops[i].y() * uvTops[i].y());
        rightPart.push_back(uvTops[i].y());
        matrix.push_back(row);
    }
    diagonalized();
    reversePass();
}

/*Movs to the top of natrix all columns with non-zero elements with intdex = number*/
void ConverterUV::sortColumn(int number)
{
    QVector<QVector<qreal> > sortedMatix = QVector<QVector<qreal> >();
    QVector<qreal> newRightPart = QVector<qreal>();
    if(number < matrix.size())
    {
        for(int i = 0; i < matrix.size(); i++)
        {
            if(i >= number)
            {
                if(matrix[i][number] == 0)
                {
                    sortedMatix.push_back(matrix[i]);
                    newRightPart.push_back(rightPart[i]);
                }
                else
                {
                    sortedMatix.push_front(matrix[i]);
                    newRightPart.push_front(rightPart[i]);
                }
            }
        }
        for(int i = number - 1; i >= 0; i--)
        {
            sortedMatix.push_front(matrix[i]);
            newRightPart.push_front(rightPart[i]);
        }
        matrix = sortedMatix;
        rightPart = newRightPart;
    }
}

/*Make up-triangle matrix*/
void ConverterUV::diagonalized()
{
    for(int i = 0; i < matrix.size(); i++)
    {
        sortColumn(i);
        if(matrix[i][i] != 0)
        {
            qreal norm = matrix[i][i];
            for(int k = i; k < matrix.size(); k++)
            {
                matrix[i][k] /= norm;
            }
            rightPart[i] /= norm;
            for(int j = i + 1; j < matrix.size(); j++)
            {
                if(matrix[j][i] != 0)
                {
                    qreal coeff = matrix[j][i];
                    for(int k = i; k < matrix.size(); k++)
                    {
                        matrix[j][k] -= (coeff * matrix[i][k]);
                    }
                    rightPart[j] -= coeff * rightPart[i];
                }
            }
        }

    }
}

/*Make reverse passe throught the matrix to count
result vector (a1, a2, a3, b1, b2, b3, d1, d2) (d3 = 1)*/
void ConverterUV::reversePass()
{
    uvConvertion = rightPart;
    for(int i = matrix.size() - 1; i >= 0; i--)
    {
        for(int j = matrix.size() - 1; j > i; j--)
        {
            uvConvertion[i] -= matrix[i][j] * uvConvertion[j];
        }
    }
}

/*Count texture coordinates of pixel center*/
QPointF ConverterUV::getTextureCoordinates(QPointF imagePoint)
{
    qreal x = imagePoint.x();
    qreal y = imagePoint.y();
    qreal u = (uvConvertion[0] * (x + 0.5) + uvConvertion[1] * (y + 0.5) + uvConvertion[2]) /
              (uvConvertion[6] * (x + 0.5) + uvConvertion[7] * (y + 0.5) + 1.0);
    qreal v = (uvConvertion[3] * (x + 0.5) + uvConvertion[4] * (y + 0.5) + uvConvertion[5]) /
              (uvConvertion[6] * (x + 0.5) + uvConvertion[7] * (y + 0.5) + 1.0);
    return QPointF(u, v);
}

/*Count texture coordinates rectangle in which the imagePoint transwer to*/
QVector<QPointF> ConverterUV::getTextureRect(QPointF imagePoint)
{
    QVector<QPointF> textureRect = QVector<QPointF>();
    for(qreal i = -0.5; i <= 0.5; i++)
    {
        for(qreal j = -0.5; j <= 0.5; j++)
        {
           textureRect.push_back(getTextureCoordinates(imagePoint + QPointF(i, j)));
        }
    }
    return textureRect;
}
