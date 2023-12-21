#ifndef INSTALLER_H
#define INSTALLER_H

#include <string>

class Installer
{
public:
    Installer();

    //=== FUNCTIONALITIES
    void install();
    void uninstall();

    //=== GETTERS
    int getInstallSize();
    std::string getGameLocation();
    std::string getGameDrive();
    int getSpaceAvailable();

private:
    int installSize;
};

#endif // INSTALLER_H
