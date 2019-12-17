#ifndef MODBUSCONFIG_H
#define MODBUSCONFIG_H

#include <QTimer>
#include <QObject>
#include <QBitArray>
#include <QtSerialBus/qtserialbusglobal.h>
#include <QModbusDataUnit>
#include <QModbusTcpClient>
#include <QModbusRtuSerialMaster>
#include <QUrl>
#include <QVariant>
#include <QFileInfo>

#if QT_CONFIG(modbus_serialport)
#include <QSerialPort>
#endif

#define TimeRequestConnectToModbus 2000 // 2s
#define TimeReadRequesting         2000  // 0.2s

enum ModbusConnection {
    Serial,
    Tcp
};

class modbusconfig : public QObject
{
    Q_OBJECT
public:

    explicit modbusconfig(QObject *parent = nullptr);
    ~modbusconfig();

    void modbusInitialize();


    void signalsAndFunctionsConnect();

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole);

    QVariant headerData(int section, Qt::Orientation orientation, int role);

    bool setData(const QModelIndex &index, const QVariant &value, int role);

    Qt::ItemFlags flags(const QModelIndex &index);

private:
    QModbusDataUnit readRequest();

signals:
    void updateViewport();

    // Write Log files
    void writeLogSystem(const QString& messager);

public slots:
//    void setStartAddress(int address);
//    void setNumberOfValues(const QString &number);

    // Read modbus
    void readModbus();

private slots:
    void connectModbus(int state = QModbusDevice::UnconnectedState);

    void readReady();

    void requestConnectToMobus();

public:
    int m_number;
    int m_address;
    QBitArray m_coils;
    QVector<quint16> m_holdingRegisters;

    // Modbus config
    QModbusClient *modbusDevice;
    QModbusReply *lastRequest;

    // Timer check modbus connection
    QTimer  *timerModbusStatusConnection;

    // Timer request read data from Modbus server
    QTimer  *timerModbusReadRequest;

    // Timer check reponsion from modbus client device
    QTimer  *timerTimeOutReponseFromClient;

    // Timer used for request connect with modbus
    QTimer  *timerRequestConnectModbus;

public:
    struct Settings {
        QString portname = QString("/dev/ttyAMA0");
        QUrl    urlToBroker = QUrl("127.0.0.1");
#if QT_CONFIG(modbus_serialport)
        int parity = QSerialPort::EvenParity;
        int baud = QSerialPort::Baud19200;
        int dataBits = QSerialPort::Data8;
        int stopBits = QSerialPort::OneStop;
        int startAdress = 0;
        int numberOfDeviceReaded = 10;
#endif
        int responseTime = 200;
        int numberOfRetries = 3;
    };

private:
    Settings    m_settings;

#if QT_CONFIG(modbus_serialport)
    ModbusConnection modbusPortocol = ModbusConnection::Serial;
#else
    ModbusConnection modbusPortocol = ModbusConnection::Tcp;
#endif


};

#endif // MODBUSCONFIG_H
