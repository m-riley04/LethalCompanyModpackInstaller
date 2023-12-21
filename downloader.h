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

    void download(std::string &url, std::string &output);
    void saveToDisk(QByteArray &data, std::string &filename, std::string &path);
    QByteArray downloadByteData(std::string &url);

private:
    QNetworkAccessManager webController;
};

#endif // DOWNLOADER_H
