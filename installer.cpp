#include "installer.h"

Installer::Installer() {}


//=== FUNCTIONALITIES
// Updates the modpack
void Installer::update() {
    // Delete the current modpack files

    // Download the latest version of the modpack

    // Install the latest version of the modpack

}
// Installs the modpack
void Installer::install() {
    // Locate the installation location for Lethal Company

    // Grab the downloaded modpack

    //
}
// Uninstalls the modpack by removing the associated folders/files
void Installer::uninstall() {
    // Remove the BepInEx folder

}
// Enables the modpack
void Installer::enable() {

}
// Disables the modpack
void Installer::disable() {

}

// Finds the game's installation directory
std::string Installer::findGameInstallation() {
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
            gameLocation = gamePath.string();
            gameDrive = drive;
            gameDrive += ":\\";
            return gamePath.string();
        }

        // Otherwise, try again
        gamePath.clear();
    }

    throw GameNotFoundException();
}

//=== STATUS
// Returns whether the modpack is updated to the latest release or not
bool Installer::isUpdated() {
    // Get the latest version number
    std::string latestVersion;
    // TODO - Get latest version number

    // Compare it to the current version
    if (latestVersion != this->version) { return false; }
    return true;

}

//=== GETTERS
// Returns the current version
std::string Installer::getVersion() { return this->version; }

//=== SETTERS
// Sets the current version
void Installer::setVersion(std::string version) { this->version = version; }

