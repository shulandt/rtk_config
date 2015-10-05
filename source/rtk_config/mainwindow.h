#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QLabel>
#include <QStringList>
#include <QFileDialog>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_Connect_pushButton_clicked();
    void on_DataRead();
    void on_Request_pushButton_clicked();
    void on_Default_pushButton_clicked();
    void on_Set_pushButton_clicked();
    void on_tableWidget_cellDoubleClicked(int row, int column);
    void on_Save_pushButton_clicked();
    void on_SaveToFile_pushButton_clicked();
    void on_LoadFromFile_pushButton_clicked();
    void on_VRS_pushButton_clicked();

private:
    Ui::MainWindow* ui;
    QSerialPort* port;
    QFileDialog* file_dialog;
    QLabel *statusLabel;
    bool vrs_request;
    void SerialList();
    void nmea_handler(int msg);
    void SendNMEA(const char* buff);
};

#endif // MAINWINDOW_H
