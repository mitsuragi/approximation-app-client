#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addnumdialog.h"
#include "chartdialog.h"
#include <QModelIndexList>
#include <QMessageBox>
#include <QString>
#include <QFileDialog>
#include <QSizePolicy>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    socket = new QTcpSocket(this);
    connectSocket();
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::slotErrorOccurred);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::slotConnected);

    concentrationModel = new DoubleListModel;
    concentrationVector.clear();
    levelModel = new DoubleListModel;
    levelVector.clear();
    consumptionModel = new DoubleListModel;
    consumptionVector.clear();

    ui->setupUi(this);

    ui->concentrationListView->setModel(concentrationModel);
    ui->concentrationListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->levelListView->setModel(levelModel);
    ui->levelListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->consumptionListView->setModel(consumptionModel);
    ui->consumptionListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(this, &MainWindow::dataReceived, this, &MainWindow::runChartWindow);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_plotBtn_clicked()
{
    sendDataToServer();
}


void MainWindow::on_removeBtn_clicked()
{
    QModelIndexList selectedConcentration = ui->concentrationListView->selectionModel()->selectedIndexes();
    QModelIndexList selectedLevel = ui->levelListView->selectionModel()->selectedIndexes();
    QModelIndexList selectedConsumption = ui->consumptionListView->selectionModel()->selectedIndexes();

    if (!selectedConcentration.empty() || !selectedLevel.empty() || !selectedConsumption.empty()) {
        std::sort(selectedConcentration.begin(), selectedConcentration.end(), [](const QModelIndex &a, const QModelIndex &b) {
            return a.row() > b.row(); // Сортировка по убыванию
        });

        std::sort(selectedLevel.begin(), selectedLevel.end(), [](const QModelIndex &a, const QModelIndex &b) {
            return a.row() > b.row(); // Сортировка по убыванию
        });

        std::sort(selectedConsumption.begin(), selectedConsumption.end(), [](const QModelIndex &a, const QModelIndex &b) {
            return a.row() > b.row(); // Сортировка по убыванию
        });

        for (const QModelIndex &idx : selectedConcentration) {
            concentrationVector.removeAt(idx.row());
            levelVector.removeAt(idx.row());
            consumptionVector.removeAt(idx.row());
        }
        for (const QModelIndex &idx : selectedLevel) {
            concentrationVector.removeAt(idx.row());
            levelVector.removeAt(idx.row());
            consumptionVector.removeAt(idx.row());
        }
        for (const QModelIndex &idx : selectedConsumption) {
            concentrationVector.removeAt(idx.row());
            levelVector.removeAt(idx.row());
            consumptionVector.removeAt(idx.row());
        }

        refreshModels();
    }
}

void MainWindow::on_addBtn_clicked()
{
    addNumDialog *w = new addNumDialog;
    double consum;
    double conc;
    double level;

    if (w->exec() == QDialog::Accepted) {
        consum = w->getConsumptionValue();
        conc = w->getConcentrationValue();
        level = w->getLevelValue();

        if (consum < 0 || conc < 0) {
            QMessageBox::critical(this, "Ошибка", "Значения расхода и концентрации не могут быть меньше 0");
            return;
        }

        consumptionVector.push_back(consum);
        concentrationVector.push_back(conc);
        levelVector.push_back(level);
    }

    refreshModels();
}


void MainWindow::on_removeAllBtn_clicked()
{
    QMessageBox *msg = new QMessageBox;
    msg->setIcon(QMessageBox::Question);
    msg->setInformativeText("Вы уверены?");
    msg->setText("Действие нельзя будет отменить.");
    QPushButton *yesBtn = msg->addButton("Да", QMessageBox::AcceptRole);
    msg->addButton("Отмена", QMessageBox::RejectRole);

    msg->exec();
    if (msg->clickedButton() == yesBtn) {
        concentrationVector.clear();
        levelVector.clear();
        consumptionVector.clear();

        refreshModels();
    }
}


void MainWindow::on_saveData_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Выберите файл для сохранения");
    if (!filepath.isEmpty()) {
        QFile file(filepath);

        if (!file.open(QIODevice::WriteOnly)) {
            QMessageBox::information(this,"info", file.errorString());
            file.close();
            return;
        }

        QTextStream out(&file);

        out << consumptionVector.size() << '\n';
        for (int i = 0; i < consumptionVector.size(); i++) {
            out << consumptionVector[i] << '\n';
        }

        out << concentrationVector.size() << '\n';
        for (int i = 0; i < concentrationVector.size(); i++) {
            out << concentrationVector[i] << '\n';
        }

        out << levelVector.size() << '\n';
        for (int i = 0; i < levelVector.size(); i++) {
            out << levelVector[i] << '\n';
        }

        file.close();
    }
}

void MainWindow::slotReadyRead() {
    coefficientsConcentration.clear();
    coefficientsLevel.clear();

    QDataStream in(socket);

    if (in.status() == QDataStream::Ok) {
        if (socket->bytesAvailable() < sizeof(int) * 2) {
            QMessageBox *msg = new QMessageBox(QMessageBox::Warning, "Ошибка", "Недостаточно данных");
            msg->show();
            return;
        }

        int sizeConc, sizeLevel;
        in >> sizeConc;
        for (int i = 0; i < sizeConc; i++) {
            double value;
            in >> value;
            coefficientsConcentration.append(value);
        }
        in >> sizeLevel;
        for (int i = 0; i < sizeLevel; i++) {
            double value;
            in >> value;
            coefficientsLevel.append(value);
        }

        emit dataReceived();
    }
}

void MainWindow::slotErrorOccurred(QAbstractSocket::SocketError socketError) {
    QMessageBox::critical(this, "Ошибка", "При подключении возникла ошибка");
    ui->statusLabel->setText("Сервер: не подключено");
}

void MainWindow::sendDataToServer() {
    Data.clear();
    QDataStream out(&Data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_8);

    out << (int)consumptionVector.size();
    for (int i = 0; i < consumptionVector.size(); i++) {
        out << consumptionVector[i];
    }

    out << (int)concentrationVector.size();
    for (int i = 0; i < concentrationVector.size(); i++) {
        out << concentrationVector[i];
    }

    out << (int)levelVector.size();
    for (int i = 0; i < levelVector.size(); i++) {
        out << levelVector[i];
    }

    socket->write(Data);
}

void MainWindow::on_loadData_triggered()
{
    QString filepath = QFileDialog::getOpenFileName(this, "Выберите файл");
    if (!filepath.isEmpty()) {
        QFile file(filepath);
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::critical(this, "Ошибка", file.errorString());
            file.close();
            return;
        }
        concentrationVector.clear();
        levelVector.clear();
        consumptionVector.clear();

        QTextStream in(&file);

        int sizeConc, sizeLevel, sizeConsum;
        sizeConsum = in.readLine().toInt();
        for (int i = 0; i < sizeConsum; i++) {
            double value = in.readLine().toDouble();
            if (value < 0) {
                QMessageBox::critical(this, "Ошибка", "Данные неверны. Расход не может быть меньше 0");
                consumptionVector.clear();
                file.close();
                return;
            }
            consumptionVector.push_back(value);
        }
        sizeConc = in.readLine().toInt();
        for (int i = 0; i < sizeConc; i++) {
            double value = in.readLine().toDouble();
            if (value < 0) {
                QMessageBox::critical(this, "Ошибка", "Данные неверны. Концентрация не может быть меньше 0");
                concentrationVector.clear();
                consumptionVector.clear();
                file.close();
                return;
            }
            concentrationVector.push_back(value);
        }
        sizeLevel = in.readLine().toInt();
        for (int i = 0; i < sizeLevel; i++) {
            levelVector.push_back(in.readLine().toDouble());
        }

        file.close();
    }

    refreshModels();
}


void MainWindow::on_connectToServer_triggered()
{
    if (socket->state() != QTcpSocket::ConnectedState) {
        connectSocket();
    }
    else {
        QMessageBox::information(this, "Информация о подключении", "Подключение уже было произведено");
        return;
    }
}

void MainWindow::runChartWindow() {
    chartDialog *w = new chartDialog(concentrationVector,
                                     levelVector,
                                     consumptionVector,
                                     coefficientsConcentration,
                                     coefficientsLevel);

    w->exec();
}

void MainWindow::slotConnected() {
    ui->statusLabel->setText("Сервер: подключено");
}

void MainWindow::on_devInfo_triggered()
{
    QMessageBox::information(this, "Информация о разработчике", "Программу разработал студент 424 группы СПбГТИ(ТУ)"
                                                                "\nКафедра система автоматизированного проектирования и управления\n"
                                                                "Направление подготовки: 09.03.01, Информатика и вычислительная техника\n"
                                                                "Дисциплина: Вычислительные системы, сети и телекоммуникации\n"
                                                                "Губкин Максим Романович");
}


void MainWindow::on_programInfo_triggered()
{
    QMessageBox::information(this, "Информация о программе", "Проект: Мониторинг технологических параметров процесса смешения жидкостей\n"
                                                             "Технологии: C++, Qt\n\n"
                                                             "Для ввода данных представляется две опции: ручной ввод, с помощью кнопки "
                                                             "\"Ввести данные\" и считывание данных из текстового файла. Для этого в меню "
                                                             "выберите пункт Файл->Загрузить данные, после чего можете выбрать нужный вам "
                                                             "текстовый файл.\n\n"
                                                             "Типы зависимостей:\n"
                                                             "Расход(кг/сек) - Концентрация(Кмоль/м*3)\n"
                                                             "Расход(кг/сек) - Отклонение уровня(мм)\n\n"
                                                             "После ввода данных при нажатии кнопки \"Построить график\" "
                                                             "программа выведет окно с графиком, на котором вы сможете увидеть 2 типа графиков\n"
                                                             "1. Графики по точкам\n"
                                                             "2. Графики аппроксимации");
}

void MainWindow::refreshModels() {
    concentrationModel->setValues(concentrationVector);
    levelModel->setValues(levelVector);
    consumptionModel->setValues(consumptionVector);
}

void MainWindow::connectSocket() {
    QFile file(":/config/config.txt");
    file.open(QIODevice::ReadOnly);
    QTextStream out(&file);

    QHostAddress adress(out.readLine());
    int host = out.readLine().toInt();

    socket->connectToHost(adress, host);
}

