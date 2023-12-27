#include "userdatahandler.h"
#include <iostream>
#include <stdexcept>
#include <QDebug>
#include <QList>
#include <filesystem>

std::vector<std::string> getKeys(std::map<std::string, QVariant> map) {
    std::vector<std::string> keys;

    for(auto const& imap: map) {
        keys.push_back(imap.first);
    }

    return keys;
}

UserDataHandler::UserDataHandler() {
    //load();
    std::string path = std::filesystem::current_path().string();
    path += "\\user_data";
    QString qPath(path.c_str());
    //settings.setPath(QSettings::defaultFormat(), QSettings::UserScope, "/user_data");
}

void UserDataHandler::save() {
    qDebug() << "Beginning to save user data...";
    settings.beginGroup("MainWindow");
    for (std::string & key : getKeys(this->settingsMap)) {
        QVariant val;
        try {
            val = this->settingsMap.at(key);
            qDebug() << "Saved value '" << key << "' to be '" << val << "'.";
        } catch (std::out_of_range) {
            std::cerr << "ERROR: could not find value at key '" << key << "'.";
            return;
        }

        settings.setValue(key, val);
    }
    settings.endGroup();
    qDebug() << "User data has finished saving.";
}

void UserDataHandler::load() {
    qDebug() << "Beginning to load user data...";
    QList<QString> keys = settings.allKeys();

    settings.beginGroup("MainWindow");
    for (QString & key : keys) {

        QVariant val = settings.value(key.toStdString());
        if (!val.isNull()) {
            this->settingsMap[key.toStdString()] = val;
            qDebug() << "Loaded value '" << key << "' of value '" << val << "'.";
        } else {
            std::cerr << "ERROR: value is empty in key '" << key.toStdString() << "'";
        }
    }
    settings.endGroup();
}

void UserDataHandler::setValue(std::string name, QVariant value) {
    if (name != "" && !value.isNull()) {
        this->settingsMap[name] = value;
    } else if (value.isNull())  {
        throw std::invalid_argument("value cannot be null.");
    } else if (name == ""){
        throw std::invalid_argument("valueName cannot be an empty string.");
    } else {
        throw std::invalid_argument("Arguments cannot be empty values.");
    }
}

QVariant UserDataHandler::getValue(std::string name) {
    if (name != "") {
        QVariant val = this->settings.value(name);
        return val;
    }
    throw std::invalid_argument("valueName cannot be empty.");
}

QVariant UserDataHandler::getValue(std::string name, QVariant defaultValue) {
    if (name != "") {
        try {
            QVariant val = this->settings.value(name, defaultValue);
            return val;
        } catch (std::out_of_range) {
            std::cerr << "Resorting to default value of '" << name << "'.\n";
            return defaultValue;
        }
    }
    throw std::invalid_argument("valueName cannot be empty.");
}
