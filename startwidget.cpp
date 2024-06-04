#include "startwidget.h"
#include "ui_startwidget.h"
#include "mainwindow.h"

StartWidget::StartWidget(MainWindow *mainWindow, QWidget *parent) :
    QWidget(parent),
    mainWindow(mainWindow),
    ui(new Ui::StartWidget)
{
    ui->setupUi(this);
    ui->lineEdit->setText("/home/aboba/test_prog/");
}

StartWidget::~StartWidget()
{
    delete ui;
}

void StartWidget::on_pushButton_clicked()
{
    QString path = ui->lineEdit->text();
    if(mainWindow)
    {
        mainWindow->showMaximized();
        mainWindow->receivePath(path);
        this->close();
    }

}

