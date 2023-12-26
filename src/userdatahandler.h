#ifndef USERDATAHANDLER_H
#define USERDATAHANDLER_H

#include <string>

class UserDataHandler
{
public:
    UserDataHandler();
    UserDataHandler(std::string directory, std::string filename);

    void save();
    void load();

    void setDirectory();
    void setFilename();
};

#endif // USERDATAHANDLER_H
