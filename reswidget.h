#ifndef RESWIDGET_H
#define RESWIDGET_H

#include <QWidget>
#include <QGraphicsScene>
#include <QWheelEvent>

namespace Ui {
class ResWidget;
}

class ResWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ResWidget(QWidget *parent, const QString &origImagePath, const QString &comprImagePath, bool PSNR, bool SSIM, bool MSE, bool compr);
    explicit ResWidget(QWidget *parent = nullptr);
    ~ResWidget();

private slots:
    void scaleViewOrig(QWheelEvent *event);
    void scaleViewCompr(QWheelEvent *event);

private:
    Ui::ResWidget *ui;

    QGraphicsScene *sceneOrig;
    QGraphicsScene *sceneCompr;

    double calculateMSE(const QImage& img1, const QImage& img2);
    double calculatePSNR(const QImage& img1, const QImage& img2);
    double calculateSSIM(const QImage& img1, const QImage& img2);
    double calculateCompressionRatio(const QString& originalData, const QString& compressedData);
    qint64 getImageSize(const QString& path);
    int getImageWidth(const QPixmap& pixmap);
    int getImageHeight(const QPixmap& pixmap);

};

#endif // RESWIDGET_H
