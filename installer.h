#ifndef INSTALLER_H
#define INSTALLER_H

#include <string>

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

    //=== STATUS
    bool isUpdated();

    //=== GETTERS
    std::string getVersion();

    //=== SETTERS
    void setVersion(std::string version);

private:
    std::string version;
};

#endif // INSTALLER_H
