#include "reswidget.h"
#include "ui_reswidget.h"

#include <QPixmap>
#include <QGraphicsPixmapItem>

ResWidget::ResWidget(QWidget *parent, QString *origImagePath, QString *comprImagePath, bool PSNR, bool SSIM, bool MSE, bool compr) :
    QWidget(parent),
    ui(new Ui::ResWidget)
{
    ui->setupUi(this);

    ui->tableWidget->setRowCount(10);
    ui->tableWidget->setColumnCount(2);

    QStringList headers;
    headers << "Показатель" << "Значение";
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    ui->tableWidget->setItem(0,0, new QTableWidgetItem("Путь исх. изобр."));
    ui->tableWidget->setItem(0,1, new QTableWidgetItem(origImagePath->toStdString().c_str()));

    ui->tableWidget->setItem(1,0, new QTableWidgetItem("Путь сжат. изобр."));
    ui->tableWidget->setItem(1,1, new QTableWidgetItem(comprImagePath->toStdString().c_str()));

    ui->tableWidget->setItem(2,0, new QTableWidgetItem("Размер исх. изобр. (кБ)"));
    ui->tableWidget->setItem(2,1, new QTableWidgetItem(QString::number(9.1)));

    ui->tableWidget->setItem(3,0, new QTableWidgetItem("Размер сжат. изобр. (кБ)"));
    ui->tableWidget->setItem(3,1, new QTableWidgetItem(QString::number(49.1)));

    ui->tableWidget->setItem(4,0, new QTableWidgetItem("H&W исх. изобр."));
    ui->tableWidget->setItem(4,1, new QTableWidgetItem("255x255"));

    ui->tableWidget->setItem(5,0, new QTableWidgetItem("H&W сжат. изобр."));
    ui->tableWidget->setItem(5,1, new QTableWidgetItem("255x255"));

    ui->tableWidget->setItem(6,0, new QTableWidgetItem("PSNR"));
    if(PSNR)
        ui->tableWidget->setItem(6,1, new QTableWidgetItem(QString::number(62.27)));
    else
        ui->tableWidget->setItem(6,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(7,0, new QTableWidgetItem("SSIM"));
    if(SSIM)
        ui->tableWidget->setItem(7,1, new QTableWidgetItem(QString::number(1)));
    else
        ui->tableWidget->setItem(7,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(8,0, new QTableWidgetItem("MSE"));
    if(MSE)
        ui->tableWidget->setItem(8,1, new QTableWidgetItem(QString::number(0.039)));
    else
        ui->tableWidget->setItem(8,1, new QTableWidgetItem("-"));

    ui->tableWidget->setItem(9,0, new QTableWidgetItem("Степень сжатия"));
    if(compr)
        ui->tableWidget->setItem(9,1, new QTableWidgetItem(QString::number(0.185336049)));
    else
        ui->tableWidget->setItem(9,1, new QTableWidgetItem("-"));

    QPixmap pixmapOrig(*origImagePath);
    QPixmap pixmapCompr(*comprImagePath);

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

    //connect(ui->graphicsView, SIGNAL(wheelEvent(QWheelEvent *event)), this, SLOT(scaleViewOrig(QWheelEvent *event)));
    //connect(ui->graphicsView_2, SIGNAL(wheelEvent(QWheelEvent *event)), this, SLOT(scaleViewCompr(QWheelEvent *event)));

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
