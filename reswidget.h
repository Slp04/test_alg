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
    explicit ResWidget(QWidget *parent, QString *origImagePath, QString *comprImagePath, bool PSNR, bool SSIM, bool MSE, bool compr);
    explicit ResWidget(QWidget *parent = nullptr);
    ~ResWidget();

private slots:
    void scaleViewOrig(QWheelEvent *event);
    void scaleViewCompr(QWheelEvent *event);

private:
    Ui::ResWidget *ui;

    QGraphicsScene *sceneOrig;
    QGraphicsScene *sceneCompr;
};

#endif // RESWIDGET_H
