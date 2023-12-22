#include "installer.h"
#include <filesystem>
#include <QDebug>
#include "appexceptions.h"

//=== CONSTRUCTORS
Installer::Installer() {}

//=== FUNCTIONALITIES
// Installs the modpack
void Installer::install(std::string &filesDirectory, std::string &gameDirectory) {
    // Check if installation directory exists
    if (!std::filesystem::exists(std::filesystem::path(filesDirectory))) {
        throw InstallationFilesNotFoundException();
    }

    // Check if game directory exists
    if (!std::filesystem::exists(std::filesystem::path(gameDirectory))) {
        throw GameNotFoundException();
    }
}

// Installs the BepInEx mod dependency
void Installer::installBepInEx(std::string &filesDirectory, std::string &gameDirectory) {
    // Check if installation directory exists
    if (!std::filesystem::exists(std::filesystem::path(filesDirectory))) {
        throw InstallationFilesNotFoundException();
    }

    // Check if game directory exists
    if (!std::filesystem::exists(std::filesystem::path(gameDirectory))) {
        throw GameNotFoundException();
    }

    // Move the BepInEx files into the game files


    // Run the game once


    // Close the game


}

// Uninstalls the modpack by removing the associated folders/files
void Installer::uninstall(std::string &gameDirectory) {
    // Check if game directory exists
    if (!std::filesystem::exists(std::filesystem::path(gameDirectory))) {
        throw GameNotFoundException();
    }

    // Remove the BepInEx folder
}
