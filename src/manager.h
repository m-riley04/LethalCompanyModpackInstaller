#ifndef MANAGER_H
#define MANAGER_H

#include <QObject>
#include "downloader.h"
#include "installer.h"
#include <zip.h>

class Manager : public QObject
{
    Q_OBJECT
public:
    Manager();
    Manager(Manager &m);
    ~Manager();

    //=== FUNCTIONALITIES
    void download();
    void downloadBepInEx();
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
    std::string fetchLatestRelease(std::string owner, std::string repo);
    std::string fetchReleaseDownload(std::string &url);

    //=== STATUS
    bool isUpdated();
    bool isBepInExInstalled();

    //=== GETTERS
    int getSpaceAvailable();
    std::string getVersion();
    Downloader &getDownloader();
    Installer &getInstaller();

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
    void errorOccurred(QString error);

public slots:
    void doDownload();
    void doDownloadBepInEx();
    void doUnzip();
    void doUnzipBepInEx();
    void doInstall();
    void doInstallBepInEx();
    void doUpdate();

private:
    Downloader downloader;
    Installer installer;

    std::string version;
    std::string gameDirectory;
    std::string gameDrive;

    std::string cacheDirectory;
    std::string userDataDirectory;
    std::string logPath;
};

#endif // MANAGER_H
