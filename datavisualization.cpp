#include "datavisualization.h"


DataVisualization::DataVisualization(QWidget *parent)
    : QChartView(parent)
    , m_chart(nullptr)
    , m_scatterSeries(nullptr)
    , m_lineSeries(nullptr)
    , m_axisX(nullptr)
    , m_axisY(nullptr)
    , m_maxX(std::numeric_limits<qreal>::min())
    , m_minX(std::numeric_limits<qreal>::max())
    , m_maxY(std::numeric_limits<qreal>::min())
    , m_minY(std::numeric_limits<qreal>::max())
    , m_defaultRangeMin(0)
    , m_defaultRangeMax(1)
    , m_mouseLastPos(0, 0)
    , m_movingEnabled(false)
    , m_zoomingEnabled(false)
{
    initialize();
}

void DataVisualization::initialize()
{
    setStyleSheet("QFrame { background-color: rgb(52, 73, 94); }");
    setRenderHint(QPainter::Antialiasing, true);

    m_chart = new QChart();
    m_chart->legend()->hide();
    m_chart->setTheme(QChart::ChartTheme::ChartThemeDark);
    m_chart->setBackgroundBrush(QBrush(QColor(52, 73, 94)));

    m_axisX = new QValueAxis(m_chart);
    m_axisY = new QValueAxis(m_chart);

    m_chart->addAxis(m_axisY, Qt::AlignLeft);
    m_chart->addAxis(m_axisX, Qt::AlignBottom);

    //ось X
    m_axisX->setLabelFormat("%i");
    m_axisX->setTickCount(10);
    m_axisX->setRange(m_defaultRangeMin, m_defaultRangeMax);

    //ось Y
    m_axisY->setLabelFormat("%.2f");
    m_axisY->setTickCount(10);
    m_axisY->setRange(m_defaultRangeMin, m_defaultRangeMax);

    initLineSeries();
    initSratterSeries();

    setChart(m_chart);
}

void DataVisualization::initSratterSeries()
{
    m_scatterSeries = new QScatterSeries(this);
    m_scatterSeries->setMarkerSize(15);
    m_scatterSeries->setBorderColor(QColor(7, 56, 0));
    m_scatterSeries->setColor(QColor(23, 178, 0));
    connect(m_scatterSeries, &QScatterSeries::pointAdded, this, &DataVisualization::updateAxes);
    m_chart->addSeries(m_scatterSeries);

    m_scatterSeries->attachAxis(m_axisX);
    m_scatterSeries->attachAxis(m_axisY);
}

void DataVisualization::initLineSeries()
{
    m_lineSeries = new QLineSeries(this);
    m_lineSeries->setPen(QPen(QBrush(QColor(23, 178, 0, 50)), 2));
    m_chart->addSeries(m_lineSeries);

    m_lineSeries->attachAxis(m_axisX);
    m_lineSeries->attachAxis(m_axisY);
}

void DataVisualization::appendPoint(const QPointF &point)
{
    m_scatterSeries->append(point);
    m_lineSeries->append(point);
}

void DataVisualization::resetAxes()
{
    for (int i = 0; i < 2; i++)
    {
        m_minX = std::numeric_limits<qreal>::max();
        m_maxX = std::numeric_limits<qreal>::min();
        m_minY = std::numeric_limits<qreal>::max();
        m_maxY = std::numeric_limits<qreal>::min();

        m_axisX->setRange(m_defaultRangeMin, m_defaultRangeMax);
        m_axisY->setRange(m_defaultRangeMin, m_defaultRangeMax);

        QApplication::processEvents();

        emit m_axisX->rangeChanged(m_defaultRangeMin, m_defaultRangeMax);
        emit m_axisY->rangeChanged(m_defaultRangeMin, m_defaultRangeMax);

        m_scatterSeries->clear();
        m_lineSeries->clear();

        m_chart->update();
    }
}

void DataVisualization::clearChart()
{
    m_scatterSeries->clear();
    m_lineSeries->clear();

    resetAxes();
}

void DataVisualization::enableSeriesMoving(bool state)
{
    m_movingEnabled = state;
}

void DataVisualization::enableSeriesZooming(bool state)
{
    m_zoomingEnabled = state;
}

void DataVisualization::updateAxes(int pointIndex)
{
    if(m_scatterSeries->count() == 0)
        return;

    auto& point = m_scatterSeries->at(pointIndex);

    qreal x = point.x();
    qreal y = point.y();

    if(x < m_minX || x > m_maxX)
    {
        m_minX = std::min(m_minX, x);
        m_maxX = std::max(m_maxX, x);

        m_axisX->setRange(m_minX, m_maxX);
    }

    if(y < m_minY || y > m_maxY)
    {
        m_minY = std::min(m_minY, y);
        m_maxY = std::max(m_maxY, y);

        m_axisY->setRange(m_minY, m_maxY);
    }

    if(chart()->isZoomed())
        chart()->zoomReset();
}

void DataVisualization::wheelEvent(QWheelEvent *event)
{
    if(m_zoomingEnabled)
    {
        auto zoomFactor = event->angleDelta().y() > 0 ? -0.2: 0.2;

        auto deltaX = m_axisX->max() - m_axisX->min();
        auto minAfterZoom = m_axisX->min() - deltaX * zoomFactor;
        auto maxAfterZoom = m_axisX->max() + deltaX * zoomFactor;

        //ограничиваем "zoom out"
        if(minAfterZoom > m_minX && maxAfterZoom < m_maxX)
            m_axisX->setRange(minAfterZoom, maxAfterZoom);
        else
            m_axisX->setRange(m_minX, m_maxX);
    }

    QChartView::wheelEvent(event);
}

void DataVisualization::mouseMoveEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_movingEnabled)
    {
        auto delta = event->pos() - m_mouseLastPos;

        chart()->scroll(-delta.x(), 0);

        //контроль пределов перемещения серии
        qreal deltaX = m_axisX->max() - m_axisX->min();
        if(m_axisX->min() < m_minX)
        {
            m_axisX->setRange(m_minX, m_minX + deltaX);
        }
        else if(m_axisX->max() > m_maxX)
        {
            m_axisX->setRange(m_maxX - deltaX, m_maxX);
        }

        m_mouseLastPos = event->pos();
    }

    QChartView::mouseMoveEvent(event);
}

void DataVisualization::mousePressEvent(QMouseEvent *event)
{
    if ((event->buttons() & Qt::LeftButton) && m_movingEnabled)
    {
        QApplication::setOverrideCursor(QCursor(Qt::SizeHorCursor));

        m_mouseLastPos = event->pos();
    }

    QChartView::mousePressEvent(event);
}

void DataVisualization::mouseReleaseEvent(QMouseEvent *event)
{
    QApplication::restoreOverrideCursor();

    QChartView::mouseReleaseEvent(event);
}




