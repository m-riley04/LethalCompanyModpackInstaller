#ifndef MANAGER_H
#define MANAGER_H

#include "downloader.h"
#include "installer.h"

class Manager
{
public:
    Manager();

    //=== FUNCTIONALITIES
    void download();
    void downloadBepInEx();
    void install();
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
    std::string getVersion();
    Downloader &getDownloader();
    Installer &getInstaller();

    //=== SETTERS
    void setVersion(std::string version);

private:
    Downloader downloader;
    Installer installer;

    std::string version;
    std::string gameDirectory;
    std::string gameDrive;
};

#endif // MANAGER_H
