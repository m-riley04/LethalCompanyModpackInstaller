#include "downloader.h"

Downloader::Downloader() {}

// Downloads a url to a given output path
void Downloader::download(std::string &url, std::string &output) {
    // Download the byte data
    QByteArray data = this->downloadByteData(url);

    // Save the byte data to the disk
    std::string name = "downloaded_file";
    this->saveToDisk(data, name, output);
}

// Saves given byte data to a given filename and path. Returns true if writing is successful.
bool saveToDisk(QByteArray &data, std::string &filename, std::string &path) {
    std::string combinedString  = path + "\\" + filename;
    QFile file(QString(combinedString.c_str()));

    // Check for writing permissions
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    // Write data to file
    file.write(data);
    file.close();

    return true;

}

QByteArray Downloader::downloadByteData(std::string &url) {
    QNetworkRequest request(QUrl(url.c_str()));
    QNetworkReply * reply = webController.get(request);
    QByteArray data = reply->readAll();

    return data;
}


