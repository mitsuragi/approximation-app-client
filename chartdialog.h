#ifndef CHARTDIALOG_H
#define CHARTDIALOG_H

#include <QDialog>
#include <QtCharts>

namespace Ui {
class chartDialog;
}

class chartDialog : public QDialog
{
    Q_OBJECT

public:
    chartDialog(const QVector<double>& _concentrationVector,
                const QVector<double>& _levelVector,
                const QVector<double>& _coefficientsConcentration,
                const QVector<double>& _coefficientsLevel,
                QWidget *parent = nullptr);
    ~chartDialog();
private:
    Ui::chartDialog *ui;

    QVector<double> concentrationVector;
    QVector<double> levelVector;
    QVector<double> coefficientsConcentration;
    QVector<double> coefficientsLevel;

    QChart* initChart();
};

#endif // CHARTDIALOG_H
