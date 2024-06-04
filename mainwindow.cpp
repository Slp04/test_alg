#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QSplitter>
#include <QTextStream>
#include <QFileDialog>
#include <QProcess>

#include "startwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setNameFilters(QStringList() << "*.cpp" << "*.h" << "*.txt");
    fileSystemModel->setNameFilterDisables(false);

    imageSystemModel = new QFileSystemModel(this);
    imageSystemModel->setRootPath(QDir::currentPath());
    imageSystemModel->setNameFilters(QStringList() << "*.png" << "*.jpg" << "*.jpeg");
    imageSystemModel->setNameFilterDisables(false);

    ui->file_list_panel->setModel(fileSystemModel);
    ui->file_list_panel->setRootIndex(fileSystemModel->index(QDir::currentPath()));

    ui->image_list_panel->setModel(imageSystemModel);
    ui->image_list_panel->setRootIndex(imageSystemModel->index(QDir::currentPath()));

    connect(ui->file_list_panel, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_file_list_panel_doubleClicked(const QModelIndex&)));
    connect(ui->image_list_panel, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(on_image_list_panel_doubleClicked(const QModelIndex&)));

    connect(ui->action_new_file, SIGNAL(triggered()), this, SLOT(create_new_file()));
    connect(ui->action_save_file, SIGNAL(triggered()), this, SLOT(save_file()));
    connect(ui->action_open_image, SIGNAL(triggered()), this, SLOT(open_image()));

    connect(ui->action_start_prog, SIGNAL(triggered()), this, SLOT(start_prog()));

    ui->file_list_panel->setFixedWidth(200);
    ui->image_list_panel->setFixedWidth(200);

}

void MainWindow::receivePath(const QString &path)
{
    pathProject = path;
    filling_file_list(pathProject);
    filling_image_list(pathProject);
}

void MainWindow::filling_file_list(const QString path)
{
    ui->file_list_panel->setRootIndex(fileSystemModel->index(path));
}

void MainWindow::filling_image_list(const QString path)
{
    ui->image_list_panel->setRootIndex(imageSystemModel->index(path));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_file_list_panel_doubleClicked(const QModelIndex &index)
{
    QString path = fileSystemModel->filePath(index);

    if (QFileInfo(path).isFile())
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            QString content = in.readAll();
            ui->text_editor->setPlainText(content);
            file.close();
        }
        else
        {
            QMessageBox::warning(this, tr("Ошибка"), tr("Невозможно открыть файл"));
        }
    }
}

void MainWindow::on_image_list_panel_doubleClicked(const QModelIndex &index)
{
    QString path = imageSystemModel->filePath(index);

    if (QFileInfo(path).isFile())
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&file);
            //QString content = in.readAll();
            hexData = QByteArray::fromHex((in.readAll()).toUtf8());

            hexEditor = new HexEditor(this);
            hexEditor->set_data(hexData, ui->hex_editor->width());

            QSplitter *splitter = ui->splitter;
            QLayout *layout = splitter->widget(1)->layout();
            QHBoxLayout *h_box_layout = qobject_cast<QHBoxLayout*>(layout);

            if(h_box_layout)
            {
                QWidget *old_hex_editor = ui->hex_editor;
                h_box_layout->removeWidget(old_hex_editor);
                delete old_hex_editor;

                ui->hex_editor = hexEditor;
                h_box_layout->addWidget(ui->hex_editor);
            }

            file.close();
        }
        else
        {
            QMessageBox::warning(this, tr("Ошибка"), tr("Невозможно открыть файл"));
        }
    }
}


void MainWindow::create_new_file()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Создание нового файла"), pathProject.toStdString().c_str(), tr("Text Files (*.txt); Source Files (*.cpp); Header Files (*.h);All Files (*)"));

    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);
            out << "";
            file.close();
            filling_file_list(pathProject);
            QMessageBox::information(this, tr("Создание нового файла"), tr("Файл успешно создан."));
        }
        else
        {
            QMessageBox::warning(this, tr("Ошибка"), tr("Создать новый файл не удалось."));
        }
    }
}

void MainWindow::open_image()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr(pathProject.toStdString().c_str()), "", tr("PNG (*.png);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            //QTextStream in(&file);
            //ui->text_editor->setPlainText(in.readAll());
            file.close();
        }
        else
        {
            QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось открыть изображение."));
        }
    }
}

void MainWindow::save_file()
{
    QFile file(pathProject + ui->file_list_panel->currentIndex().data(Qt::DisplayRole).toString());
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {

        QTextStream out(&file);
        out << ui->text_editor->toPlainText();
        QMessageBox::information(this, tr("Сохранение файла"), tr("Файл успешно сохранён."));
        file.close();
    }
    else
    {
        QMessageBox::warning(this, tr("Ошибка"), tr("Не удалось сохранить файл."));
    }
}

void MainWindow::on_action_start_prog_triggered()
{
    QProcess cmakeProcess;
    cmakeProcess.setWorkingDirectory(pathProject);
    cmakeProcess.start("cmake", QStringList() << ".");
    if(!cmakeProcess.waitForFinished())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить настройку CMake");
        return;
    }

    QProcess makeProcess;
    makeProcess.setWorkingDirectory(pathProject);
    makeProcess.start("make");
    if(!makeProcess.waitForFinished())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось собрать проект");
        return;
    }

    QMessageBox::information(this, "Успешно", "Сборка успешно завершена!");
}

