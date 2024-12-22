#include "addnumdialog.h"
#include "ui_addnumdialog.h"
#include "utils.h"
#include <limits>
#include <QPushButton>
#include <QAbstractSpinBox>

addNumDialog::addNumDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::addNumDialog)
{
    ui->setupUi(this);

    ui->buttonBox->addButton(new QPushButton("Добавить"), QDialogButtonBox::ButtonRole::AcceptRole);
    ui->buttonBox->addButton(new QPushButton("Отмена"), QDialogButtonBox::ButtonRole::RejectRole);

    ui->doubleSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->doubleSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui->doubleSpinBox->setMaximum(std::numeric_limits<double>::max());

    ui->parameterBox->insertItem(CONCENTRATION, "Концентрация");
    ui->parameterBox->insertItem(LEVEL, "Отклонение уровня");
}

addNumDialog::~addNumDialog()
{
    delete ui;
}

parameter addNumDialog::getParameterValue() {
    return static_cast<parameter>(ui->parameterBox->currentIndex());
}

double addNumDialog::getNumValue() {
    return ui->doubleSpinBox->value();
}
