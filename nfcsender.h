#ifndef NFCSENDER_H
#define NFCSENDER_H

#include "configsetting.h"

#include <QTimer>
#include <QObject>
#include <QtDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QTimer>

class nfcsender : public QObject
{
    Q_OBJECT
public:
    explicit nfcsender(quint16 qrpid, quint16 qrvid);

    // Ham giai ma the NFC
    QString nfcEncode(QString nfcData);

signals:
    void nfcDataUpdated(const QString& nfc);

public slots:

private slots:
    void nfcDataChanged(const QString &nfc);

private:
    DeviceManager *readHidQrcode;
};

#endif // NFCSENDER_H
