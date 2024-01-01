#include "downloader.h"
#include <QDebug>
#include <QEventLoop>

Downloader::Downloader() {}

Downloader::Downloader(std::string url, std::string output, std::string name)
    : url(url), output(output), name(name)
{}

Downloader::~Downloader() {
    webController.deleteLater();
}

// Downloads a url to a given output path
void Downloader::download(std::string &url, std::string &output, std::string name = "latest_release") {
    qDebug() << "Chosen URL: '" << url << "'";
    qDebug() << "Preparing to download...";

    // Request to the URL
    QUrl _url(url.c_str());
    QNetworkRequest request(_url);

    // Get the request
    webController.get(request);
}

// Saves given byte data to a given filename and path. Returns true if writing is successful.
bool Downloader::saveToDisk(QByteArray &data, std::string &filename, std::string &path, std::string extension = "") {
    qDebug() << "Preparing to write downloaded data to file...";
    std::string combinedString  = path + "\\" + filename + extension;
    QFile file(QString(combinedString.c_str()));

    // Check for writing permissions
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "Could not write to disk in path: '" << combinedString << "'";
        return false;
    }

    // Write data to file
    file.write(data);
    file.close();

    qDebug() << "File Successfully Written to Path: '" << combinedString << "'";

    return true;

}

QByteArray &Downloader::downloadByteData(std::string &url) {
    QUrl _url(url.c_str());
    QNetworkRequest request(_url);

    QNetworkReply * reply = webController.get(request);
    while (!reply->isFinished()) {
        // Wait until reply is finished
    }

    QByteArray data = reply->readAll();
    reply->deleteLater();

    this->data = data;

    return this->data;
}

QByteArray &Downloader::downloadJSONData(std::string &url) {
    QUrl _url(url.c_str());
    QNetworkRequest request(_url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply * reply = webController.get(request);
    QEventLoop eventLoop;
    //QAbstractSocket::connect(reply, &QNetworkReply::finished, this, &Downloader::downloadFinished);
    QAbstractSocket::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::quit);
    QAbstractSocket::connect(reply, &QNetworkReply::finished, &eventLoop, &QEventLoop::deleteLater);

    eventLoop.exec();

    QByteArray data = reply->readAll();
    reply->deleteLater();

    this->data = data;

    return this->data;
}

//=== SLOTS
void Downloader::onDownloadFinished(QNetworkReply * reply) {
    // Disconnect signals/slots
    disconnect(&webController, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);

    // Check for download errors
    if (reply->error()) {
        qDebug() << "Download error: " << reply->errorString();
        reply->deleteLater();

        emit downloadError(reply->errorString());
        return;
    }

    // Read all the byte data
    QByteArray data = reply->readAll();
    reply->deleteLater();

    // Try to save to the disk
    if (!saveToDisk(data, name, output, ".zip")) {
        qDebug() << "Failed to save data to disk.";

        emit downloadError("Failed to save data to disk.");
    } else {
        qDebug() << "Successfully saved data to disk.";

        // Emit the signal
        emit downloadFinished(data);
    }


}

void Downloader::onDownloadJsonFinished(QNetworkReply * reply) {
    // Disconnect signals/slots
    disconnect(&webController, &QNetworkAccessManager::finished, this, &Downloader::onDownloadJsonFinished);

    // Check for download errors
    if (reply->error()) {
        qDebug() << "Download error: " << reply->errorString();
        reply->deleteLater();

        emit downloadError(reply->errorString());
        return;
    }

    // Read all the byte data
    QByteArray data = reply->readAll();
    reply->deleteLater();

    // Try to save to the disk
    if (!saveToDisk(data, name, output, ".json")) {
        qDebug() << "Failed to save data to disk.";

        emit downloadError("Failed to save data to disk.");
    } else {
        qDebug() << "Successfully saved data to disk.";

        // Emit the signal
        emit downloadFinished(data);
    }


}

void Downloader::doDownload() {
    // Implement connection
    connect(&webController, &QNetworkAccessManager::finished, this, &Downloader::onDownloadFinished);

    // Download
    download(url, output, name);
}

void Downloader::doDownloadJson() {
    // Implement connection
    connect(&webController, &QNetworkAccessManager::finished, this, &Downloader::onDownloadJsonFinished);

    // Download
    download(url, output, name);
}
