#include "modbusconfig.h"
#include "QDebug"

modbusconfig::modbusconfig(QObject *parent) : QObject(parent)
    , modbusDevice(nullptr)
    , lastRequest(nullptr)
{

    // Timer check connection with Modbus
    timerRequestConnectModbus = new QTimer();
    if(false == timerRequestConnectModbus->isActive())
    {
        timerRequestConnectModbus->setInterval(TimeRequestConnectToModbus);

        connect(timerRequestConnectModbus, &QTimer::timeout,
                this, &modbusconfig::requestConnectToMobus);

        timerRequestConnectModbus->stop();
    }

    // Timer request read Modbus
    timerModbusReadRequest = new QTimer();
    if(false == timerModbusReadRequest->isActive())
    {
        timerModbusReadRequest->setInterval(TimeReadRequesting);

        connect(timerModbusReadRequest, &QTimer::timeout,
                this, &modbusconfig::readModbus);

        timerModbusReadRequest->stop();
    }

    modbusInitialize();
}

modbusconfig::~modbusconfig()
{
    // Detructor:
    if(this->modbusDevice)
    {
        modbusDevice->disconnectDevice();
    }

    delete modbusDevice;

}

void modbusconfig::modbusInitialize()
{
    if(nullptr != modbusDevice)
    {
        modbusDevice->disconnectDevice();
        delete  modbusDevice;
        modbusDevice = nullptr;
    }

    auto type = static_cast<ModbusConnection>(modbusPortocol);
    if(type == Serial)
    {
#if QT_CONFIG(modbus_serialport)
    modbusDevice = new QModbusRtuSerialMaster();
    qDebug() << "Connect Modbus RTU via serial";
#endif
    }
    else if(type == Tcp)
    {
        modbusDevice = new QModbusTcpClient();
        qDebug() << "Connect Modbus Tcp";

    }

    connect(modbusDevice, &QModbusClient::errorOccurred, [this](QModbusDevice::Error) {
        qDebug() << modbusDevice->errorString();
        emit writeLogSystem(QString(modbusDevice->errorString()));
    });

    if (nullptr != modbusDevice)
    {
        // Stop connect with Modbus
        if(QModbusDevice::ConnectedState == modbusDevice->state()) {
            modbusDevice->disconnectDevice();
        }

        if(static_cast<ModbusConnection>(modbusPortocol) == Serial)
        {
            if(!QFileInfo(m_settings.portname).exists())
            {
                qDebug() << "Port does not exist";
            }

            modbusDevice->setConnectionParameter(QModbusDevice::SerialPortNameParameter,
                m_settings.portname);
#if QT_CONFIG(modbus_serialport)
            modbusDevice->setConnectionParameter(QModbusDevice::SerialParityParameter,
                m_settings.parity);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialBaudRateParameter,
                m_settings.baud);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialDataBitsParameter,
                m_settings.dataBits);
            modbusDevice->setConnectionParameter(QModbusDevice::SerialStopBitsParameter,
                m_settings.stopBits);
            qDebug() << "Configure modbus serial: " << m_settings.portname;
#endif
        }

        else if(static_cast<ModbusConnection>(modbusPortocol) == Tcp)
        {
            const QUrl url = m_settings.urlToBroker;
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkPortParameter, url.port());
            modbusDevice->setConnectionParameter(QModbusDevice::NetworkAddressParameter, url.url());
        }

        modbusDevice->setTimeout(m_settings.responseTime);
        modbusDevice->setNumberOfRetries(m_settings.numberOfRetries);

        // Connect Functions
        signalsAndFunctionsConnect();

        // Connect to Modbus
        if(!modbusDevice->connectDevice()) {
            qDebug() << "Connect faied: " << modbusDevice->errorString();
        }
    }
}

void modbusconfig::connectModbus(int state)
{
    if(nullptr != modbusDevice)
    {
        if(state == QModbusDevice::UnconnectedState)
        {
            qDebug() << "Disconnect to Modbus !!!";
            qDebug() << "QModbusDevice: " << state;

            // Reconnect to Modbus
            if(false == modbusDevice->connectDevice())
            {
                // starting timer check connection with Modbus
                qDebug() << "Disconnect with Modbus";

                timerModbusReadRequest->stop();
            }

            timerRequestConnectModbus->start();
            // Write Log
            emit writeLogSystem(QString("Lost connect to Modbus. Reconnecting...."));
        }

        else if(state == QModbusDevice::ConnectedState)
        {
            qDebug() << "QModbusDevice: " << state;

            qDebug() << "Connected to Modbus Network!!!";

            // stop timer check connection with MOdbus
            timerRequestConnectModbus->stop();

            timerModbusReadRequest->start();

            // Write Log
            emit writeLogSystem(QString("Connected to Modbus Network!!!"));
        }

        else
        {
            // Do nothing
        }
    }

    else
    {
        // Do nothing
    }
}

void modbusconfig::signalsAndFunctionsConnect()
{
    if(nullptr != modbusDevice)
    {
        connect(modbusDevice, &QModbusClient::stateChanged,
                this, &modbusconfig::connectModbus);
    }

    else if(nullptr == modbusDevice)
    {
        // Do nothing
    }
}

QModbusDataUnit modbusconfig::readRequest()
{
    int startAddress = m_settings.startAdress;
    Q_ASSERT(startAddress >= 0 && startAddress < 10);

    // do not go beyond 127 entries
    int numberOfEntries = m_settings.numberOfDeviceReaded;
    return QModbusDataUnit(QModbusDataUnit::Coils, startAddress, quint16(numberOfEntries));
}

void modbusconfig::readModbus()
{
    if(!modbusDevice) {
        return;
    }

    qDebug() << "Read: " + modbusDevice->errorString();

    // Read Modbus address 0 first: --> this is only for testing
    if(auto *reply = modbusDevice->sendReadRequest(readRequest(), int(1))) {
        if(!reply->isFinished()) {
            qDebug() << "replay connected";
            connect(reply, &QModbusReply::finished,
                    this, &modbusconfig::readReady);
        }

        else {
            qDebug() << "Delete reply: " << reply;
            delete reply; // Broadcast replies return immediately
        }
    }

    else {
        qDebug() << "Read error: " + modbusDevice->errorString();

        // Write Log files
        emit writeLogSystem("Read error: " + modbusDevice->errorString());
    }

}

void modbusconfig::readReady()
{
    auto reply = qobject_cast<QModbusReply *>(sender());
    if(!reply) {
        return;
    }

    if(reply->error() == QModbusDevice::NoError) {
        const QModbusDataUnit unit = reply->result();
        for(uint i = 0; i < unit.valueCount(); i++)
        {
            const QString entry = tr("Address: %1, Value: %2").arg(unit.startAddress() + int(i))
                    .arg(QString::number(unit.value(int(i))));
            qDebug() << entry;
        }
    }

    else if (reply->error() == QModbusDevice::ProtocolError) {
        qDebug() << QString("Read response error: %1 (Modbus exception: 0x%2)")
                    .arg(reply->errorString())
                    .arg(reply->rawResult().exceptionCode(), -1, 16);

        // Wrtie Logs
        emit writeLogSystem(QString("Read response error: %1 (Modbus exception: 0x%2)")
                            .arg(reply->errorString())
                            .arg(reply->rawResult().exceptionCode(), -1, 16));
    }

    else {
        qDebug() << (QString(tr("Read response error: %1 (code: 0x%2)")).
                                    arg(reply->errorString()).
                                    arg(reply->error(), -1, 16));

        // Write Logs
        emit writeLogSystem(QString(tr("Read response error: %1 (code: 0x%2)")).
                            arg(reply->errorString()).
                            arg(reply->error(), -1, 16));
    }

    reply->deleteLater();
}

void modbusconfig::requestConnectToMobus()
{
    // request connect to Modbus
    if(nullptr != modbusDevice)
    {
        if(QModbusDevice::UnconnectedState == modbusDevice->state()) {
            modbusDevice->connectDevice();
            qDebug() << "Disconnected to Modbus";
        }

        else
        {
            qDebug() << "Connected to Modbus: " << modbusDevice->state();
        }
    }

    else
    {
        // Do nothing
    }
}
