#include "manager.h"
#include <filesystem>
#include "ziphandler.h"
#include "appexceptions.h"

//=== CONSTRUCTORS
Manager::Manager() {
    std::filesystem::path cwd(std::filesystem::current_path());

    // Check if cache directory exists
    std::filesystem::path cachePath = cwd; cachePath /= "cache";
    if (!std::filesystem::exists(cachePath)) {
        std::filesystem::create_directory(cachePath);
    }
    this->cacheDirectory = cachePath.string();

    // Check if user_data directory exists
    std::filesystem::path userDataPath = cwd; userDataPath /= "user_data";
    if (!std::filesystem::exists(userDataPath)) {
        std::filesystem::create_directory(userDataPath);
    }
    this->userDataDirectory = userDataPath.string();
}

//=== FUNCTIONALITIES
void Manager::download() {
    // Get the latest release URL
    qDebug() << "Grabbing latest release URL...";
    std::string latestReleaseURL = this->fetchLatestRelease("m-riley04", "TheWolfPack");
    qDebug() << "Latest Release: " << latestReleaseURL;
    std::string url = this->fetchReleaseDownload(latestReleaseURL);
    qDebug() << "Latest Release Download: " << url;

    // Download the zip file to cache directory
    if (!std::filesystem::exists(std::filesystem::path(cacheDirectory + "\\latest_release.zip"))) {
        this->downloader.download(url, cacheDirectory);
    }

    // Extract the zip file to the cache directory
    qDebug() << "Extracting downloaded zip file...";
    std::string zip = cacheDirectory + "\\latest_release.zip";
    std::string output = cacheDirectory + "\\latest_release";
    ZipHandler::extract(zip, output);
}

void Manager::downloadBepInEx() {
    std::string bepinexURL = "https://thunderstore.io/package/download/BepInEx/BepInExPack/5.4.2100/";

    // Download the bepinex files
    if (!std::filesystem::exists(std::filesystem::path(cacheDirectory + "\\latest_release.zip"))) {
        this->downloader.download(bepinexURL, cacheDirectory);
    }

    // Extract the zip file to the cache directory
    qDebug() << "Extracting downloaded zip file...";
    std::string zip = cacheDirectory + "\\BepInEx.zip";
    std::string output = cacheDirectory + "\\BepInEx";
    ZipHandler::extract(zip, output);
}

void Manager::install() {
    std::string installationFilesDirectory = cacheDirectory + "\\latest_release";
    installer.install(installationFilesDirectory, gameDirectory);
}

void Manager::installBepInEx() {
    std::string installationFilesDirectory = cacheDirectory + "\\latest_release";
    installer.installBepInEx(installationFilesDirectory, gameDirectory);
}

// Updates the modpack
void Manager::update() {
    // Download the latest version of the modpack
    download();

    // Delete the current modpack files
    clearPlugins();
    clearConfig();
    clearPatchers();

    // Install the latest version of the modpack
    install();
}

// Enables the modpack
void Manager::enable() {

}

// Disables the modpack temporarily
void Manager::disable() {

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
    std::string latestVersion;
    // TODO - Get latest version number

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
    for (auto & entry : path) {
        if (entry.filename().string() == "BepInEx") {
            return true;
        }
    }

    return false;
}

//=== FINDERS
// Finds the game's installation directory
std::string Manager::locateGameLocation() {
    // Check common drives
    std::string commonDrives[10] = {"C", "D", "E", "F", "G", "H", "I", "J", "K", "L"};
    std::filesystem::path gamePath;
    std::string gameName = "Lethal Company";

    // Iterate through each drive
    for (auto & drive : commonDrives) {

        std::string pathStr = drive;
        pathStr += ":\\SteamLibrary\\steamapps\\common";
        gamePath /= pathStr;
        gamePath /= gameName;

        qDebug() << gamePath.string();

        // Check if the Steam game exists
        if (std::filesystem::exists(gamePath)) {
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
std::string Manager::fetchLatestRelease(std::string owner = "m-riley04", std::string repo = "TheWolfPack") {
    std::string url = "https://api.github.com/repos/";
    url += owner; url += "/"; url += repo; url += "/releases/latest";
    return url;
}

// Returns a string of the lastest release's zipball download url
std::string Manager::fetchReleaseDownload(std::string &url) {
    QByteArray bytes = downloader.downloadJSONData(url);
    QJsonDocument json = QJsonDocument::fromJson(bytes);
    QJsonObject jsonObj = json.object();

    std::string downloadURL = jsonObj.value(QString("zipball_url")).toString().toStdString();

    return downloadURL;
}

//=== GETTERS
// Returns the current version
std::string Manager::getVersion() { return this->version; }

int Manager::getSpaceAvailable() {
    return std::filesystem::space(std::filesystem::path(gameDrive)).available;
}

//=== SETTERS
void Manager::setVersion(std::string version) {
    this->version = version;
}

void Manager::setGameDirectory(std::string directory) {
    this->gameDirectory = directory;
}
