#include "maincontroller.h"

maincontroller::maincontroller(QObject *parent) : QObject(parent)
{

}

void maincontroller::start()
{
    // Initialie value
    ConfigSetting* settings = &ConfigSetting::instance();

    /*
     * Setting param for Mqtt
     */
    // Mqtt Local
    QString topic = settings->getMqttTopic();
    QString host = settings->getMqttHost();
    int port = settings->getMqttPort();

    m_mqttconfig = new mqttconfig(host, port);
    m_mqttconfig->setDefaultTopic(topic);
    QStringList listTopics;
    listTopics.append(topic);
    listTopics.append("bus/recallserver");
    m_mqttconfig->setMultiKeyTopic(listTopics);

    m_mqttconfig->connectToBroker();

    // Mqtt Cloud
    QString hostCloud = QString("61.28.235.129");
    QString userCloud = QString("iotbus");
    QString passCloud = QString("$6$kZryfWy1I6Clabq7$2Qt9mCe4HJU7VcrDfqeYLYzAOYnxD+FoAh5dfUNRLTTkJz+75Ks4yypXSHrvjKVEqWptTdAtaxav7kCvVLiaKQ==");
    int portCloud    = int(1883);

    m_mqttCloudConfig = new mqttconfig(hostCloud, portCloud, userCloud, passCloud);
    m_mqttCloudConfig->connectToBroker();

    QStringList listTopicsCloud;
    listTopicsCloud.append("iot/bus/re_create_vng_invoice_response_v1");
    m_mqttCloudConfig->setMultiKeyTopic(listTopicsCloud);

    /*
     * Modbus RTU
     */
    m_modbusconfig = new modbusconfig(this);

    /*
     * latency Time
     */
    int carId = settings->getCarId();
    int routeId = settings->getRouteId();

//    m_recordLatencyNetworkExcelFIle = new recordLatencyNetworkExcelFIle(carId, routeId);

    /*
     * Connect Functions
     */
//    connect(m_mqttCloudConfig, SIGNAL(receiverMessageUpdated(const QByteArray &, const QMqttTopicName &)),
//            m_recordLatencyNetworkExcelFIle, SLOT(receiveReponseFromServer(const QByteArray &, const QMqttTopicName &)));

//    connect(m_mqttconfig, SIGNAL(receiverMessageUpdated(const QByteArray &, const QMqttTopicName &)),
//            m_recordLatencyNetworkExcelFIle, SLOT(receiveReponseFromServer(const QByteArray &, const QMqttTopicName &)));

//    connect(m_recordLatencyNetworkExcelFIle, SIGNAL(signalRequestCalculateLantencyTime(const QByteArray&)),
//            this, SLOT(sendToBroker(const QByteArray&)));
}

void maincontroller::sendToBroker(const QByteArray &message)
{
    // Chuyen doi form cua nfc
#ifdef nfcDebug
    qDebug() << "nfc:" << nfc;
#endif

    if(false == message.isEmpty())
    {
        QString jsonMessage(message);
        QJsonDocument jsd = QJsonDocument::fromJson(jsonMessage.toUtf8());
        QJsonObject   jsoObj = jsd.object();

        QJsonDocument jsonDoc(jsoObj);

        if(m_mqttconfig)
        {
            this->m_mqttconfig->publish(jsonDoc.toJson().data(), jsonDoc.toJson().size());
        }
    }
}

