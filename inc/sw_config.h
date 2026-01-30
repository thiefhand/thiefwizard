#pragma once

#include <string>
#include <unordered_map>

#include "toml++/toml.hpp"

#include "software.h"

/* Represents the software that's available. Shouldn't be modified, loaded from TOML. */
class SoftwareConfig {
   public:
    SoftwareConfig();

    static SoftwareConfig loadFromFile(const char* path);
    static SoftwareConfig loadFromTOML(toml::table* tbl);

    void downloadMetas();
    Meta findMeta(std::string& name);

    std::vector<Software> software;
    std::vector<Meta> metas;
};