#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "addnumdialog.h"
#include "chartdialog.h"
#include "utils.h"
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
    socket->connectToHost(QHostAddress::LocalHost, 8010);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::slotReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::slotErrorOccurred);
    connect(socket, &QTcpSocket::connected, this, &MainWindow::slotConnected);

    concentrationModel = new DoubleListModel;
    concentrationVector.clear();
    levelModel = new DoubleListModel;
    levelVector.clear();

    ui->setupUi(this);

    ui->concentrationListView->setModel(concentrationModel);
    ui->concentrationListView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->levelListView->setModel(levelModel);
    ui->levelListView->setSelectionMode(QAbstractItemView::ExtendedSelection);

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

    if (!selectedConcentration.empty()) {
        std::sort(selectedConcentration.begin(), selectedConcentration.end(), [](const QModelIndex &a, const QModelIndex &b) {
            return a.row() > b.row(); // Сортировка по убыванию
        });

        for (const QModelIndex &idx : selectedConcentration) {
            concentrationVector.removeAt(idx.row());
        }
        concentrationModel->setValues(concentrationVector);
    }

    if (!selectedLevel.empty()) {
        std::sort(selectedLevel.begin(), selectedLevel.end(), [](const QModelIndex &a, const QModelIndex &b) {
            return a.row() > b.row(); // Сортировка по убыванию
        });

        for (const QModelIndex &idx : selectedLevel) {
            levelVector.removeAt(idx.row());
        }
        levelModel->setValues(levelVector);
    }
}

void MainWindow::on_addBtn_clicked()
{
    addNumDialog *w = new addNumDialog;
    parameter param;
    double value;

    if (w->exec() == QDialog::Accepted) {
        param = w->getParameterValue();
        value = w->getNumValue();
        switch(param) {
        case CONCENTRATION:
            concentrationVector.push_back(value);
            concentrationModel->setValues(concentrationVector);
            break;
        case LEVEL:
            levelVector.push_back(value);
            levelModel->setValues(levelVector);
            break;
        }
    }
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
        levelModel->setValues(levelVector);
        concentrationModel->setValues(concentrationVector);
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

        QTextStream in(&file);

        int sizeConc, sizeLevel;
        sizeConc = in.readLine().toInt();
        for (int i = 0; i < sizeConc; i++) {
            concentrationVector.push_back(in.readLine().toDouble());
        }
        sizeLevel = in.readLine().toInt();
        for (int i = 0; i < sizeLevel; i++) {
            levelVector.push_back(in.readLine().toDouble());
        }

        file.close();
    }

    levelModel->setValues(levelVector);
    concentrationModel->setValues(concentrationVector);
}


void MainWindow::on_connectToServer_triggered()
{
    if (socket->state() != QTcpSocket::ConnectedState) {
        socket->connectToHost(QHostAddress::LocalHost, 8010);
    }
    else {
        QMessageBox::information(this, "Информация о подключении", "Подключение уже было произведено");
        return;
    }
}

void MainWindow::runChartWindow() {
    chartDialog *w = new chartDialog(concentrationVector,
                                     levelVector,
                                     coefficientsConcentration,
                                     coefficientsLevel);

    w->exec();
}

void MainWindow::slotConnected() {
    ui->statusLabel->setText("Сервер: подключено");
}
