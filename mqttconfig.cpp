#include "mqttconfig.h"

mqttconfig::mqttconfig(const QString& host, int port, const QString &user, const QString &pass)
{
    // Setting information of broker
    this->mqttHost = host;
    this->mqttPort = port;
    this->mqttUser = user;
    this->mqttPass = pass;

    // Initilize timer
    _timerCheckStatusConnectToBroker = new QTimer(this);
    if(_timerCheckStatusConnectToBroker)
    {
        // Per 3s, call function tryConnectedToBroker will be called
        _timerCheckStatusConnectToBroker->setInterval(SET_TIME_BROKER_CONNECT_CHECKED);

        connect(_timerCheckStatusConnectToBroker, SIGNAL(timeout()),
                this, SLOT(tryConnectToBroker()));
    }
}

mqttconfig::~mqttconfig()
{
    // Do nothing
}

void mqttconfig::setDefaultTopic(const QString& topic)
{
    this->mqttDefaultTopic = topic;
}

int mqttconfig::connectToBroker()
{
    qDebug() << "connect to host: " << mqttHost << ", port: " << mqttPort;

    mqttClient.setHostname(this->mqttHost);
    mqttClient.setPort(quint16(this->mqttPort));
    if((false == mqttUser.isEmpty())
        && (false == mqttPass.isEmpty()))
    {
        mqttClient.setUsername(mqttUser);
        mqttClient.setPassword(mqttPass);
    }

    mqttClient.setProtocolVersion(QMqttClient::ProtocolVersion::MQTT_3_1);

    connect(&mqttClient,&QMqttClient::connected,
            this,&mqttconfig::brokerConnected);
    connect(&mqttClient,&QMqttClient::disconnected,
            this,&mqttconfig::brokerDisconnected);
    connect(&mqttClient,&QMqttClient::messageReceived,
            this,&mqttconfig::receiverMessage);
    connect(&mqttClient,&QMqttClient::pingResponseReceived,
            this,&mqttconfig::pingResponse);

    mqttClient.connectToHost();

    return 0;
}

// operation
int mqttconfig::publish(void*data, int size)
{
    if (size <=0)
        return -1;

    QByteArray qdata((char*) data, size);
    return mqttClient.publish(mqttDefaultTopic, qdata);
}

int mqttconfig::publishToTopic(void*data, int size, QString mqttTopic)
{
    if (size <=0)
        return -1;

    QByteArray qdata((char*) data, size);
    return mqttClient.publish(mqttTopic, qdata);
}

void mqttconfig::clientPublish(QString topic, QString msg)
{
    qDebug() << msg << " - " << topic;
    mqttClient.publish(topic,  msg.toUtf8());
}

int mqttconfig::clientSubscribe(QString topic)
{
    QMqttSubscription* subcribe = mqttClient.subscribe(topic);
    if(!subcribe) return -1;
    return 0;
}

void mqttconfig::clientPing()
{
    mqttClient.requestPing();
}

void mqttconfig::brokerDisconnected()
{
    // Start checking timer when connection broker broken
    _timerCheckStatusConnectToBroker->start();

    // Thong bao mat ket noi toi Broker
    emit statusConnectToBroker(false);
}

void mqttconfig::brokerConnected()
{
    qDebug() << "brokerConnected";

    // try subscribe
    qDebug() << "connected to host: " << mqttHost << ",port: " << mqttPort;

    // Stopping timer check status of connected Broker
    _timerCheckStatusConnectToBroker->stop();

    // Subribe topis
    if(nullptr != _listTopic)
    {
        multiSubribe();
    }

    // Thong bao da ket noi duoc toi Broker
    emit statusConnectToBroker(true);
}

void mqttconfig::pingResponse()
{
    //Debug only
    qDebug() << "pong received";
}

void mqttconfig::receiverMessage(const QByteArray &message, const QMqttTopicName &topic)
{
#ifdef debug_mqttconfig
    qDebug() << "message "<< message << "topic " << topic.name();
#endif
    // Update messenge and topic
    emit receiverMessageUpdated(message, topic);
}

void mqttconfig::tryConnectToBroker()
{
    qDebug() << "Try connect to Broker";
    mqttClient.connectToHost();
}

void mqttconfig::setMultiKeyTopic(const QStringList topicList)
{
    _listTopic = new QStringList();
    if(_listTopic)
    {
        _listTopic->append(topicList);
    }
}

void mqttconfig::multiSubribe()
{
#ifdef debug_mqttconfig
    qDebug() << "Begin subribe all topic";
    qDebug() << "Number of Topic: " << _listTopic->count();
#endif
    if(false == _listTopic->isEmpty())
    {
        for(u_int8_t topic_number = 0; topic_number < _listTopic->count(); topic_number++)
        {
            if(-1 == clientSubscribe(_listTopic->value(topic_number, QString())))
            {
                qDebug() << "Topic Error: " << _listTopic->value(topic_number, QString());
            }
        }
    }
}
