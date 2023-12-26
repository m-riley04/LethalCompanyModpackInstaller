#ifndef ZIPHANDLER_H
#define ZIPHANDLER_H
#include <string>

class ZipHandler
{
public:
    ZipHandler();

    static int extract(std::string &filePath, std::string &targetPath);
    static std::string sanitizeFilename(std::string& filename);
    static bool isPathTooLong(const std::string & path);
};

#endif // ZIPHANDLER_H
