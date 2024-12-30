#include "addnumdialog.h"
#include "ui_addnumdialog.h"
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

    ui->consumSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->consumSpinBox->setDecimals(5);
    ui->consumSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui->consumSpinBox->setMaximum(std::numeric_limits<double>::max());

    ui->concSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->concSpinBox->setDecimals(5);
    ui->concSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui->concSpinBox->setMaximum(std::numeric_limits<double>::max());

    ui->levelSpinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->levelSpinBox->setDecimals(5);
    ui->levelSpinBox->setMinimum(-std::numeric_limits<double>::max());
    ui->levelSpinBox->setMaximum(std::numeric_limits<double>::max());
}

addNumDialog::~addNumDialog()
{
    delete ui;
}

double addNumDialog::getConsumptionValue() {
    return ui->consumSpinBox->value();
}

double addNumDialog::getConcentrationValue() {
    return ui->concSpinBox->value();
}

double addNumDialog::getLevelValue() {
    return ui->levelSpinBox->value();
}
