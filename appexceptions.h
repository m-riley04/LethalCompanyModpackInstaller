#ifndef APPEXCEPTIONS_H
#define APPEXCEPTIONS_H

#include <exception>

class GameNotFoundException : public std::exception
{
public:
    const char * what() const noexcept override;
};

class InstallationFilesNotFoundException : public std::exception
{
public:
    const char * what() const noexcept override;
};

class BepInExNotInstalledException : public std::exception
{
public:
    const char * what() const noexcept override;
};

#endif // APPEXCEPTIONS_H
