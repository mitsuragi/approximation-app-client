#include "chartdialog.h"
#include "ui_chartdialog.h"

QSplineSeries* createSeries(QVector<double> x, QVector<double> y, QVector<double>xCoef = {});
double calculatePoint(double x, QVector<double> xCoef);

chartDialog::chartDialog(const QVector<double>& _concentrationVector,
                         const QVector<double>& _levelVector,
                         const QVector<double>& _consumptionVector,
                         const QVector<double>& _coefficientsConcentration,
                         const QVector<double>& _coefficientsLevel,
                         QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::chartDialog)
{
    ui->setupUi(this);

    concentrationVector = _concentrationVector;
    levelVector = _levelVector;
    consumptionVector = _consumptionVector;
    coefficientsConcentration = _coefficientsConcentration;
    coefficientsLevel = _coefficientsLevel;

    QValueAxis *axisX = new QValueAxis;
    int range = consumptionVector.last();
    axisX->setRange(0, range);
    axisX->setTickCount(consumptionVector.size());
    axisX->setLabelFormat("%i");

    QChart *chart = initChart();
    chart->createDefaultAxes();
    chart->setAxisX(axisX);
    chart->setAnimationOptions(QChart::NoAnimation);
    chart->setBackgroundRoundness(0);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setVisible(true);

    QChartView *chartView = new QChartView(chart, this);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setVisible(true);
    chartView->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    chartView->setRubberBand(QChartView::ClickThroughRubberBand);

    ui->layout->addWidget(chartView);

    setWindowFlags(Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
}

chartDialog::~chartDialog()
{
    delete ui;
}

QChart* chartDialog::initChart() {
    QSplineSeries *concSeries = createSeries(consumptionVector, concentrationVector);
    concSeries->setName("Концентрация(Точки)");
    concSeries->setColor(QColor::fromRgb(0, 255, 0));
    concSeries->setPointsVisible(true);

    QSplineSeries *levelSeries = createSeries(consumptionVector, levelVector);
    levelSeries->setName("Отклонение уровня(Точки)");
    levelSeries->setColor(QColor::fromRgb(1, 1, 1));
    levelSeries->setPointsVisible(true);

    QSplineSeries *concCoefSeries = createSeries(consumptionVector, concentrationVector, coefficientsConcentration);
    concCoefSeries->setName("Концентрация");
    concCoefSeries->setColor(QColor::fromRgb(255, 0, 0));

    QSplineSeries *levelCoefSeries = createSeries(consumptionVector, levelVector, coefficientsLevel);
    levelCoefSeries->setName("Отклонение уровня");
    levelCoefSeries->setColor(QColor::fromRgb(0, 0, 255));

    QChart *chart = new QChart;
    chart->addSeries(concSeries);
    chart->addSeries(levelSeries);
    chart->addSeries(concCoefSeries);
    chart->addSeries(levelCoefSeries);

    return chart;
}

QSplineSeries* createSeries(QVector<double> x, QVector<double> y, QVector<double>xCoef) {
    QSplineSeries *series = new QSplineSeries;
    if (xCoef.isEmpty()) {
        for (int i = 0; i < x.size(); i++) {
            series->append(x[i], y[i]);
        }
    }
    else {
        for (int i = 0; i < x.size(); i++) {
            series->append(x[i], calculatePoint(x[i], xCoef));
        }
    }

    return series;
}

double calculatePoint(double x, QVector<double> xCoef) {
    return xCoef[0] * x + xCoef[1];
}
