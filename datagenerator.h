#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QPointF>
#include <thread>
#include <memory>
#include <condition_variable>


class DataGenerator : public QObject
{
    Q_OBJECT
public:
    explicit DataGenerator(QObject *parent);
    ~DataGenerator();

    bool isPaused() const;
    bool isStoped() const;

public slots:
    void startGenerating();
    void pauseGenerating();
    void resumeGenerating();
    void stopGenerating();

signals:
    void dataUpdated(QPointF point);

private:
    void resetData();
    qreal random(qreal min, qreal max);

    QPointF m_data;
    std::unique_ptr<std::thread> m_threadPtr;
    std::atomic_bool m_pauseFlag;
    std::atomic_bool m_stopFlag;
    std::condition_variable m_cvResume;
    std::mutex m_mutex;
};

#endif // DATAGENERATOR_H
