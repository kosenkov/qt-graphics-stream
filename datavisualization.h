#ifndef DATAVISUALIZATION_H
#define DATAVISUALIZATION_H

#include <QWidget>
#include <QtCharts>
#include <QChart>
#include <QLineSeries>
#include <QValueAxis>


class DataVisualization : public QChartView
{
    Q_OBJECT
public:
    explicit DataVisualization(QWidget *parent);

    void appendPoint(const QPointF &point);
    void clearChart();
    void enableAntialiasing(bool state);
    void enableSeriesMoving(bool state);
    void enableSeriesZooming(bool state);

private slots:
    void updateAxes(int pointIndex);

private:
    void initialize();
    void resetAxes();

    void initSratterSeries();
    void initLineSeries();

    QChart* m_chart;
    QScatterSeries *m_scatterSeries;
    QLineSeries *m_lineSeries;
    QValueAxis *m_axisX;
    QValueAxis *m_axisY;

    qreal m_maxX;
    qreal m_minX;
    qreal m_maxY;
    qreal m_minY;

    const qreal m_defaultRangeMin;
    const qreal m_defaultRangeMax;

    QPoint m_mouseLastPos;

    bool m_movingEnabled;
    bool m_zoomingEnabled;

    // QChartView interface
protected:
    void wheelEvent(QWheelEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
};

#endif // DATAVISUALIZATION_H
