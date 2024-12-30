#ifndef ADDNUMDIALOG_H
#define ADDNUMDIALOG_H

#include <QDialog>
#include <string>

namespace Ui {
class addNumDialog;
}

class addNumDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addNumDialog(QWidget *parent = nullptr);
    ~addNumDialog();

    double getConsumptionValue();
    double getConcentrationValue();
    double getLevelValue();

private:
    Ui::addNumDialog *ui;
};

#endif // ADDNUMDIALOG_H
