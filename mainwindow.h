#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class DataVisualization;
class DataGenerator;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void newDataAvailable(QPointF point);
    void on_tbStart_clicked();
    void on_tbPause_clicked();
    void on_tbStop_clicked();

signals:
    void allDataRecieved();

private:
    void initialize();

    DataVisualization *m_dataVisualization;
    DataGenerator* m_dataGenerator;

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
