#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include <QThread>
#include <QStorageInfo>
#include "downloader.h"
#include "installer.h"
#include <zip.h>

class Manager : public QObject
{
    Q_OBJECT
    QThread thread;
public:
    Manager();
    Manager(Manager &m);
    ~Manager();

    //=== FUNCTIONALITIES
    void download();
    void downloadBepInEx();
    void unzip();
    void unzipBepInEx();
    void install();
    void installBepInEx();
    void update();
    void enable();
    void disable();
    void clearPlugins();
    void clearConfig();
    void clearPatchers();

    //=== FINDERS
    std::string locateGameLocation();

    //=== URL FETCHERS
    std::string fetchLatestReleaseURL(std::string owner = "m-riley04", std::string repo = "TheWolfPack" );
    QJsonDocument fetchLatestRelease(std::string &url);
    std::string fetchReleaseDownload(std::string &url);
    std::string fetchLatestVersion(std::string &url);
    std::string fetchReleaseChangelog(std::string &url);

    //=== STATUS
    bool isUpdated();
    bool isBepInExInstalled();
    bool hasEnoughStorage(std::string path, qint64 bytes);
    qint64 getAvailableStorage(std::string path);

    //=== GETTERS
    std::string getVersion();
    Downloader &getDownloader();
    Installer &getInstaller();
    QJsonDocument& getRelease();
    int getSpaceAvailable();
    int getSpaceTotal();

    //=== SETTERS
    void setVersion(std::string version);
    void setGameDirectory(std::string directory);
    void setLogPath(std::string path);

signals:
    void bepInExDownloaded();
    void bepInExInstalled();
    void bepInExUnzipped();
    void modpackDownloaded();
    void modpackInstalled();
    void modpackUpdated();
    void modpackUnzipped();
    void upToDate();
    void outOfDate();
    void updateDownloaded();
    void updateUnzipped();
    void updateInstalled();
    void errorOccurred(QString error);

public slots:
    void doDownload();
    void doUnzip();
    void doInstall();
    void doUninstall();

    void doDownloadBepInEx();
    void doUnzipBepInEx();
    void doInstallBepInEx();

    void onModpackDownloaded();
    void onModpackUnzipped();
    void onModpackInstalled();

    void onBepInExDownloaded();
    void onBepInExUnzipped();
    void onBepInExInstalled();

    // Updating
    void doUpdateDownload();
    void doUpdateUnzip();
    void doUpdateInstall();
    void onUpdateDownloaded();
    void onUpdateUnzipped();
    void onUpdateInstalled();

    // Fetching
    //void doFetchLatestVersion(QString version);

private:
    Downloader downloader;
    Installer installer;

    QJsonDocument release;
    std::string version;
    std::string gameDirectory;
    std::string gameDrive;

    std::string packUrl;
    std::string cacheDirectory;
    std::string userDataDirectory;
    std::string logPath;
};

#endif // MANAGER_H
