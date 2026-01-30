#pragma once

#include "toml++/toml.h"

#include "installation.h"

/* Contains info about presently installed software. */
class InstallDB {
    public:
        static InstallDB loadFromFile(const char* path);
        static InstallDB loadFromTOML(toml::table* tbl);

        std::vector<Installation> installs;

        void saveToFile(const char* path);
        bool isInstalled(std::string& swName);
        Installation* findInstallation(std::string& name);
        void removeInstallation(std::string& name);
};