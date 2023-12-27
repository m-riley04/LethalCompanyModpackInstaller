#ifndef USERDATAHANDLER_H
#define USERDATAHANDLER_H

#include <string>
#include <QSettings>
#include <QVariant>

class UserDataHandler
{
public:
    UserDataHandler();

    void save();

    void setValue(std::string name, QVariant value);
    QVariant getValue(std::string name);
    QVariant getValue(std::string name, QVariant defaultValue);

private:
    QSettings * settings;
};

#endif // USERDATAHANDLER_H
