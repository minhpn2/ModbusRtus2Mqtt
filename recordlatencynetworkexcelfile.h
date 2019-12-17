#ifndef RECORDLATENCYNETWORKEXCELFILE_H
#define RECORDLATENCYNETWORKEXCELFILE_H

#include <QObject>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QTimer>
#include <qmqttclient.h>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDateTime>
#include <QTimeZone>
#include <QFile>

#define  TIME_REQUEST_CALCULATE_LATENCY 5000 //5 seconds

class recordLatencyNetworkExcelFIle : public QObject
{
    Q_OBJECT
public: 
    explicit recordLatencyNetworkExcelFIle(const int& carId,const int& routeId);
    ~recordLatencyNetworkExcelFIle();

    void openRecordFileTxt();

    void closeRecordFileTxt();

    void recordLatencyTxt(const QString& message);

signals:
    // Send signals to server to calculate latency time
    void signalRequestCalculateLantencyTime(const QByteArray& message);

public slots:
    // Receive messenges from Server
    void receiveReponseFromServer(const QByteArray &message, const QMqttTopicName &topic);

private slots:
    // Request calculate latency time of system
    void requestCalculateLatencyTime();

private:
    /*
     *  Calculate latency by using QTimer and UTC time
     */
    // Timer uses for requesting calculation lantency
    QTimer      *timerRequestCalculateLatency;

    u_int32_t   latencyNetwork;

    bool        waitingReponseFromServer;

    /*
     *  Param use for recording letancy time
     */
    // path to file excel recordLatencyNetworkExcelFile
    QString      pathToRecordLatencyFile;

    // record time before and after send request
    QDateTime       timeSendRequest;
    QDateTime       timeReceiveReponse;

    // Name of file excel
    QString      recordLatencyNetworkExcel;

    QString      m_file_name_log_created;

    QFile        _file_log_created;

    QString      _car_number = QString("3G");

    QString     _gps_latitude;
    QString     _gps_longitude;

    int routeId;
    int carId;

    QString device_id;
};

#endif // RECORDLATENCYNETWORKEXCELFILE_H
