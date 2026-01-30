#pragma once

#include <string>

#include "toml++/toml.h"

class Installation {
   public:
    static Installation loadFromTOML(toml::table* tbl);

    toml::table saveToTOML();
    void execute();
    bool isBehind(std::string versionStr);

    std::string name;
    std::string exePath;
    std::string version;
};