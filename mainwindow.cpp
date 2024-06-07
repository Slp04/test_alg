#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QRegularExpression>
#include <chrono>
#include <QGraphicsPixmapItem>

#include "startwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , cmakeProcess(new QProcess(this))
    , makeProcess(new QProcess(this))
{
    ui->setupUi(this);

    fileSystemModel = new QFileSystemModel(this);
    fileSystemModel->setRootPath(QDir::currentPath());
    fileSystemModel->setFilter(QDir::Files | QDir::NoDotAndDotDot);
    fileSystemModel->setNameFilters(QStringList() << "*.cpp" << "*.h" << "*.txt");
    fileSystemModel->setNameFilterDisables(false);

    imageSystemModel = new QFileSystemModel(this);
    imageSystemModel->setRootPath(QDir::currentPath());
    imageSystemModel->setFilter( QDir::Files | QDir::NoDotAndDotDot);
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

    connect(cmakeProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readCmakeOutput()));
    connect(cmakeProcess, SIGNAL(readyReadStandardError()), this, SLOT(readCmakeError()));
    connect(cmakeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::handleCmakeFinished);
    connect(makeProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(readMakeOutput()));
    connect(makeProcess, SIGNAL(readyReadStandardError()), this, SLOT(readMakeError()));
    connect(makeProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &MainWindow::handleMakeFinished);


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
    ui->textEdit_console->clear();

    origImageName = ui->lineEdit_orig_name->text();
    if(origImageName.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Не указано название исходного изображения");
        return;
    }

    comprImageName = ui->lineEdit_compr_name->text();
    if(comprImageName.isEmpty())
    {
        QMessageBox::critical(this, "Ошибка", "Не указано название сжатого изображения");
        return;
    }

    if(ui->checkBox_data->isChecked())
    {
        m_width = ui->lineEdit_width->text().toInt();
        m_height = ui->lineEdit_height->text().toInt();

        if(!m_width or !m_height)
        {
            QMessageBox::critical(this, "Ошибка", "Не указаны размеры ихображения!");
            return;
        }


        ui->comboBox_format->currentText().toInt();
        if(ui->comboBox_format->currentText().toLatin1().data() == "RGB")
        {
            m_format = QImage::Format_RGB888;
        }

        QString tmp = pathProject+origImageName;
        origImage = loadRawImage(&tmp);
        if(origImage.isNull())
        {
            QMessageBox::critical(this, "Ошибка", "Не удалось загрузить сырые данные!");
            return;
        }

        QGraphicsPixmapItem *item = new QGraphicsPixmapItem(QPixmap::fromImage(origImage));
        QGraphicsScene *scene = new QGraphicsScene;
        scene->addItem(item);
        ui->graphicsView->setScene(scene);
        ui->graphicsView->setRenderHint(QPainter::Antialiasing);
        ui->graphicsView->setRenderHint(QPainter::SmoothPixmapTransform);

    }

    cmakeProcess->setWorkingDirectory(pathProject);
    cmakeProcess->start("cmake", QStringList() << ".");
    if(!cmakeProcess->waitForStarted())
    {
        QMessageBox::critical(this, "Ошибка", "Не удалось выполнить настройку CMake");
        return;
    }
}

void MainWindow::readCmakeOutput()
{
    ui->textEdit_console->append(cmakeProcess->readAllStandardOutput());
}

void MainWindow::readCmakeError()
{
    ui->textEdit_console->append(cmakeProcess->readAllStandardError());
}

void MainWindow::readMakeOutput()
{
    ui->textEdit_console->append(makeProcess->readAllStandardOutput());
}

void MainWindow::readMakeError()
{
    ui->textEdit_console->append(makeProcess->readAllStandardOutput());
}

void MainWindow::handleCmakeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        return;
    }
    if(exitCode != 0)
    {
        return;
    }

    makeProcess->setWorkingDirectory(pathProject);
    makeProcess->start("make");
    if(!makeProcess->waitForStarted())
    {
        return;
    }
}
void MainWindow::handleMakeFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if(exitStatus == QProcess::CrashExit)
    {
        return;
    }
    if(exitCode != 0)
    {
        return;
    }

    getProjectName();
    if(nameProject.isEmpty())
    {
        ui->textEdit_console->insertPlainText("Ошибка сборки проекта: не удалось найти имя проекта в CMakeLists.txt");
        return;
    }

    auto start = std::chrono::high_resolution_clock::now();
    runCode();
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> time = finish - start;
    //ui->textEdit_result->clear();
    //ui->textEdit_result->insertPlainText("Название программы: " + nameProject + '\n');
    //ui->textEdit_result->insertPlainText("Время работы программы: " + QString::number(time.count()) + "сек." + '\n');
}

void MainWindow::getProjectName()
{
    QFile file(pathProject + "CMakeLists.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return;
    }

    QTextStream in(&file);
    QRegularExpression re("^\\s*project\\s*\\(\\s*([^\\s\\)]+)");

    while(!in.atEnd())
    {
        QString line = in.readLine();
        QRegularExpressionMatch match = re.match(line);
        if(match.hasMatch())
        {
            nameProject = match.captured(1);
            break;
        }
    }
    file.close();
}

void MainWindow::runCode()
{
    QProcess startProg;
    startProg.start("./" + nameProject);
    startProg.waitForFinished();
}

void MainWindow::on_checkBox_data_stateChanged(int check)
{
    if(check)
    {
        ui->lineEdit_width->setEnabled(true);
        ui->lineEdit_height->setEnabled(true);
        ui->comboBox_format->setEnabled(true);
    }
    else
    {
        ui->lineEdit_width->setEnabled(false);
        ui->lineEdit_height->setEnabled(false);
        ui->comboBox_format->setEnabled(false);
    }
}

QImage MainWindow::loadRawImage(QString *filePath)
{
    QFile file(*filePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(this, "Ошибка", "Ошибка открытия файла для чтения");
        return QImage();
    }

    QByteArray ba = file.readAll();
    file.close();

    QImage image(reinterpret_cast<const uchar*>(ba.data()), m_width, m_height, m_format);
    return image;
}
