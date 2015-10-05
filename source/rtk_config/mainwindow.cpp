#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "nmea.h"
#include <QDebug>
#include <QTableWidget>
#include <QTableWidgetItem>

NMEA nmea; // Экземпляр класса протокола NMEA

//-----------------------------------------------------------------------------
// Конструктор главного окна
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // Инициализация строки состояния
    statusLabel = new QLabel;
    ui->statusBar->addWidget(statusLabel);
    statusLabel->setText("Status: not connected");
    // Инициализация таблицы
    ui->tableWidget->setRowHeight(2, 45);
    ui->tableWidget->setRowHeight(3, 45);
    ui->tableWidget->setRowHeight(4, 90);
    ui->tableWidget->setRowHeight(5, 45);
    ui->tableWidget->setRowHeight(7, 45);
    ui->tableWidget->setRowHeight(9, 45);
    ui->tableWidget->setRowHeight(11, 90);
    ui->tableWidget->setRowHeight(14, 45);
    ui->tableWidget->setRowHeight(15, 45);
    ui->tableWidget->setRowHeight(16, 45);
    ui->tableWidget->setRowHeight(18, 45);
    ui->tableWidget->setRowHeight(19, 60);
    ui->tableWidget->setRowHeight(20, 75);
    ui->tableWidget->setRowHeight(21, 45);
    ui->tableWidget->setRowHeight(22, 75);
    ui->tableWidget->setRowHeight(23, 45);
    ui->tableWidget->setRowHeight(24, 45);
    ui->tableWidget->setRowHeight(25, 60);
    ui->tableWidget->setRowHeight(26, 115);
    ui->tableWidget->setRowHeight(27, 60);
    // Инициализация виджета выбора файлов
    file_dialog = new QFileDialog(this);
    // Инициализация списка COM-портов и скоростей
    port = new QSerialPort(this);
    connect(port, SIGNAL(readyRead()), this, SLOT(on_DataRead()));
    SerialList();
    ui->Baud_comboBox->addItem("4800");
    ui->Baud_comboBox->addItem("9600");
    ui->Baud_comboBox->addItem("19200");
    ui->Baud_comboBox->addItem("38400");
    ui->Baud_comboBox->addItem("57600");
    ui->Baud_comboBox->addItem("115200");
    ui->Baud_comboBox->addItem("230400");
    ui->Baud_comboBox->setCurrentIndex(6);
    // Инициализация протокола NMEA
    nmea.Init();
    // Сброс признака VRS-запроса
    vrs_request = false;
}
//-----------------------------------------------------------------------------
// Деструктор главного окна
MainWindow::~MainWindow()
{
    delete port;
    delete file_dialog;
    delete ui;
}
//-----------------------------------------------------------------------------
// Функция создания списка доступных портов
void MainWindow::SerialList()
{
    QStringList pName;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        pName << info.portName();
        //pName << info.manufacturer();
    }
    pName.sort();
    ui->Serial_comboBox->clear();
    for(int i = 0; i < pName.count(); i++)
    {
        ui->Serial_comboBox->addItem(pName.at(i));
    }
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Connect
void MainWindow::on_Connect_pushButton_clicked()
{
    if(ui->Connect_pushButton->isChecked())
    {
        port->setPortName(ui->Serial_comboBox->currentText());
        port->setBaudRate(ui->Baud_comboBox->currentText().toInt());
        port->setFlowControl(QSerialPort::NoFlowControl);
        port->setParity(QSerialPort::NoParity);
        if(port->open(QIODevice::ReadWrite))
        {
            ui->Connect_pushButton->setText("Disconnect");
            statusLabel->setText("Status: wait connection");
            ui->Serial_comboBox->setEnabled(false);
            ui->Baud_comboBox->setEnabled(false);
            SendNMEA("$PNVGVER");
        }
        else
        {
            statusLabel->setText("Status: port is busy");
        }
    }
    else
    {
        if(port->isOpen())
            port->close();
        ui->Connect_pushButton->setText("Connect");
        statusLabel->setText("Status: not connected");
        ui->Serial_comboBox->setEnabled(true);
        ui->Baud_comboBox->setEnabled(true);
        ui->Request_pushButton->setEnabled(false);
        ui->Set_pushButton->setEnabled(false);
        ui->Save_pushButton->setEnabled(false);
        ui->VRS_pushButton->setEnabled(false);
    }
}
//-----------------------------------------------------------------------------
// Обработчик приема данных из COM-порта
void MainWindow::on_DataRead()
{
    char buffer[256];

    forever
    {
        int numRead = port->read(buffer, sizeof(buffer));
        if(numRead == 0)
            break;
        for(int i = 0; i < numRead; i++)
        {
            nmea_handler(nmea.Decode(buffer[i]));
        }
    }
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Request
void MainWindow::on_Request_pushButton_clicked()
{
    if(!port->isOpen())
      return;
    //SendNMEA("$PNVGVER");
    SendNMEA("$PNVGNME");
    SendNMEA("$PNVGRZA,1");
    SendNMEA("$PNVGRZA,2");
    SendNMEA("$PNVGRZA,3");
    SendNMEA("$PNVGRTK,MODE");
    SendNMEA("$PNVGRTK,PVTRATE");
    SendNMEA("$PNVGRTK,SYSGPS");
    SendNMEA("$PNVGRTK,SYSGLO");
    SendNMEA("$PNVGRTK,ELGPS");
    SendNMEA("$PNVGRTK,ELGLO");
    SendNMEA("$PNVGRTK,SNR");
    SendNMEA("$PNVGRTK,BASEID");
    SendNMEA("$PNVGRTK,BASEXYZ");
    SendNMEA("$PNVGRTK,BASEBLH");
    SendNMEA("$PNVGRTK,AVGTIME");
    SendNMEA("$PNVGRTK,AVGMODE");
    SendNMEA("$PNVGRTK,SFMODE");
    SendNMEA("$PNVGRTK,SFTRS");
    SendNMEA("$PNVGRTK,ICBGLO");
    SendNMEA("$PNVGRTK,ICBMODE");
    SendNMEA("$PNVGRTK,ICBTIME");
}
//-----------------------------------------------------------------------------
// Функция отправки сообщений NMEA
void MainWindow::SendNMEA(const char* buff)
{
    static char nmeabuff[80];
    int CSum = 0;

    strcpy(nmeabuff, buff);
    for(unsigned int i = 1; i < strlen(buff); i++)
        CSum ^= buff[i];
    sprintf(nmeabuff + strlen(buff), "*%02X\r\n", CSum);
    QByteArray arr;
    arr.append(nmeabuff);
    port->write(arr);
}
//-----------------------------------------------------------------------------
// Обработчик входящих сообщений NMEA
void MainWindow::nmea_handler(int msg)
{
    QString item_value;

    switch(msg)
    {
        // Обработка сообщения PNVGVER
        case _P_VER_:
        {
            statusLabel->setText("Status: connected");
            ui->Request_pushButton->setEnabled(true);
            ui->Set_pushButton->setEnabled(true);
            ui->Save_pushButton->setEnabled(true);
            ui->VRS_pushButton->setEnabled(true);
            //qDebug() << nmea.P_VERmsg.Item[1];
            item_value = nmea.P_VERmsg.Item[2];
            item_value += " ";
            item_value += nmea.P_VERmsg.Item[3];
            item_value += " ";
            item_value += nmea.P_VERmsg.Item[4];
            item_value += " ";
            item_value += nmea.P_VERmsg.Item[5];
            ui->tableWidget->item(1,1)->setText(item_value);
            break;
        }
        // Обработка сообщения PNVGCFG
        case _P_CFG_:
        {
            item_value = nmea.P_CFGmsg.Item[1];
            if(item_value == "OK")
                statusLabel->setText("Status: data is saved");
            else
                statusLabel->setText("Status: data is not saved");
            break;
        }
        // Обработка сообщения PNVGNME
        case _P_NME_:
        {
            item_value = nmea.P_NMEmsg.Item[1];
            ui->tableWidget->item(2,1)->setTextColor(Qt::blue);
            ui->tableWidget->item(2,1)->setText(item_value);
            item_value = nmea.P_NMEmsg.Item[2];
            ui->tableWidget->item(3,1)->setTextColor(Qt::blue);
            ui->tableWidget->item(3,1)->setText(item_value);
            item_value = nmea.P_NMEmsg.Item[3];
            ui->tableWidget->item(4,1)->setTextColor(Qt::blue);
            ui->tableWidget->item(4,1)->setText(item_value);
            break;
        }
        // Обработка сообщения PNVGRZA
        case _P_RZA_:
        {
            switch(nmea.P_RZAmsg.Item[1][0])
            {
                // Настройки порта COM1
                case '1':
                {
                    item_value = nmea.P_RZAmsg.Item[2];
                    ui->tableWidget->item(5,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(5,1)->setText(item_value);
                    item_value = nmea.P_RZAmsg.Item[3];
                    ui->tableWidget->item(6,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(6,1)->setText(item_value);
                    break;
                }
                // Настройки порта COM2
                case '2':
                {
                    item_value = nmea.P_RZAmsg.Item[2];
                    ui->tableWidget->item(7,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(7,1)->setText(item_value);
                    item_value = nmea.P_RZAmsg.Item[3];
                    ui->tableWidget->item(8,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(8,1)->setText(item_value);
                    // Если ответ получен в режиме VRS-запроса
                    if(vrs_request)
                    {
                        // Отключить на порту 2 все сообщения NMEA
                        SendNMEA("$PNVGRZB,PORT,2");
                        // Задать по порту 2 выдачу сообщений GGA
                        SendNMEA("$PNVGRZB,PORT,2,GGA,10");
                        // Сбросить признак VRS-запроса
                        vrs_request = false;
                    }
                    break;
                }
                // Настройки порта USB
                case '3':
                {
                    item_value = nmea.P_RZAmsg.Item[2];
                    ui->tableWidget->item(9,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(9,1)->setText(item_value);
                    item_value = nmea.P_RZAmsg.Item[3];
                    ui->tableWidget->item(10,1)->setTextColor(Qt::blue);
                    ui->tableWidget->item(10,1)->setText(item_value);
                    break;
                }
            }
            break;
        }
        // Обработка сообщения PNVGRTK
        case _P_RTK_:
        {
            // PNVGRTK,MODE
            if(!strcmp(nmea.P_RTKmsg.Item[1], "MODE"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(11,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(11,1)->setText(item_value);
            }
            // PNVGRTK,PVTRATE
            if(!strcmp(nmea.P_RTKmsg.Item[1], "PVTRATE"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(12,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(12,1)->setText(item_value);
            }
            // PNVGRTK,SYSGPS
            if(!strcmp(nmea.P_RTKmsg.Item[1], "SYSGPS"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(13,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(13,1)->setText(item_value);
            }
            // PNVGRTK,SYSGLO
            if(!strcmp(nmea.P_RTKmsg.Item[1], "SYSGLO"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(14,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(14,1)->setText(item_value);
            }
            // PNVGRTK,ELGPS
            if(!strcmp(nmea.P_RTKmsg.Item[1], "ELGPS"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(15,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(15,1)->setText(item_value);
            }
            // PNVGRTK,ELGLO
            if(!strcmp(nmea.P_RTKmsg.Item[1], "ELGLO"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(16,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(16,1)->setText(item_value);
            }
            // PNVGRTK,SNR
            if(!strcmp(nmea.P_RTKmsg.Item[1], "SNR"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(17,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(17,1)->setText(item_value);
            }
            // PNVGRTK,BASEID
            if(!strcmp(nmea.P_RTKmsg.Item[1], "BASEID"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(18,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(18,1)->setText(item_value);
            }
            // PNVGRTK,BASEXYZ
            if(!strcmp(nmea.P_RTKmsg.Item[1], "BASEXYZ"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                item_value += "\n";
                item_value += nmea.P_RTKmsg.Item[3];
                item_value += "\n";
                item_value += nmea.P_RTKmsg.Item[4];
                ui->tableWidget->item(19,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(19,1)->setText(item_value);
            }
            // PNVGRTK,BASEBLH
            if(!strcmp(nmea.P_RTKmsg.Item[1], "BASEBLH"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                item_value += "\n";
                item_value += nmea.P_RTKmsg.Item[3];
                item_value += "\n";
                item_value += nmea.P_RTKmsg.Item[4];
                ui->tableWidget->item(20,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(20,1)->setText(item_value);
            }
            // PNVGRTK,AVGTIME
            if(!strcmp(nmea.P_RTKmsg.Item[1], "AVGTIME"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(21,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(21,1)->setText(item_value);
            }
            // PNVGRTK,AVGMODE
            if(!strcmp(nmea.P_RTKmsg.Item[1], "AVGMODE"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(22,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(22,1)->setText(item_value);
            }
            // PNVGRTK,SFMODE
            if(!strcmp(nmea.P_RTKmsg.Item[1], "SFMODE"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(23,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(23,1)->setText(item_value);
            }
            // PNVGRTK,SFTRS
            if(!strcmp(nmea.P_RTKmsg.Item[1], "SFTRS"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(24,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(24,1)->setText(item_value);
            }
            // PNVGRTK,ICBGLO
            if(!strcmp(nmea.P_RTKmsg.Item[1], "ICBGLO"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(25,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(25,1)->setText(item_value);
            }
            // PNVGRTK,ICBMODE
            if(!strcmp(nmea.P_RTKmsg.Item[1], "ICBMODE"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(26,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(26,1)->setText(item_value);
            }
            // PNVGRTK,ICBTIME
            if(!strcmp(nmea.P_RTKmsg.Item[1], "ICBTIME"))
            {
                item_value = nmea.P_RTKmsg.Item[2];
                ui->tableWidget->item(27,1)->setTextColor(Qt::blue);
                ui->tableWidget->item(27,1)->setText(item_value);
            }
            break;
        }

    }
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Default
void MainWindow::on_Default_pushButton_clicked()
{
    // Записываем в таблицу параметры по умолчанию, цвет текста - красный
    ui->tableWidget->item(2,1)->setTextColor(Qt::red);
    ui->tableWidget->item(2,1)->setText("2");
    ui->tableWidget->item(3,1)->setTextColor(Qt::red);
    ui->tableWidget->item(3,1)->setText("7");
    ui->tableWidget->item(4,1)->setTextColor(Qt::red);
    ui->tableWidget->item(4,1)->setText("0");
    ui->tableWidget->item(5,1)->setTextColor(Qt::red);
    ui->tableWidget->item(5,1)->setText("115200");
    ui->tableWidget->item(6,1)->setTextColor(Qt::red);
    ui->tableWidget->item(6,1)->setText("1");
    ui->tableWidget->item(7,1)->setTextColor(Qt::red);
    ui->tableWidget->item(7,1)->setText("115200");
    ui->tableWidget->item(8,1)->setTextColor(Qt::red);
    ui->tableWidget->item(8,1)->setText("7");
    ui->tableWidget->item(9,1)->setTextColor(Qt::red);
    ui->tableWidget->item(9,1)->setText("230400");
    ui->tableWidget->item(10,1)->setTextColor(Qt::red);
    ui->tableWidget->item(10,1)->setText("1");
    ui->tableWidget->item(11,1)->setTextColor(Qt::red);
    ui->tableWidget->item(11,1)->setText("2");
    ui->tableWidget->item(12,1)->setTextColor(Qt::red);
    ui->tableWidget->item(12,1)->setText("5");
    ui->tableWidget->item(13,1)->setTextColor(Qt::red);
    ui->tableWidget->item(13,1)->setText("1");
    ui->tableWidget->item(14,1)->setTextColor(Qt::red);
    ui->tableWidget->item(14,1)->setText("1");
    ui->tableWidget->item(15,1)->setTextColor(Qt::red);
    ui->tableWidget->item(15,1)->setText("5");
    ui->tableWidget->item(16,1)->setTextColor(Qt::red);
    ui->tableWidget->item(16,1)->setText("5");
    ui->tableWidget->item(17,1)->setTextColor(Qt::red);
    ui->tableWidget->item(17,1)->setText("33");
    ui->tableWidget->item(18,1)->setTextColor(Qt::red);
    ui->tableWidget->item(18,1)->setText("0002");
    ui->tableWidget->item(19,1)->setTextColor(Qt::red);
    ui->tableWidget->item(19,1)->setText("0.0000\n0.0000\n0.0000");
    ui->tableWidget->item(20,1)->setTextColor(Qt::red);
    ui->tableWidget->item(20,1)->setText("-1.5707963268\n0.0000000000\n-6378137.0000");
    ui->tableWidget->item(21,1)->setTextColor(Qt::red);
    ui->tableWidget->item(21,1)->setText("30");
    ui->tableWidget->item(22,1)->setTextColor(Qt::red);
    ui->tableWidget->item(22,1)->setText("0");
    ui->tableWidget->item(23,1)->setTextColor(Qt::red);
    ui->tableWidget->item(23,1)->setText("0");
    ui->tableWidget->item(24,1)->setTextColor(Qt::red);
    ui->tableWidget->item(24,1)->setText("0.050");
    ui->tableWidget->item(25,1)->setTextColor(Qt::red);
    ui->tableWidget->item(25,1)->setText("0.000");
    ui->tableWidget->item(26,1)->setTextColor(Qt::red);
    ui->tableWidget->item(26,1)->setText("0");
    ui->tableWidget->item(27,1)->setTextColor(Qt::red);
    ui->tableWidget->item(27,1)->setText("20");
}
//-----------------------------------------------------------------------------
// Обработчик редактирования ячейки
void MainWindow::on_tableWidget_cellDoubleClicked(int row, int column)
{
    // При редактировании ячейки меняем цвет текста на красный
    ui->tableWidget->item(row,column)->setTextColor(Qt::red);
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Set
void MainWindow::on_Set_pushButton_clicked()
{
    QString nmea_msg;

    // PNVGNME
    nmea_msg = "$PNVGNME," + ui->tableWidget->item(2,1)->text() + "," +
               ui->tableWidget->item(3,1)->text() + "," +
               ui->tableWidget->item(4,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());

    // PNVGRZA
    // Для порта COM1
    nmea_msg = "$PNVGRZA,1," + ui->tableWidget->item(5,1)->text() + "," +
               ui->tableWidget->item(6,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // Для порта COM2
    nmea_msg = "$PNVGRZA,2," + ui->tableWidget->item(7,1)->text() + "," +
               ui->tableWidget->item(8,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // Для порта USB
    nmea_msg = "$PNVGRZA,3," + ui->tableWidget->item(9,1)->text() + "," +
               ui->tableWidget->item(10,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());

    // PNVGRTK
    // MODE
    nmea_msg = "$PNVGRTK,MODE," + ui->tableWidget->item(11,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // PVTRATE
    nmea_msg = "$PNVGRTK,PVTRATE," + ui->tableWidget->item(12,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // SYSGPS
    nmea_msg = "$PNVGRTK,SYSGPS," + ui->tableWidget->item(13,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // SYSGLO
    nmea_msg = "$PNVGRTK,SYSGLO," + ui->tableWidget->item(14,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // ELGPS
    nmea_msg = "$PNVGRTK,ELGPS," + ui->tableWidget->item(15,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // ELGLO
    nmea_msg = "$PNVGRTK,ELGLO," + ui->tableWidget->item(16,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // SNR
    nmea_msg = "$PNVGRTK,SNR," + ui->tableWidget->item(17,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // BASEID
    nmea_msg = "$PNVGRTK,BASEID," + ui->tableWidget->item(18,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // BASEXYZ
    char tempbuff[80];
    // Проверка, менялись ли координаты (пока по цвету шрифта в ячейке)
    if(ui->tableWidget->item(19,1)->textColor() == Qt::red)
    {
        nmea_msg = "$PNVGRTK,BASEXYZ," + ui->tableWidget->item(19,1)->text();
        // Замена символов перевода каретки запятыми. Пока не знаю, как это
        // сделать методами Qt
        strcpy(tempbuff, nmea_msg.toStdString().c_str());
        for(unsigned int i = 0; i < strlen(tempbuff); i++)
            if(tempbuff[i] == '\n')
                tempbuff[i] = ',';
        SendNMEA(tempbuff);
        SendNMEA("$PNVGRTK,BASEBLH");
    }
    // BASEBLH
    // Проверка, менялись ли координаты (пока по цвету шрифта в ячейке)
    if(ui->tableWidget->item(20,1)->textColor() == Qt::red)
    {
        nmea_msg = "$PNVGRTK,BASEBLH," + ui->tableWidget->item(20,1)->text();
        // Замена символов перевода каретки запятыми. Пока не знаю, как это
        // сделать методами Qt
        strcpy(tempbuff, nmea_msg.toStdString().c_str());
        for(unsigned int i = 0; i < strlen(tempbuff); i++)
            if(tempbuff[i] == '\n')
                tempbuff[i] = ',';
        SendNMEA(tempbuff);
        SendNMEA("$PNVGRTK,BASEXYZ");
    }
    // AVGTIME
    nmea_msg = "$PNVGRTK,AVGTIME," + ui->tableWidget->item(21,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // AVGMODE
    nmea_msg = "$PNVGRTK,AVGMODE," + ui->tableWidget->item(22,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // SFMODE
    nmea_msg = "$PNVGRTK,SFMODE," + ui->tableWidget->item(23,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // SFTRS
    nmea_msg = "$PNVGRTK,SFTRS," + ui->tableWidget->item(24,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // ICBGLO
    nmea_msg = "$PNVGRTK,ICBGLO," + ui->tableWidget->item(25,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // ICBMODE
    nmea_msg = "$PNVGRTK,ICBMODE," + ui->tableWidget->item(26,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
    // ICBTIME
    nmea_msg = "$PNVGRTK,ICBTIME," + ui->tableWidget->item(27,1)->text();
    SendNMEA(nmea_msg.toStdString().c_str());
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Save to Flash
void MainWindow::on_Save_pushButton_clicked()
{
    // Запись конфигурационных данных в энергонезависимую память устройства
    SendNMEA("$PNVGCFG,W");
    statusLabel->setText("Status: saving data...");
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Save to File
void MainWindow::on_SaveToFile_pushButton_clicked()
{
    // Запись конфигурационных данных в файл
    QString file_name = file_dialog->getSaveFileName(this, tr("Save File"),
                                                     "rtk_config.csv",
                                                     tr("*.csv"));
    if(file_name == "")
        return;

    QFile f(file_name);
    if(f.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QTextStream ts(&f);
        for(int i = 2; i < ui->tableWidget->rowCount();i++)
            ts << ui->tableWidget->item(i,1)->text() << ";";
        f.close();
    }
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки Load from File
void MainWindow::on_LoadFromFile_pushButton_clicked()
{
    // Чтение конфигурационных данных из файла
    QString file_name = file_dialog->getOpenFileName(this, tr("Open File"),
                                                     "rtk_config.csv",
                                                     tr("*.csv"));
    if(file_name == "")
        return;

    QFile f(file_name);
    if(f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString     value = f.readAll();
        QStringList rowData;
        rowData.clear();
        rowData = value.split(";");
        for(int i = 2; i < ui->tableWidget->rowCount();i++)
        {
            ui->tableWidget->item(i,1)->setText(rowData[i - 2]);
            ui->tableWidget->item(i,1)->setTextColor(Qt::red);
        }
        f.close();
    }
}
//-----------------------------------------------------------------------------
// Обработчик нажатия кнопки VRS
void MainWindow::on_VRS_pushButton_clicked()
{
    // Установить на порту 2 протокол NMEA
    SendNMEA("$PNVGRZA,2,115200,1");
    // Установить признак VRS-запроса
    vrs_request = true;
    /*
    // Отключить на порту 2 все сообщения NMEA
    SendNMEA("$PNVGRZB,PORT,2");
    // Задать по порту 2 выдачу сообщений GGA
    SendNMEA("$PNVGRZB,PORT,2,GGA,10");
    */
}
//-----------------------------------------------------------------------------
