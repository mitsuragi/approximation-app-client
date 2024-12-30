#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtNetwork>
#include "doublelistmodel.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void slotReadyRead();
    void slotErrorOccurred(QAbstractSocket::SocketError socketError);
    void slotConnected();
    void runChartWindow();

private slots:
    void on_plotBtn_clicked();

    void on_removeBtn_clicked();

    void on_addBtn_clicked();

    void on_removeAllBtn_clicked();

    void on_saveData_triggered();

    void on_loadData_triggered();

    void on_connectToServer_triggered();

    void on_devInfo_triggered();

    void on_programInfo_triggered();

signals:
    void dataReceived();

private:
    DoubleListModel *concentrationModel;
    DoubleListModel *levelModel;
    DoubleListModel *consumptionModel;

    QVector<double> concentrationVector;
    QVector<double> levelVector;
    QVector<double> consumptionVector;

    QVector<double> coefficientsConcentration;
    QVector<double> coefficientsLevel;

    QTcpSocket *socket;
    QByteArray Data;

    void connectSocket();
    void sendDataToServer();
    void refreshModels();

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
