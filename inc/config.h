#pragma once

#include <string>

#include "toml++/toml.h"

/* Represents the local user's settings and choices. */
class Config {
    public:
        static Config loadFromFile(const char* path);
        static Config loadFromTOML(toml::table* tbl);

        std::string installTo = "install/";
};