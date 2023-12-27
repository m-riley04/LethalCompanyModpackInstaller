#include "userdatahandler.h"
#include <iostream>
#include <stdexcept>
#include <QDebug>
#include <QList>
#include <QDir>

// Function that is able to return the keys of a map
template <class X, class Y>
std::vector<X> getKeys(std::map<X, Y> map) {
    // Create return keys vector
    std::vector<X> keys;

    // Iterate through map for all keys
    for(auto const& imap: map) {
        keys.push_back(imap.first);
    }

    // Return the keys
    return keys;
}

UserDataHandler::UserDataHandler() {
    // Set the filepath for the .ini save file
    std::string path = QDir::currentPath().toStdString() + "\\user_data\\data.ini";
    QString qPath(path.c_str());
    settings = new QSettings(qPath, QSettings::IniFormat);
}

void UserDataHandler::save() {
    qDebug() << "Beginning to save user data...";
    settings->beginGroup("MainWindow");
    /*for (std::string & key : getKeys(this->settingsMap)) {
        QVariant val;
        try {
            val = this->settingsMap.at(key);
            qDebug() << "Saved value '" << key << "' to be '" << val << "'.";
        } catch (std::out_of_range) {
            std::cerr << "ERROR: could not find value at key '" << key << "'.";
            return;
        }

        settings->setValue(key, val);
    } */
    settings->endGroup();
    qDebug() << "User data has finished saving.";

    settings->sync();
}

// Sets the value of a given setting name
void UserDataHandler::setValue(std::string name, QVariant value) {
    if (name != "" && !value.isNull()) {
        settings->setValue(name, value);
    } else if (value.isNull())  {
        throw std::invalid_argument("value cannot be null.");
    } else if (name == ""){
        throw std::invalid_argument("valueName cannot be an empty string.");
    } else {
        throw std::invalid_argument("Arguments cannot be empty values.");
    }
}

// Gets the value of a given setting name
QVariant UserDataHandler::getValue(std::string name) {
    // Check if name is empty
    if (name != "") {
        QVariant val = this->settings->value(name);
        return val;
    }
    throw std::invalid_argument("name cannot be empty.");
}

// Gets the value of a given setting name. If it doesn't exist, goes with the given default value.
QVariant UserDataHandler::getValue(std::string name, QVariant defaultValue) {
    // Check if name is empty
    if (name != "") {
        try {
            QVariant val = this->settings->value(name, defaultValue);
            return val;
        } catch (std::out_of_range) {
            std::cerr << "Resorting to default value of '" << name << "'.\n";
            return defaultValue;
        }
    }
    throw std::invalid_argument("name cannot be empty.");
}
