#ifndef INSTALLER_H
#define INSTALLER_H

#include <string>

class Installer
{
public:
    Installer();

    //=== FUNCTIONALITIES
    void install(std::string &filesDirectory, std::string &gameDirectory);
    void installBepInEx(std::string &filesDirectory, std::string &gameDirectory);
    void uninstall(std::string &gameDirectory);

    //=== GETTERS
    int getInstallSize();

private:
    int installSize;
};

#endif // INSTALLER_H
