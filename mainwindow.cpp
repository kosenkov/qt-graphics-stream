#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "datavisualization.h"
#include "datagenerator.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_dataVisualization(nullptr)
    , m_dataGenerator(nullptr)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initialize()
{
    m_dataVisualization = new DataVisualization(this);
    ui->centralWidget->layout()->addWidget(m_dataVisualization);

    m_dataGenerator = new DataGenerator(this);
    connect(m_dataGenerator, &DataGenerator::dataUpdated, this, &MainWindow::newDataAvailable);

    ui->tbPause->setEnabled(false);
    ui->tbStop->setEnabled(false);
}

void MainWindow::newDataAvailable(QPointF point)
{
    m_dataVisualization->appendPoint(point);
}

void MainWindow::on_tbStart_clicked()
{
    if(m_dataGenerator->isPaused())
    {
        m_dataGenerator->resumeGenerating();

        ui->tbStart->setEnabled(false);
        ui->tbPause->setEnabled(true);
    }
    else if(m_dataGenerator->isStoped())
    {
        m_dataGenerator->startGenerating();

        ui->tbStart->setEnabled(false);
        ui->tbPause->setEnabled(true);
        ui->tbStop->setEnabled(true);
    }

    m_dataVisualization->enableSeriesMoving(false);
    m_dataVisualization->enableSeriesZooming(false);
}

void MainWindow::on_tbPause_clicked()
{
    if(!m_dataGenerator->isStoped() && !m_dataGenerator->isPaused())
    {
        m_dataGenerator->pauseGenerating();

        m_dataVisualization->enableSeriesMoving(true);
        m_dataVisualization->enableSeriesZooming(true);

        ui->tbStart->setEnabled(true);
        ui->tbPause->setEnabled(false);
    }
}

void MainWindow::on_tbStop_clicked()
{
    if(!m_dataGenerator->isStoped())
    {
        m_dataGenerator->stopGenerating();

        m_dataVisualization->clearChart();
        m_dataVisualization->enableSeriesMoving(false);
        m_dataVisualization->enableSeriesZooming(false);

        ui->tbStart->setEnabled(true);
        ui->tbPause->setEnabled(false);
        ui->tbStop->setEnabled(false);
    }
}
