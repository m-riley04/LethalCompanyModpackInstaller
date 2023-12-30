#include "installer.h"
#include <filesystem>
#include <QDebug>
#include "appexceptions.h"

//=== CONSTRUCTORS/DESTRUCTORS
Installer::Installer() {}
Installer::Installer(std::string filesDirectory, std::string gameDirectory)
    : filesDirectory(filesDirectory), gameDirectory(gameDirectory) {}
Installer::~Installer() {}

//=== FUNCTIONALITIES
// Installs the modpack
void Installer::install(std::string &filesDirectory, std::string &gameDirectory) {
    // Check if installation directory exists
    qDebug() << "Checking installation folder...";
    if (!std::filesystem::exists(filesDirectory)) {
        throw InstallationFilesNotFoundException();
    }

    // Check if game directory exists
    qDebug() << "Checking game directory...";
    if (!std::filesystem::exists(gameDirectory)) {
        throw GameNotFoundException();
    }

    // Check if BepInEx directory exists
    qDebug() << "Checking for BepInEx...";
    std::string bepinexDirectory = gameDirectory + "\\BepInEx";
    std::filesystem::path bepinexPath(bepinexDirectory);
    if (!std::filesystem::exists(bepinexPath)) {
        throw BepInExNotInstalledException();
    }

    // Find name of zipped folder
    qDebug() << "Finding extracted folder name...";
    std::string installationFolderName;
    std::filesystem::path filesPath(filesDirectory);
    for (auto & entry : std::filesystem::directory_iterator(filesPath)) {
        if (std::filesystem::is_directory(entry)) {
            installationFolderName = entry.path().filename().string();
            break;
        }
    }
    qDebug() << "Extracted folder name found.";

    // Get installation file paths
    std::filesystem::path pluginsInstallation(filesDirectory + "\\" + installationFolderName + "\\plugins");
    std::filesystem::path configInstallation(filesDirectory + "\\" + installationFolderName + "\\config");
    std::filesystem::path patchersInstallation(filesDirectory + "\\" + installationFolderName + "\\patchers");

    // Get destination paths
    std::filesystem::path pluginsDirectory(bepinexDirectory + "\\plugins");
    std::filesystem::path configDirectory(bepinexDirectory + "\\config");
    std::filesystem::path patchersDirectory(bepinexDirectory + "\\patchers");

    // Remove folders if they exist
    if (std::filesystem::exists(pluginsDirectory)) {
        std::filesystem::remove_all(pluginsDirectory);
    }
    if (std::filesystem::exists(patchersDirectory)) {
        std::filesystem::remove_all(patchersDirectory);
    }
    // Remove all files except for BepInEx config
    if (std::filesystem::exists(configDirectory)) {
        std::filesystem::remove_all(configDirectory);
    }

    // Create the deleted/missing folders
    std::filesystem::create_directory(pluginsDirectory);
    std::filesystem::create_directory(patchersDirectory);

    // Move 3 folders into BepInEx directory
    const auto copyOption = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
    qDebug() << "Installing plugins...";
    std::filesystem::copy(pluginsInstallation, pluginsDirectory, copyOption);
    qDebug() << "Installed plugins.";

    qDebug() << "Installing patchers...";
    std::filesystem::copy(patchersInstallation, patchersDirectory, copyOption);
    qDebug() << "Installed patchers.";

    qDebug() << "Installing config...";
    std::filesystem::copy(configInstallation, configDirectory, copyOption);
    qDebug() << "Installed config.";
}

// Installs the BepInEx mod dependency
void Installer::installBepInEx(std::string &filesDirectory, std::string &gameDirectory) {
    // Check if installation directory exists
    if (!std::filesystem::exists(filesDirectory)) {
        throw InstallationFilesNotFoundException();
    }

    // Check if game directory exists
    if (!std::filesystem::exists(gameDirectory)) {
        throw GameNotFoundException();
    }

    try {
        // Move the BepInEx files into the game files
        std::filesystem::path bepinexInstallation(filesDirectory + "\\BepInExPack");
        //for (auto & entry : std::filesystem::directory_iterator(bepinexInstallation)) {
        // Copy file into game directory
        const auto copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
        std::filesystem::copy(bepinexInstallation, gameDirectory, copyOptions);
        //}
    } catch (std::filesystem::filesystem_error & e) {
        qDebug() << "Error: " << e.what() << '\n';
        throw BepInExInstallationError();
    } catch (...) {
        throw BepInExInstallationError();
    }

}

// Uninstalls the modpack by removing the associated folders/files
void Installer::uninstall(std::string &gameDirectory) {
    // Check if game directory exists
    if (!std::filesystem::exists(std::filesystem::path(gameDirectory))) {
        throw GameNotFoundException();
    }

    // Remove the BepInEx folder
    std::filesystem::path bepinexDirectory(gameDirectory + "\\BepInEx");
    std::filesystem::remove_all(bepinexDirectory);
}

//=== SLOTS
void Installer::doInstall() {
    install(filesDirectory, gameDirectory);
    onInstallFinished();
}
void Installer::doInstallBepInEx() {
    installBepInEx(filesDirectory, gameDirectory);
    onInstallBepInExFinished();
}
void Installer::doUninstall() {
    uninstall(gameDirectory);
    onUninstallFinished();
}
void Installer::onInstallFinished() {


    emit installFinished();
}
void Installer::onInstallBepInExFinished() {

    emit installBepInExFinished();
}
void Installer::onUninstallFinished() {


    emit uninstallFinished();
}
//=== GETTERS/SETTERS
void Installer::setFilesDirectory(std::string directory) {
    filesDirectory = directory;
}

void Installer::setGameDirectory(std::string directory) {
    gameDirectory = directory;
}
