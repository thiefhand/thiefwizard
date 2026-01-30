#pragma once

#include <string>

#include "toml++/toml.h"

/* Metadata for a software. */
class Meta {
   public:
    static Meta loadFromTOML(toml::table* tbl);
    static Meta download(std::string& url);

    std::string name;
    std::string description;
    std::string version;
    std::string exePath;
};