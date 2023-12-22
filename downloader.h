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

class Downloader
{
public:
    Downloader();
    ~Downloader();

    void download(std::string &url, std::string &output, std::string name);
    bool saveToDisk(QByteArray &data, std::string &filename, std::string &path);
    QByteArray &downloadByteData(std::string &url);
    QByteArray &downloadJSONData(std::string &url);

private:
    QNetworkAccessManager webController;
    QByteArray data;
};

#endif // DOWNLOADER_H
