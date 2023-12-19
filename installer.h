#ifndef INSTALLER_H
#define INSTALLER_H

#include <string>
class GameNotFoundException : public std::exception
{
public:
    const char * what() const noexcept override;
};

class Installer
{
public:
    Installer();

    //=== FUNCTIONALITIES
    void update();
    void install();
    void uninstall();
    void enable();
    void disable();
    std::string findGameInstallation();

    //=== STATUS
    bool isUpdated();

    //=== GETTERS
    std::string getVersion();
    std::string getGameLocation();
    std::string getGameDrive();
    int getInstallSize();
    int getSpaceAvailable();

    //=== SETTERS
    void setVersion(std::string version);

private:
    int installSize;
    std::string version;
    std::string gameLocation;
    std::string gameDrive;
};

#endif // INSTALLER_H
