#include "datagenerator.h"
#include <QtMath>
#include <QTime>
#include <thread>


DataGenerator::DataGenerator(QObject *parent)
    : QObject(parent)
    , m_data(0, 0)
    , m_pauseFlag(false)
    , m_stopFlag(true)
{
}

DataGenerator::~DataGenerator()
{
    stopGenerating();
}

void DataGenerator::resetData()
{
    m_data = QPointF(0, 0);
}

void DataGenerator::startGenerating()
{
    if(m_threadPtr.get() != nullptr)
        stopGenerating();

    qsrand(QTime::currentTime().msec());

    m_stopFlag = false;
    m_pauseFlag = false;

    m_threadPtr = std::make_unique<std::thread>([&](){

        while (!m_stopFlag)
        {
            //обработчик паузы
            std::unique_lock<std::mutex> guard(m_mutex);
            while (m_pauseFlag) {
                m_cvResume.wait(guard);
            }
            guard.unlock();

            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            m_data = QPointF(m_data.x() + random(10, 100),
                             random(-100, 100));

            emit dataUpdated(m_data);
        }
    });
}

void DataGenerator::pauseGenerating()
{
    m_pauseFlag = true;
}

void DataGenerator::resumeGenerating()
{
    m_pauseFlag = false;
    m_cvResume.notify_all();
}

void DataGenerator::stopGenerating()
{
    if(m_threadPtr.get() == nullptr)
        return;

    m_stopFlag = true;

    //если поток находится в режиме паузы,
    //то выводим его из неё
    resumeGenerating();

    //если поток по прежнему активен
    //то дожидаемся полной остановки
    if (m_threadPtr.get()->joinable())
        m_threadPtr.get()->join();

    //удаляем объект потока
    m_threadPtr.reset();

    //очищаем наш буфер с данными
    resetData();
}

bool DataGenerator::isPaused() const
{
    return m_pauseFlag;
}

bool DataGenerator::isStoped() const
{
    return m_stopFlag;
}

qreal DataGenerator::random(qreal min, qreal max)
{
    return static_cast<qreal>(qrand())/RAND_MAX*(max - min) + min;
}
