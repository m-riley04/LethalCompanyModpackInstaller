#include "installer.h"

Installer::Installer() {}


//=== FUNCTIONALITIES
// Updates the modpack
void Installer::update() {

}
// Installs the modpack
void Installer::install() {

}
// Uninstalls the modpack by removing the associated folders/files
void Installer::uninstall() {

}
// Enables the modpack
void Installer::enable() {

}
// Disables the modpack
void Installer::disable() {

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

