#include "appexceptions.h"

const char * GameNotFoundException::what() const noexcept {
    return "The game directory was not found.";
}

const char * InstallationFilesNotFoundException::what() const noexcept {
    return "The installation files were not found.";
}

const char * BepInExNotInstalledException::what() const noexcept {
    return "BepInEx is not installed.";
}

const char * BepInExInstallationError::what() const noexcept {
    return "There was an issue installing BepInEx.";
}
