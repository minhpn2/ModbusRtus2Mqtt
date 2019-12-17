#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "configsetting.h"
#include "mqttconfig.h"
#include "recordlatencynetworkexcelfile.h"
#include "modbusconfig.h"

#include <QTimer>
#include <QObject>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTimer>
#include <QFile>


class maincontroller : public QObject
{
    Q_OBJECT
public:
    explicit maincontroller(QObject *parent = nullptr);

    void start();

    void upgradeListStaff(const QString &pathToFileListStaff);
signals:

public slots:

private slots:
    void sendToBroker(const QByteArray &message);

private:
    mqttconfig  *m_mqttconfig;
    mqttconfig  *m_mqttCloudConfig;

    QStringList *listIdCard;

    QString pathToListStaff;
    QString nameOfFileListStaffLocal;

    QString pathToFileListStaff;

    int qrPid;
    int qrVid;

    recordLatencyNetworkExcelFIle   *m_recordLatencyNetworkExcelFIle;

    modbusconfig    *m_modbusconfig;
};

#endif // MAINCONTROLLER_H
