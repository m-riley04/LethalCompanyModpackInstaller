#ifndef MANAGER_H
#define MANAGER_H

#include "downloader.h"
#include "installer.h"
#include <zip.h>
#include "logger.h"

class Manager
{
public:
    Manager();
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
