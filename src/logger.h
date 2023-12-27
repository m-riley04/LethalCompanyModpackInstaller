#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <fstream>

class Logger
{
public:
    Logger();
    Logger(std::string directory, std::string filename);
    Logger(std::string path);
    ~Logger();

    void log(std::string message, bool includeTime = true);
    static void log(std::string message, std::string path, bool includeTime = true);

private:
    std::string directory;
    std::string filename;
    std::string path;
    std::ofstream file;
};

#endif // LOGGER_H
