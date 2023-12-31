#include "manager.h"
#include <filesystem>
#include <QThread>
#include "ziphandler.h"
#include "appexceptions.h"
#include "logger.h"

//=== CONSTRUCTORS/DESTRUCTORS
Manager::Manager() {
    std::filesystem::path cwd(std::filesystem::current_path());

    // Check if cache directory exists
    std::filesystem::path cachePath = cwd;
    cachePath /= "cache";
    if (!std::filesystem::exists(cachePath)) {
        std::filesystem::create_directory(cachePath);
    }
    this->cacheDirectory = cachePath.string();

    // Check if user_data directory exists
    std::filesystem::path userDataPath = cwd;
    userDataPath /= "user_data";
    if (!std::filesystem::exists(userDataPath)) {
        std::filesystem::create_directory(userDataPath);
    }
    this->userDataDirectory = userDataPath.string();
}

// Copy Constructor
Manager::Manager(Manager &m) {
    version             = m.version;
    gameDirectory       = m.gameDirectory;
    gameDrive           = m.gameDrive;
    cacheDirectory      = m.cacheDirectory;
    userDataDirectory   = m.userDataDirectory;
    logPath             = m.logPath;

    // Inhereit from main constructor
    Manager();
}

Manager::~Manager() {
    // Exit the thread safely
    thread.quit();
    thread.wait();
}

//=== FUNCTIONALITIES
// Grabs the latest release and downloads it to the filesystem
void Manager::download() {
    // Get the latest release URL
    Logger::log("Grabbing latest release URL...", logPath);
    std::string latestReleaseURL = this->fetchLatestReleaseURL("m-riley04", "TheWolfPack");
    Logger::log("Latest Release: " + latestReleaseURL, logPath);
    std::string url = this->fetchReleaseDownload(latestReleaseURL);
    Logger::log("Latest Release Download: " + url, logPath);

    // Download the zip file to cache directory
    std::string filename = "latest_release";
    if (!std::filesystem::exists(std::filesystem::path(cacheDirectory + "\\latest_release.zip"))) {
        Logger::log("Beginning download...", logPath);
        this->downloader.download(url, cacheDirectory, filename);
        Logger::log("Download finished.", logPath);
    }
}

void Manager::downloadBepInEx() {
    std::string bepinexURL = "https://thunderstore.io/package/download/BepInEx/BepInExPack/5.4.2100/";

    // Download the bepinex files
    if (!std::filesystem::exists(std::filesystem::path(cacheDirectory + "\\BepInEx.zip"))) {
        Logger::log("Beginning download...", logPath);
        this->downloader.download(bepinexURL, cacheDirectory, "BepInEx");
        Logger::log("Download finished.", logPath);
    }
}

void Manager::unzip() {
    std::string filename = "latest_release";
    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\" + filename + ".zip";
    std::string output = cacheDirectory + "\\" + filename;
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);
}

void Manager::unzipBepInEx() {
    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\BepInEx.zip";
    std::string output = cacheDirectory + "\\BepInEx";
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);
}

void Manager::install() {
    std::string installationFilesDirectory = cacheDirectory + "\\latest_release";
    installer.install(installationFilesDirectory, gameDirectory);
}

void Manager::installBepInEx() {
    std::string installationFilesDirectory = cacheDirectory + "\\BepInEx";
    installer.install(installationFilesDirectory, gameDirectory);
}

// Updates the modpack
void Manager::update() {
    // Download the latest version of the modpack
    Logger::log("Downloading latest modpack version...", logPath);
    download();
    Logger::log("Latest modpack version downloaded.", logPath);

    // Delete the current modpack files
    Logger::log("Clearing current modpack files...", logPath);
    clearPatchers();
    Logger::log("Cleared patchers.", logPath);
    clearPlugins();
    Logger::log("Cleared plugins.", logPath);
    clearConfig();
    Logger::log("Cleared config.", logPath);

    // Install the latest version of the modpack
    Logger::log("Installing latest modpack version...", logPath);
    install();
    Logger::log("Latest modpack version installed.", logPath);
}

// Enables the modpack
void Manager::enable() {
    Logger::log("The modpack has been enabled.", logPath);
}

// Disables the modpack temporarily
void Manager::disable() {
   Logger::log("The modpack has been disabled.", logPath);
}

// Clears out the plugins folder
void Manager::clearPlugins() {
    std::filesystem::path pluginsPath(gameDirectory + "\\BepInEx\\plugins");
    std::filesystem::remove(pluginsPath);
}

// Clears out the config folder
void Manager::clearConfig() {
    std::filesystem::path configPath(gameDirectory + "\\BepInEx\\config");
    std::filesystem::remove(configPath);
}

// Clears out the patchers folder
void Manager::clearPatchers() {
    std::filesystem::path patchersPath(gameDirectory + "\\BepInEx\\patchers");
    std::filesystem::remove(patchersPath);
}

//=== STATUS
// Returns whether the modpack is updated to the latest release or not
bool Manager::isUpdated() {
    // Get the latest version number
    std::string latestVersion = fetchLatestVersion(packUrl);

    // Compare it to the current version
    if (latestVersion != this->version) { return false; }
    return true;

}

// Returns whether BepInEx is installed by searching the game diretory path
bool Manager::isBepInExInstalled() {
    std::filesystem::path path(this->gameDirectory);

    if (!std::filesystem::exists(path)) {
        throw GameNotFoundException();
    }
    Logger::log("Searching through game directory...", logPath);
    for (auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().filename().string() == "BepInEx") {
            Logger::log("BepInEx folder found!", logPath);
            return true;
        }
    }

    return false;
}

// Returns true if there is enough storage for the given path and size in bytes. False otherwise.
bool Manager::hasEnoughStorage(std::string path, qint64 bytes) {
    QStorageInfo info(QString(path.c_str()));

    qint64 bytesAfter = info.bytesAvailable() - bytes;
    if (bytesAfter <= 0) { return false; }

    return true;
}

// Returns the amount of available storage on a path
qint64 Manager::getAvailableStorage(std::string path) {
    QStorageInfo info(QString(path.c_str()));

    return info.bytesAvailable();
}

//=== FINDERS
// Finds the game's installation directory
std::string Manager::locateGameLocation() {
    Logger::log("Locating game directory...", logPath);
    // Check common drives
    std::string commonDrives[10] = {"C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
    std::filesystem::path gamePath;
    std::string gameName = "Lethal Company";

    Logger::log("Iterating through common drives...", logPath);
    // Iterate through each drive
    for (auto & drive : commonDrives) {

        std::string pathStr = drive;
        pathStr += ":\\SteamLibrary\\steamapps\\common";
        gamePath /= pathStr;
        gamePath /= gameName;

        Logger::log("Checking path: " + gamePath.string(), logPath);

        // Check if the Steam game exists
        if (std::filesystem::exists(gamePath)) {
            Logger::log("Path exists!", logPath);
            gameDirectory = gamePath.string();
            gameDrive = drive;
            gameDrive += ":\\";
            return gamePath.string();
        }

        // Otherwise, try again
        gamePath.clear();
    }

    throw GameNotFoundException();
}

//=== URL FETCHERS
// Returns a string of the latest url release
std::string Manager::fetchLatestReleaseURL(std::string owner, std::string repo) {
    std::string url = "https://api.github.com/repos/";
    url += owner; url += "/"; url += repo; url += "/releases/latest";
    packUrl = url;
    return url;
}

// Returns a QJsonDocument of the lastest release
QJsonDocument Manager::fetchLatestRelease(std::string &url) {
    QByteArray bytes = downloader.downloadJSONData(url);
    QJsonDocument json = QJsonDocument::fromJson(bytes);

    release = json;
    return json;
}

// Returns a string of the lastest release's zipball download url
std::string Manager::fetchReleaseDownload(std::string &url) {
    QJsonDocument json = release;
    if (json.isEmpty() || json.isNull()) {
        json = fetchLatestRelease(url);
    }

    std::string downloadURL = json.object().value(QString("zipball_url")).toString().toStdString();

    return downloadURL;
}

// Returns a string of the lastest release's version
std::string Manager::fetchLatestVersion(std::string &url) {
    QJsonDocument json;
    if (json.isEmpty() || json.isNull()) {
        json = fetchLatestRelease(url);
    }

    std::string latestVersion = json.object().value(QString("tag_name")).toString().toStdString();

    return latestVersion;
}

// Returns a string of the latest release's changelog
std::string Manager::fetchReleaseChangelog(std::string &url) {
    QJsonDocument json = release;
    if (json.isEmpty() || json.isNull()) {
        json = fetchLatestRelease(url);
    }

    std::string changelog = json.object().value(QString("body")).toString().toStdString();

    return changelog;
}

//=== SLOTS
void Manager::doDownload() {
    // Get the latest release URL
    Logger::log("Grabbing latest release URL...", logPath);
    std::string latestReleaseURL = this->fetchLatestReleaseURL("m-riley04", "TheWolfPack");
    Logger::log("Latest Release: " + latestReleaseURL, logPath);
    std::string url = this->fetchReleaseDownload(latestReleaseURL);
    Logger::log("Latest Release Download: " + url, logPath);
    std::string filename = "latest_release";

    // Check if file is already in cache
    Logger::log("Beginning download...", logPath);
    if (std::filesystem::exists(cacheDirectory + "\\" + filename + ".zip")) {
        Logger::log("Requested file already downloaded!", logPath);
        onModpackDownloaded();
        return;
    }

    // Implement threading
    Downloader* worker      = new Downloader(url, cacheDirectory, filename);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Downloader::doDownload);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Downloader::downloadFinished, this, &Manager::onModpackDownloaded);

    thread.start();
    Logger::log("Modpack download thread started.", logPath);
}
void Manager::doDownloadBepInEx() {
    // Get the latest release URL
    std::string bepinexURL = "https://thunderstore.io/package/download/BepInEx/BepInExPack/5.4.2100/";
    std::string filename = "BepInEx";

    // Check if file is already in cache
    Logger::log("Beginning download...", logPath);
    if (std::filesystem::exists(cacheDirectory + "\\" + filename + ".zip")) {
        Logger::log("Requested file already downloaded!", logPath);
        onBepInExDownloaded();
        return;
    }

    // Implement threading
    Downloader* worker      = new Downloader(bepinexURL, cacheDirectory, filename);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Downloader::doDownload);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Downloader::downloadFinished, this, &Manager::onBepInExDownloaded);

    thread.start();
    Logger::log("BepInEx download thread started.", logPath);
}
void Manager::doUnzip() {
    std::string filename = "latest_release";
    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\" + filename + ".zip";
    std::string output = cacheDirectory + "\\" + filename;
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);

    onModpackUnzipped();
}
void Manager::doUnzipBepInEx() {
    std::string filename = "BepInEx";
    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\" + filename + ".zip";
    std::string output = cacheDirectory + "\\" + filename;
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);

    onBepInExUnzipped();
}
void Manager::doInstall() {
    std::string installationFilesDirectory = cacheDirectory + "\\latest_release";
    Installer * worker = new Installer(installationFilesDirectory, gameDirectory);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Installer::doInstall);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Installer::installFinished, this, &Manager::onModpackInstalled);
    //connect(worker, &Installer::installFinished, &thread, &QThread::quit);

    thread.start();
    Logger::log("Modpack install thread started.", logPath);
}
void Manager::doInstallBepInEx() {
    std::string installationFilesDirectory = cacheDirectory + "\\BepInEx";
    Installer * worker = new Installer(installationFilesDirectory, gameDirectory);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Installer::doInstallBepInEx);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Installer::installBepInExFinished, this, &Manager::onBepInExInstalled);
    //connect(worker, &Installer::installBepInExFinished, &thread, &QThread::quit);

    thread.start();
    Logger::log("BepInEx install thread started.", logPath);
}
void Manager::doUninstall() {
    installer.doUninstall();
}
void Manager::doUpdateDownload() {
    // Get the latest release URL
    Logger::log("Grabbing latest release URL...", logPath);
    std::string latestReleaseURL = this->fetchLatestReleaseURL("m-riley04", "TheWolfPack");
    Logger::log("Latest Release: " + latestReleaseURL, logPath);
    std::string url = this->fetchReleaseDownload(latestReleaseURL);
    Logger::log("Latest Release Download: " + url, logPath);
    std::string filename = "latest_release";

    // Implement threading
    Downloader* worker      = new Downloader(url, cacheDirectory, filename);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Downloader::doDownload);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Downloader::downloadFinished, this, &Manager::onUpdateDownloaded);

    thread.start();
    Logger::log("Modpack update download thread started.", logPath);
}
void Manager::doUpdateUnzip() {
    std::string filename = "latest_release";
    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\" + filename + ".zip";
    std::string output = cacheDirectory + "\\" + filename;
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);

    onUpdateUnzipped();
}
void Manager::doUpdateInstall() {
    std::string installationFilesDirectory = cacheDirectory + "\\latest_release";
    Installer * worker = new Installer(installationFilesDirectory, gameDirectory);
    worker->moveToThread(&thread);

    connect(&thread, &QThread::started, worker, &Installer::doInstallUpdate);
    connect(&thread, &QThread::finished, worker, &QObject::deleteLater);
    connect(worker, &Installer::installUpdateFinished, this, &Manager::onUpdateInstalled);

    thread.start();
    Logger::log("Modpack update install thread started.", logPath);
}
void Manager::onModpackDownloaded() {
    thread.quit();
    emit modpackDownloaded();
}
void Manager::onBepInExDownloaded() {
    thread.quit();
    emit bepInExDownloaded();
}
void Manager::onModpackUnzipped() {
    emit modpackUnzipped();
}
void Manager::onBepInExUnzipped() {
    emit bepInExUnzipped();
}
void Manager::onModpackInstalled() {
    version = fetchLatestVersion(packUrl);

    thread.quit();
    emit modpackInstalled();
}
void Manager::onBepInExInstalled() {
    thread.quit();
    emit bepInExInstalled();
}
void Manager::onUpdateDownloaded() {
    thread.quit();
    emit updateDownloaded();
}
void Manager::onUpdateUnzipped() {
    emit updateUnzipped();
}
void Manager::onUpdateInstalled() {
    version = fetchLatestVersion(packUrl);

    thread.quit();
    emit updateInstalled();
}

//=== GETTERS
// Returns the current version
std::string Manager::getVersion() { return this->version; }

// Returns a reference to the downloader object
Downloader& Manager::getDownloader() { return this->downloader; }

// Returns a reference to the installer object
Installer& Manager::getInstaller() { return this->installer; }

// Returns a json document of the current release
QJsonDocument& Manager::getRelease() { return this->release;}

// Returns the current space avaliable on the game drive as an integer of bytes
int Manager::getSpaceAvailable() { return std::filesystem::space(std::filesystem::path(gameDrive)).available; }

//=== SETTERS
void Manager::setVersion(std::string version) { this->version = version; }

void Manager::setGameDirectory(std::string directory) { this->gameDirectory = directory; }

void Manager::setLogPath(std::string path) { this->logPath = path; }
