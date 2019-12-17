#include "configsetting.h"

ConfigSetting* ConfigSetting::m_instance = nullptr;

ConfigSetting::ConfigSetting(QObject *parent) : QObject(parent)
{
    // Set defaut name of file system
    sets = new QSettings("vng", "vpos");

    // Setting param for NFC module
    sets->beginGroup("option");
    qrpid  = sets->value(QString("valueqrpid"), 0xffff).toInt();
    qrvid  = sets->value(QString("valueqrvid"), 0x0035).toInt();

    // Setting param for connection with Broker
    hostmqtt     = sets->value(QString("hostmqtt"), QString("localhost")).toString();
    portmqtt     = sets->value(QString("portmqtt"), 1883).toInt();
    topicpubnfcidcard     = sets->value(QString("topicsub"), QString("bus/callserver")).toString();

    // Local list's staff
    pathToListStaff = sets->value(QString("pathToListStaff"), QString("/usr/local/qt5pi/vpos/")).toString();
    nameOfFileListStaffUpdate = sets->value(QString("nameOfFileListStaffUpdate"), QString("ListStaffUpdate.txt")).toString();
    nameOfFileListStaffLocal = sets->value(QString("nameOfFileListStaffLocal"), QString("ListStaff.txt")).toString();

    carId = sets->value(QString("carId"), int(100)).toString().toInt();
    routeId = sets->value(QString("routeId"), int(100)).toString().toInt();

    sets->endGroup();
    sets->sync();
}

ConfigSetting& ConfigSetting::instance()
{
    static QMutex mutex;

    if (m_instance == nullptr)
    {
        mutex.lock();

        if (m_instance == nullptr)
        {
            m_instance = new ConfigSetting();
        }

        mutex.unlock();
    }

    return *(m_instance);
}





