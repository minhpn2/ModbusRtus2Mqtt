#include "recordlatencynetworkexcelfile.h"

recordLatencyNetworkExcelFIle::recordLatencyNetworkExcelFIle(const int &carId, const int &routeId)
{
    // Initialize all param for this function
    this->routeId = routeId;
    this->carId = carId;
    // Timer control request calcule time
    timerRequestCalculateLatency = new QTimer();
    if(false == timerRequestCalculateLatency->isActive())
    {
        timerRequestCalculateLatency->setInterval(TIME_REQUEST_CALCULATE_LATENCY);
        connect(timerRequestCalculateLatency, SIGNAL(timeout()),
                this, SLOT(requestCalculateLatencyTime()));

        timerRequestCalculateLatency->start(); // start this timer
    }

    openRecordFileTxt();

}

recordLatencyNetworkExcelFIle::~recordLatencyNetworkExcelFIle()
{
    // does nothing, it should be never called
}

void recordLatencyNetworkExcelFIle::openRecordFileTxt()
{
    QDateTime m_currentTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Asia/Ho_Chi_Minh"));
    QString dateLogFile = m_currentTime.toString(QString("yyyyMMdd")) +"_"+ _car_number +"_Call.txt";
    m_file_name_log_created = "/usr/local/qt5pi/vpos/logfiles/" + dateLogFile;

    _file_log_created.setFileName(m_file_name_log_created);
    _file_log_created.open(QIODevice::Append | QIODevice::Text);
}

void recordLatencyNetworkExcelFIle::closeRecordFileTxt()
{

}

void recordLatencyNetworkExcelFIle::recordLatencyTxt(const QString &message)
{
    QDateTime m_currentTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Asia/Ho_Chi_Minh"));
    QString timeinvoice =  m_currentTime.toString(QString("HH:mm:ss"));
    QTextStream stream(&_file_log_created);
    stream << timeinvoice + ": "+ message << endl;
    stream << "\n" << endl;
}

void recordLatencyNetworkExcelFIle::receiveReponseFromServer(const QByteArray &message, const QMqttTopicName &topic)
{
    qDebug() << "message "<< message << "topic " << topic.name();

    QString jsonMessage(message);
    QJsonDocument jsd = QJsonDocument::fromJson(jsonMessage.toUtf8());
    QJsonObject   jsoObj = jsd.object();

    if((true == jsoObj.contains("data")))
    {
        QString jsomessenge = jsd["data"].toString();
        QJsonDocument jsdData = QJsonDocument::fromJson(jsomessenge.toUtf8());
        QJsonObject   jso = jsdData.object();

        if((true == jso.contains("car_id"))
           && (true == jso.contains("route_id"))
           && (true == jso.contains("device_id"))
           && (true == jso.contains("mac_hardware")))
        {

            if((int(101) == jso["car_id"].toInt())
                && (int(101) == jso["route_id"].toInt()))
            {
                device_id = jso["device_id"].toString();
            }
        }
    }


    else if((true == jsoObj.contains("dt")) &&
            (true == jsoObj.contains("err")) &&
            (true == jsoObj.contains("msg")))
    {
        QJsonObject jso = jsoObj["dt"].toObject();
        if(jso["device_id"].toString() == device_id)
        {
            qDebug() << "device_id:" << device_id;
            // Get timestamp
            QDateTime currentTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Asia/Ho_Chi_Minh"));
            timeReceiveReponse = currentTime;

            qDebug() << "timeReceiveReponse:" << QString::number(timeReceiveReponse.toSecsSinceEpoch()).toUtf8();

            latencyNetwork = QString::number(timeReceiveReponse.toMSecsSinceEpoch() - timeSendRequest.toMSecsSinceEpoch()).toUInt();

            qDebug() << "latencyNetwork" << latencyNetwork;
            qDebug() << timeReceiveReponse.toLocalTime().toString(QString("yyyyMMdd"));
            QString messeges = "latencyNetwork: " + QString::number(latencyNetwork) + ",lat: "
                    + _gps_latitude + ",long: " + _gps_longitude;
            recordLatencyTxt(messeges);

            timerRequestCalculateLatency->start();
        }
    }

    else if((true == jsoObj.contains("gps_location")))
    {
        _gps_latitude = jsoObj["latitude"].toString();
        _gps_longitude = jsoObj["longitude"].toString();
    }
}

void recordLatencyNetworkExcelFIle::requestCalculateLatencyTime()
{
    // Chuyen doi form cua nfc
#ifdef nfcDebug
    qDebug() << "request calculate latency time:" << ;
#endif

    // Get timestamp
    QDateTime currentTime = QDateTime::currentDateTimeUtc().toTimeZone(QTimeZone("Asia/Ho_Chi_Minh"));
    timeSendRequest = currentTime;

    qDebug() << "timeSendRequest:" << QString::number(timeSendRequest.toSecsSinceEpoch()).toUtf8();

    QJsonObject jsonObj;
    jsonObj["id_card"] = "00:00:00:8A:74:80:C4";
    jsonObj["id_card_checked"] = QString::number(1);

    QJsonDocument jsonDoc(jsonObj);

    if(false == jsonDoc.isEmpty())
    {
        // stop timer
        timerRequestCalculateLatency->stop();
        // send signal to server and waiting reponse from it
        emit signalRequestCalculateLantencyTime(jsonDoc.toJson().data());
    }
}
