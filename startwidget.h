#ifndef STARTWIDGET_H
#define STARTWIDGET_H

#include <QWidget>

class MainWindow;

QT_BEGIN_NAMESPACE
namespace Ui { class StartWidget; }
QT_END_NAMESPACE

class StartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartWidget(MainWindow *mainWindow, QWidget *parent = nullptr);
    ~StartWidget();

private slots:
    void on_pushButton_clicked();

private:
    Ui::StartWidget *ui;
    MainWindow *mainWindow;
};

#endif // STARTWIDGET_H
