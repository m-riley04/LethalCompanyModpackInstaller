#include "appexceptions.h"

const char * GameNotFoundException::what() const noexcept {
    return "The game directory was not found.";
}

const char * InstallationFilesNotFoundException::what() const noexcept {
    return "The installation files were not found.";
}
