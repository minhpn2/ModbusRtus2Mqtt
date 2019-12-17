#include "nfcsender.h"

nfcsender::nfcsender(quint16 qrpid, quint16 qrvid)
{
    // Initialize NFC
    readHidQrcode = new DeviceManager();
    connect(readHidQrcode, SIGNAL(onMifare(const QString)),
            this,SLOT(nfcDataChanged(const QString)),Qt::QueuedConnection);

    readHidQrcode->value_pid = qrpid;
    readHidQrcode->value_vid = qrvid;
    readHidQrcode->start();

#ifdef nfcDebug
    qDebug() << "HID Initialized!!!";
#endif
}

void nfcsender::nfcDataChanged(const QString &nfc)
{
#ifdef nfcDebug
    qDebug() << "HID QR data:" << nfc;
#endif
    if(!nfc.isEmpty())
    {
#ifdef nfcDebug
        qDebug() << nfc;
#endif

        emit nfcDataUpdated(nfcEncode(nfc));
    }
}

QString nfcsender::nfcEncode(QString nfcData)
{
    // Chuyen doi sang ma Hex
    QString result = QString::number(nfcData.toUInt(), 16).toUpper();
#ifdef nfcDebug
    qDebug() << "result:" << result;
#endif

    /*  Thuc hien ma hoa chuoi data cua nfc
        Solution:
        - Cắt hai kí tự liên tiếp --> lưu vào một thanh ghi để lưu trữ
        - Đổi thứ tự của các kí tự để mã hóa đúng như yêu cầu
        Explain:
        - C480748A là chuổi Nfc module đọc được
        --> Phải mã hóa thành 8A:74:80:C4
    */
    QString resultEcode;
    for(int position = 0; position < result.size(); position ++)
    {
        if(0 == position%2) {
            resultEcode.insert(0, QString(result.mid(position, 2)));
            if(position < (result.size() - 2)) {
                resultEcode.insert(0, ":");
            }
        }
    }

    resultEcode =resultEcode.insert(0, "00:00:00:");
    /*
     * 20 là chiều dài tối đa của chuổi mã hóa
     * Example:
     *  -   04:68:40:DA:FA:43:80
     * Sau khi chèn thêm chuổi 00:00:00:
     * ==> 00:00:00:04:68:40:DA:FA:43:80
     * ==> Chiều dài của chuổi lúc này là 29
     * Vậy là phải cắt từ vị trí thứ 9 và chiều dài chuổi cần lấy
     * là 20.
     * -----------------------
     * String:  00:00:00:04:68:40:DA:FA:43:80
     * Number:  0123456789----------------->28
     * Size: 29
     */
    if(resultEcode.size() > 20)
    {
        QString lastResultEcode = resultEcode.mid((resultEcode.size() - 20) , int(20));
#ifdef nfcDebug
    qDebug() << "lastResultEcode:" << lastResultEcode;
#endif
        return lastResultEcode;
    }
#ifdef nfcDebug
    qDebug() << "result:" << resultEcode;
#endif

    return resultEcode;
}
