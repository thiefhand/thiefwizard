#pragma once

#include <string>

#include "toml++/toml.h"

#include "meta.h"

/* Represents a single piece of software. */
class Software {
    public:
        Software();
        Software(std::string name);

        std::string name;
        std::string archiveUrl;
        std::string metaUrl;

        std::string downloadArchive(std::string& workingDir);
        std::string installArchive(std::string& installDir, std::string& archivePath, Meta meta);
        void uninstall(std::string& installDir);

        static Software loadFromTOML(toml::table* tbl);
};