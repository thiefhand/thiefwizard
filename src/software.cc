#include "software.h"

#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

#include "curl/curl.h"

#include "archive.h"
#include "archive_entry.h"

Software::Software() {}

Software::Software(std::string name) : name(name) {}

size_t write_to_file(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

std::string Software::downloadArchive(std::string& workingDir) {
    /* Download a target to [target].tar. */
    std::string containingDir = workingDir + name + "/";
    std::filesystem::create_directories(containingDir);

    std::string targetFileName = containingDir + name + ".tar";
    FILE* targetFile = fopen(targetFileName.c_str(), "wb");

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, archiveUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_file);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, targetFile);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "Archive download failed: %s\n", curl_easy_strerror(result));
    }

    curl_easy_cleanup(curl);
    fclose(targetFile);

    return targetFileName;
}

std::string Software::installArchive(std::string& installDir, std::string& archivePath, Meta meta) {
    std::string containingDir = installDir + name + "/";

    struct archive* a;
    struct archive_entry* entry;

    a = archive_read_new();
    archive_read_support_filter_all(a);
    archive_read_support_format_all(a);

    archive_read_open_filename(a, archivePath.c_str(), 10240);

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char* pathname = archive_entry_pathname(entry);
        // printf("Pathname: %s\n", pathname);
        
        /* Determine directories needed. */
        std::string fullPath = containingDir + std::string{ pathname };
        std::string parentPath = std::filesystem::path { fullPath }.parent_path().string();
        // printf("Parent Dir: %s\n", parentPath.c_str());
        if (!std::filesystem::exists(parentPath)) {
            std::filesystem::create_directories(parentPath);
            // printf("[created]\n");
        }

        /* Only read if we can open the file. */
        FILE* file = fopen(fullPath.c_str(), "wb");
        if (file) {
            archive_read_data_into_fd(a, fileno(file));
            fclose(file);
        }
    }

    archive_read_free(a);

    /* Return the full path the executable. */
    return containingDir + meta.exePath;
}

void Software::uninstall(std::string& installDir) {
    std::string containingDir = installDir + name + "/";

    std::filesystem::remove_all(containingDir);
}

Software Software::loadFromTOML(toml::table* tbl) {
    Software software;

    software.name = tbl->get_as<std::string>("name")->value_or("??");
    software.archiveUrl = tbl->get_as<std::string>("archive_url")->value_or("");
    software.metaUrl = tbl->get_as<std::string>("meta_url")->value_or("");

    return software;
}
