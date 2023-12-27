#include "manager.h"
#include <filesystem>
#include "ziphandler.h"
#include "appexceptions.h"
#include "logger.h"

//=== CONSTRUCTORS/DESTRUCTORS
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

Manager::~Manager() {}

//=== FUNCTIONALITIES
// Grabs the latest release and downloads it to the filesystem
void Manager::download() {
    // Get the latest release URL
    Logger::log("Grabbing latest release URL...", logPath);
    std::string latestReleaseURL = this->fetchLatestRelease("m-riley04", "TheWolfPack");
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

    // Extract the zip file to the cache directory
    Logger::log("Extracting downloaded zip file...", logPath);
    std::string zip = cacheDirectory + "\\" + filename + ".zip";
    std::string output = cacheDirectory + "\\" + filename;
    ZipHandler::extract(zip, output);
    Logger::log("Zip file has been extracted.", logPath);
}

void Manager::downloadBepInEx() {
    std::string bepinexURL = "https://thunderstore.io/package/download/BepInEx/BepInExPack/5.4.2100/";

    // Download the bepinex files
    if (!std::filesystem::exists(std::filesystem::path(cacheDirectory + "\\BepInEx.zip"))) {
        Logger::log("Beginning download...", logPath);
        this->downloader.download(bepinexURL, cacheDirectory, "BepInEx");
        Logger::log("Download finished.", logPath);
    }

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
    installer.installBepInEx(installationFilesDirectory, gameDirectory);
}

// Updates the modpack
void Manager::update() {
    // Download the latest version of the modpack
    Logger::log("Downloading latest modpack version...", logPath);
    download();
    Logger::log("Latest modpack version downloaded.", logPath);

    // Delete the current modpack files
    Logger::log("Clearing current modpack files...", logPath);
    clearPlugins();
    Logger::log("Cleared plugins.", logPath);
    clearConfig();
    Logger::log("Cleared config.", logPath);
    clearPatchers();
    Logger::log("Cleared patchers.", logPath);
    Logger::log("Cleared plugins.", logPath);

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
    Logger::log("Searching through game directory...", logPath);
    for (auto & entry : std::filesystem::directory_iterator(path)) {
        if (entry.path().filename().string() == "BepInEx") {
            Logger::log("'BepInEx' folder found!", logPath);
            return true;
        }
    }

    return false;
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
std::string Manager::fetchLatestRelease(std::string owner = "m-riley04", std::string repo = "TheWolfPack") {
    std::string url = "https://api.github.com/repos/";
    url += owner; url += "/"; url += repo; url += "/releases/latest";
    return url;
}

// Returns a string of the lastest release's zipball download url
std::string Manager::fetchReleaseDownload(std::string &url) {
    Logger::log("Downloading JSON data from GitHub API...", logPath);
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

void Manager::setLogPath(std::string path) {
    this->logPath = path;
}
