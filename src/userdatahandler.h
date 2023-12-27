#ifndef USERDATAHANDLER_H
#define USERDATAHANDLER_H

#include <string>
#include <QSettings>
#include <QVariant>
#include <map>

class UserDataHandler
{
public:
    UserDataHandler();

    void save();
    void load();

    void setValue(std::string name, QVariant value);
    QVariant getValue(std::string name);
    QVariant getValue(std::string name, QVariant defaultValue);

private:
    QSettings settings;
    std::map<std::string, QVariant> settingsMap;
};

#endif // USERDATAHANDLER_H
