#include "reswidget.h"
#include "ui_reswidget.h"

#include <QPixmap>
#include <QGraphicsPixmapItem>

#include <QImage>
#include <QColor>
#include <QByteArray>
#include <cmath>
#include <limits>
#include <QFileInfo>

ResWidget::ResWidget(QWidget *parent, const QString &origImagePath, const QString &comprImagePath, bool PSNR, bool SSIM, bool MSE, bool compr) :
    QWidget(parent),
    ui(new Ui::ResWidget)
{
    ui->setupUi(this);

    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);

    QStringList headers;
    headers << "Показатель" << "Значение";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    QPixmap pixmapOrig(origImagePath);
    QPixmap pixmapCompr(comprImagePath);

    sceneOrig = new QGraphicsScene();
    sceneCompr = new QGraphicsScene();

    sceneOrig->addPixmap(pixmapOrig);
    sceneCompr->addPixmap(pixmapCompr);

    ui->graphicsView->setScene(sceneOrig);
    ui->graphicsView_2->setScene(sceneCompr);

    ui->graphicsView->setDragMode(QGraphicsView::ScrollHandDrag);
    ui->graphicsView_2->setDragMode(QGraphicsView::ScrollHandDrag);

    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView_2->setRenderHint(QPainter::Antialiasing);

    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView_2->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView_2->setResizeAnchor(QGraphicsView::AnchorUnderMouse);

    QImage img1 = pixmapOrig.toImage();
    QImage img2 = pixmapCompr.toImage();

    ui->tableWidget->setItem(0,0, new QTableWidgetItem("Путь исх. изобр."));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(origImagePath.toStdString().c_str()));

    ui->tableWidget->setItem(1,0, new QTableWidgetItem("Путь сжат. изобр."));
    ui->tableWidget->setItem(1,1, new QTableWidgetItem(comprImagePath.toStdString().c_str()));

    ui->tableWidget->setItem(2,0, new QTableWidgetItem("Размер исх. изобр. (кБ)"));
    ui->tableWidget->setItem(2,1, new QTableWidgetItem(QString::number(getImageSize(origImagePath) / 1024)));

    ui->tableWidget->setItem(3,0, new QTableWidgetItem("Размер сжат. изобр. (кБ)"));
    ui->tableWidget->setItem(3,1, new QTableWidgetItem(QString::number(getImageSize(comprImagePath) / 1024)));

    ui->tableWidget->setItem(4,0, new QTableWidgetItem("H&W исх. изобр."));
    ui->tableWidget->setItem(4,1, new QTableWidgetItem(QString::number(getImageWidth(pixmapOrig))+"x"+QString::number(getImageHeight(pixmapOrig))));

    ui->tableWidget->setItem(5,0, new QTableWidgetItem("H&W сжат. изобр."));
    ui->tableWidget->setItem(5,1, new QTableWidgetItem(QString::number(getImageWidth(pixmapCompr))+"x"+QString::number(getImageHeight(pixmapCompr))));

    ui->tableWidget->setItem(6,0, new QTableWidgetItem("PSNR"));
    if(PSNR)
        ui->tableWidget->setItem(6,1, new QTableWidgetItem(QString::number(calculatePSNR(img1, img2))));
    else
        ui->tableWidget->setItem(6,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(7,0, new QTableWidgetItem("SSIM"));
    if(SSIM)
        ui->tableWidget->setItem(7,1, new QTableWidgetItem(QString::number(calculateSSIM(img1, img2))));
    else
        ui->tableWidget->setItem(7,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(8,0, new QTableWidgetItem("MSE"));
    if(MSE)
        ui->tableWidget->setItem(8,1, new QTableWidgetItem(QString::number(calculateMSE(img1, img2))));
    else
        ui->tableWidget->setItem(8,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(9,0, new QTableWidgetItem("Степень сжатия"));
    if(compr)
        ui->tableWidget->setItem(9,1, new QTableWidgetItem(QString::number(calculateCompressionRatio(origImagePath, comprImagePath))));
    else
        ui->tableWidget->setItem(9,1, new QTableWidgetItem("-"));

}

ResWidget::ResWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ResWidget)
{
    ui->setupUi(this);
}

ResWidget::~ResWidget()
{
    delete ui;
}

void ResWidget::scaleViewOrig(QWheelEvent *event)
{
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        ui->graphicsView->scale(scaleFactor, scaleFactor);
    } else {
        ui->graphicsView->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

void ResWidget::scaleViewCompr(QWheelEvent *event)
{
    double scaleFactor = 1.15;
    if (event->angleDelta().y() > 0) {
        ui->graphicsView_2->scale(scaleFactor, scaleFactor);
    } else {
        ui->graphicsView_2->scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}

double ResWidget::calculateMSE(const QImage& img1, const QImage& img2)
{
    if (img1.size() != img2.size())
        return -1; // размеры изображений должны быть одинаковы

    double mse = 0.0;
    int width = img1.width();
    int height = img1.height();

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            QColor color1 = img1.pixelColor(x, y);
            QColor color2 = img2.pixelColor(x, y);

            double diffR = color1.red() - color2.red();
            double diffG = color1.green() - color2.green();
            double diffB = color1.blue() - color2.blue();

            mse += diffR * diffR + diffG * diffG + diffB * diffB;
        }
    }
    mse /= (width * height * 3); // делим на количество пикселей и 3 (R, G, B)
    return mse;
}

double ResWidget::calculatePSNR(const QImage& img1, const QImage& img2)
{
    double mse = calculateMSE(img1, img2);
    if (mse == 0) return std::numeric_limits<double>::infinity(); // идеальное совпадение

    double psnr = 10.0 * log10((255 * 255) / mse);
    return psnr;
}

double ResWidget::calculateSSIM(const QImage& img1, const QImage& img2)
{
    if (img1.size() != img2.size()) return -1; // размеры изображений должны быть одинаковы

    const double C1 = 6.5025, C2 = 58.5225;
    int width = img1.width();
    int height = img1.height();

    double mean1 = 0.0, mean2 = 0.0;
    double variance1 = 0.0, variance2 = 0.0;
    double covariance = 0.0;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            QColor color1 = img1.pixelColor(x, y);
            QColor color2 = img2.pixelColor(x, y);

            double val1 = 0.299 * color1.red() + 0.587 * color1.green() + 0.114 * color1.blue();
            double val2 = 0.299 * color2.red() + 0.587 * color2.green() + 0.114 * color2.blue();

            mean1 += val1;
            mean2 += val2;
        }
    }
    mean1 /= (width * height);
    mean2 /= (width * height);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            QColor color1 = img1.pixelColor(x, y);
            QColor color2 = img2.pixelColor(x, y);

            double val1 = 0.299 * color1.red() + 0.587 * color1.green() + 0.114 * color1.blue();
            double val2 = 0.299 * color2.red() + 0.587 * color2.green() + 0.114 * color2.blue();

            variance1 += (val1 - mean1) * (val1 - mean1);
            variance2 += (val2 - mean2) * (val2 - mean2);
            covariance += (val1 - mean1) * (val2 - mean2);
        }
    }
    variance1 /= (width * height - 1);
    variance2 /= (width * height - 1);
    covariance /= (width * height - 1);

    double ssim = ((2 * mean1 * mean2 + C1) * (2 * covariance + C2)) / ((mean1 * mean1 + mean2 * mean2 + C1) * (variance1 + variance2 + C2));
    return ssim;
}

double ResWidget::calculateCompressionRatio(const QString& origImagePath, const QString& comprImagePath)
{
    QFileInfo orig(origImagePath);
    QFileInfo compr(comprImagePath);

    if (!orig.size() || !compr.size())
        return -1; // данные не должны быть пустыми

    double ratio = static_cast<double>(orig.size()) / compr.size();
    return ratio;
}

qint64 ResWidget::getImageSize(const QString& filePath)
{
    QFileInfo file(filePath);
    return file.size();
}

int ResWidget::getImageWidth(const QPixmap& pixmap)
{
    return pixmap.width();
}

int ResWidget::getImageHeight(const QPixmap& pixmap)
{
    return pixmap.height();
}

