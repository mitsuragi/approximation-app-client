#ifndef ADDNUMDIALOG_H
#define ADDNUMDIALOG_H

#include <QDialog>
#include <string>
#include "utils.h"

namespace Ui {
class addNumDialog;
}

class addNumDialog : public QDialog
{
    Q_OBJECT

public:
    explicit addNumDialog(QWidget *parent = nullptr);
    ~addNumDialog();

    parameter getParameterValue();
    double getNumValue();

private:
    Ui::addNumDialog *ui;
};

#endif // ADDNUMDIALOG_H
