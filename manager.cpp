#include "manager.h"

//=== CONSTRUCTORS
Manager::Manager() {}

//=== FUNCTIONALITIES
void Manager::download() {
    std::string latestReleaseURL = this->fetchLatestRelease("m-riley04", "TheWolfPack");
    std::string url = this->fetchReleaseDownload(latestReleaseURL);

    // Get the
    this->downloader.download(url, gameDirectory);
}

void Manager::downloadBepInEx() {
    std::string bepinexURL = "https://thunderstore.io/package/download/BepInEx/BepInExPack/5.4.2100/";

    // Get the bepinex files
    this->downloader.download(bepinexURL, gameDirectory);
}

void Manager::install() {
    installer.install();

}

// Updates the modpack
void Manager::update() {
    // Delete the current modpack files
    clearPlugins();
    clearConfig();
    clearPatchers();

    // Download the latest version of the modpack


    // Install the latest version of the modpack

}

// Enables the modpack
void Manager::enable() {

}

// Disables the modpack temporarily
void Manager::disable() {

}

// Clears out the plugins folder
void Manager::clearPlugins() {

}

// Clears out the config folder
void Manager::clearConfig() {

}


// Clears out the patchers folder
void Manager::clearPatchers() {

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

    QJsonDocument response = QJsonDocument::fromJson(downloader.downloadByteData(url));
    QJsonObject json = response.object();
    std::string downloadURL = json.value(QString("zipball_url")).toString().toStdString();
    return downloadURL;
}

//=== GETTERS
// Returns the current version
std::string Manager::getVersion() { return this->version; }

int Installer::getSpaceAvailable() {
    return std::filesystem::space(std::filesystem::path(gameDrive)).available;
}

//=== SETTERS
// Sets the current version
void Manager::setVersion(std::string version) { this->version = version; }
