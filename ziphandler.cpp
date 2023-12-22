#include "ziphandler.h"
#include <filesystem>
#include <zip.h>
#include <iostream>
#include <vector>
#include <fstream>

ZipHandler::ZipHandler() {}

int ZipHandler::extract(std::string &filePath, std::string &targetPath) {
    int err = 0;
    zip* za = zip_open(filePath.c_str(), 0, &err);
    if (za == nullptr) {
        std::cerr << "Error opening archive: " << err << "\n";
        return -1;
    }

    // Get the number of entries in the archive
    zip_int64_t numEntries = zip_get_num_entries(za, 0);

    for (zip_int64_t i = 0; i < numEntries; ++i) {
        // Open zip file index
        zip_file* zf = zip_fopen_index(za, i, 0);
        if (!zf) {
            std::cerr << "Error opening file at index " << i << "\n";
            continue;
        }

        // Get the name of the file
        const char* filename = zip_get_name(za, i, 0);
        if (!filename) {
            zip_fclose(zf);
            continue;
        }

        std::string fullPath = targetPath + "/" + std::string(filename);
        std::filesystem::path path(fullPath);

        // Create directories if they don't exist
        if (path.has_parent_path()) {
            std::filesystem::create_directories(path.parent_path());
        }

        // Check if the path is too long
        if (isPathTooLong(fullPath)) {
            std::cerr << "Error: Path too long for " << fullPath << "\n";
            zip_fclose(zf);
            continue;
        }

        // Open the file
        std::ofstream file(fullPath, std::ios::binary);
        if (!file.is_open() && !std::filesystem::is_directory(path)) {
            std::cerr << "Error opening " << fullPath << "\n";
            zip_fclose(zf);
            continue;
        }

        // Read contents of zip file and write to disk
        std::vector<char> buffer(4096);
        zip_int64_t bytesRead;
        while ((bytesRead = zip_fread(zf, buffer.data(), buffer.size())) > 0) {
            file.write(buffer.data(), bytesRead);
        }

        file.close();
        zip_fclose(zf);
    }

    zip_close(za);
    return 0;
}

bool ZipHandler::isPathTooLong(const std::string& path) {
    const size_t MAX_PATH_LENGTH = 260;  // Windows limit
    return path.length() >= MAX_PATH_LENGTH;
}

std::string ZipHandler::sanitizeFilename(std::string& filename) {
    std::string result = filename;
    std::replace_if(result.begin(), result.end(),
        [](unsigned char c) { return !std::isalnum(c) && c != '.' && c != '_' && c != '-'; },
        '_');
    return result;
}
