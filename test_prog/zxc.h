#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileSystemModel>
#include <QListView>

//#include <QSettings>
//#include <QSplitter>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void filling_file_list(const QString &path);
    void filling_image_list(const QString &path);

private slots:
    void on_file_list_panel_doubleClicked(const QModelIndex &index);
    void on_image_list_panel_doubleClicked(const QModelIndex &index);

private:
    Ui::MainWindow *ui;

    QFileSystemModel *fileSystemModel;
    QFileSystemModel *imageSystemModel;

    QByteArray hexData;

    QString *pathProject;
    //QSplitter *splitter;

    //void save_splitter_size();
    //void load_splitter_size();
};
#endif // MAINWINDOW_H

