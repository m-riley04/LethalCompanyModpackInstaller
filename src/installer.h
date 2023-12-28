#ifndef INSTALLER_H
#define INSTALLER_H

#include <QObject>
#include <string>

class Installer : public QObject
{
    Q_OBJECT
public:
    Installer();
    Installer(std::string filesDirectory, std::string gameDirectory);
    ~Installer();

    //=== FUNCTIONALITIES
    static void install(std::string &filesDirectory, std::string &gameDirectory);
    static void installBepInEx(std::string &filesDirectory, std::string &gameDirectory);
    static void uninstall(std::string &gameDirectory);

    //=== GETTERS
    int getInstallSize();

    //=== SETTERS
    void setFilesDirectory(std::string directory);
    void setGameDirectory(std::string directory);

signals:
    void installFinished();
    void uninstallFinished();
    void installError();
    void uninstallError();

public slots:
    void doInstall();
    void doInstallBepInEx();
    void doUninstall();

private:
    int installSize;
    std::string filesDirectory;
    std::string gameDirectory;
};

#endif // INSTALLER_H
