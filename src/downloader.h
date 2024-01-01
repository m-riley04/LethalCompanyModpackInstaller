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
    void downloadJson(std::string &url, std::string &output, std::string name);
    QByteArray &downloadByteData(std::string &url);
    QByteArray &downloadJSONData(std::string &url);

    QNetworkAccessManager& getWebController();

signals:
    void downloadFinished(const QByteArray& data);
    void downloadProgress(int bytesReceived, int bytesTotal);
    void downloadError(QString errorString);

public slots:
    void onDownloadFinished(QNetworkReply * reply);
    void onDownloadJsonFinished(QNetworkReply * reply);
    void doDownload();
    void doDownloadJson();

private:
    QNetworkAccessManager webController;
    QByteArray data;
    std::string url;
    std::string output;
    std::string name;

    bool saveToDisk(QByteArray &data, std::string &filename, std::string &path, std::string extension);
};

#endif // DOWNLOADER_H
