#ifndef CONFIGSETTING_H
#define CONFIGSETTING_H

#include <QObject>
#include <QMutex>
#include <QSettings>
#include <QDebug>
#include <QStandardPaths>
#include <QTextStream>

class ConfigSetting : public QObject
{
    Q_OBJECT
public:
    static ConfigSetting& instance();
    explicit ConfigSetting(QObject *parent = nullptr);

    // Send Pid and Vid
    int* getQrPid()
    {
        return &qrpid;
    }

    int* getQrVid()
    {
        return &qrvid;
    }

    const QString& getMqttHost()
    {
        return hostmqtt;
    }

    int getMqttPort()
    {
        return portmqtt;
    }

    const QString& getMqttTopic()
    {
        return topicpubnfcidcard;
    }

    const QString& getPathToListStaff()
    {
        return pathToListStaff;
    }

    const QString& getnameOfFileListStaffUpdate()
    {
        return nameOfFileListStaffUpdate;
    }

    const QString& getnameOfFileListStaffLocal()
    {
        return nameOfFileListStaffLocal;
    }

    const int& getCarId()
    {
        return carId;
    }

    const int& getRouteId()
    {
        return routeId;
    }

private:
    static ConfigSetting *m_instance;

    // Initialize config setting param
    QSettings* sets;

    bool    m_autoSave;

    int qrpid;
    int qrvid;

    QString hostmqtt;
    int     portmqtt;
    QString topicpubnfcidcard;

    // Param for list's staff
    QString pathToListStaff;
    QString nameOfFileListStaffUpdate;
    QString nameOfFileListStaffLocal;

    int carId;
    int routeId;

signals:

public slots:
};

#endif // CONFIGSETTING_H
