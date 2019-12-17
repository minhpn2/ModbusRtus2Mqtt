#ifndef MQTTSENDERPRIVATE_H
#define MQTTSENDERPRIVATE_H

#include <QCoreApplication>
#include <qmqttclient.h>
#include <QObject>
#include <QString>
#include <QTimer>
#include <QList>

#define SET_TIME_BROKER_CONNECT_CHECKED 3000 //3s

class mqttconfig : public QObject
{
    Q_OBJECT
public:
    explicit mqttconfig(const QString& host, int port, const QString& user = nullptr, const QString& pass = nullptr);

    ~mqttconfig();

    void setDefaultTopic(const QString& topic);

    void setMultiKeyTopic(const QStringList topicList);

    int connectToBroker();

    int publish(void*data, int size);

    int publishToTopic(void*data, int size, QString mqttTopic);

    int clientSubscribe(QString topic);

    void clientPing();

    // Do nothing

signals:
    // Tin hieu thong bao khi nhan duoc messenger
    void receiverMessageUpdated(const QByteArray &message, const QMqttTopicName &topic);

    // Tin hieu thong bao trang thai ket noi voi Broker
    void statusConnectToBroker(bool statusConnection);

private slots:
    void brokerDisconnected();

    void brokerConnected();

    void pingResponse();

    void tryConnectToBroker();

    void receiverMessage(const QByteArray &message, const QMqttTopicName &topic);

    void multiSubribe();

    void clientPublish(QString topic, QString msg);

private:
    // This timer will control time checking connection with MQTT Broker
    QTimer*      _timerCheckStatusConnectToBroker;

    // This param used for subribe to multi topics
    QStringList* _listTopic = nullptr;

    // Information of MQTT Broker
    QMqttClient  mqttClient;
    QString      mqttHost;
    QString      mqttDefaultTopic;
    int          mqttPort;
    QString      mqttUser;
    QString      mqttPass;

    // friend class
    friend class maincontroller;
};

#endif // MQTTSENDERPRIVATE_H
