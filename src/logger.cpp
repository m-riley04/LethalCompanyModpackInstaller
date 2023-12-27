#include "logger.h"
#include <filesystem>
#include <iostream>
#include <time.h>
#include <qDebug>

// Returns a formatted string of the current time
std::string getCurrentTime() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer [80];
    std::strftime(buffer, 80, "%T", timeinfo);

    return std::string(buffer);
}

// Returns a formatted string of the current date and time
std::string getCurrentDateTime() {
    time_t rawtime;
    struct tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    char buffer [80];
    std::strftime(buffer, 80, "%c", timeinfo);

    return std::string(buffer);
}

// Instantiates the logger and opens the log file
Logger::Logger() :
                    directory(std::filesystem::current_path().string()),
                    filename("log.txt")
{
    path        = directory + "\\" + filename;

    // Create the log file
    file.open(path);

    // Create header
    this->log(getCurrentDateTime(), false);

}

// Instantiates the logger and opens the log file. Throws an invalid_argument exception if the given directory does not exist
Logger::Logger(std::string directory, std::string filename) :
                    directory(directory),
                    filename(filename)
{
    this->path      = directory + "\\" + filename;

    // Create the log file if the directory exists
    if (std::filesystem::exists(directory)) {
        this->file.open(this->path);

        // Create header
        this->log(getCurrentDateTime(), false);

        // Return early
        return;
    }
    throw std::invalid_argument("The given directory does not exist");
}

// Destroys the Logger object and safely closes the file
Logger::~Logger() {
    if (file.is_open()) {
        file.close();
    }
}

// Logs a message to the class' log path
void Logger::log(std::string message, bool includeTime) {
    // Check if file is open. If not, open it
    if (!file.is_open()) {
        file.open(path, std::ios_base::app);
    }

    // Write message to log file
    if (includeTime) {
        file << "[" << getCurrentTime() << "] " << message << "\n";
    } else {
        file << message << "\n";
    }

    // Send message to Qt debugger as well
    qDebug() << message;

    // Close/save it
    file.close();
}

// Logs a message to the end of a given file (from filepath). Will never throw an exception.
void Logger::log(std::string message, std::string path, bool includeTime) {
    if (path != "") {
        try {
            // Open the text file
            std::ofstream file;
            file.open(path, std::ios_base::app);

            // Write message to log file
            if (includeTime) {
                file << "[" << getCurrentTime() << "] " << message << "\n";
            } else {
                file << message << "\n";
            }

            // Send message to Qt debugger as well
            qDebug() << message;

            // Close/save it
            file.close();

            // Exit
            return;
        } catch (...) {
            std::cerr << "ERROR: Unable to log message.";
        }
    }
    std::cerr << "ERROR: log path was blank.";
}
