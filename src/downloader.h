#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QUrl>
#include <QByteArray>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrlQuery>
#include <QIODevice>

class Downloader : public QObject
{
    Q_OBJECT
public:
    Downloader();
    Downloader(std::string url, std::string output, std::string name);
    ~Downloader();

    void download(std::string &url, std::string &output, std::string name);
    bool saveToDisk(QByteArray &data, std::string &filename, std::string &path);
    QByteArray &downloadByteData(std::string &url);
    QByteArray &downloadJSONData(std::string &url);

    QNetworkAccessManager& getWebController();

signals:
    void downloadFinished();
    void downloadProgress(int bytesReceived, int bytesTotal);
    void downloadError(QString errorString);

public slots:
    void doDownload();

private:
    QNetworkAccessManager webController;
    QByteArray data;
    std::string url;
    std::string output;
    std::string name;
};

#endif // DOWNLOADER_H
