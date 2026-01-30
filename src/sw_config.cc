#include "sw_config.h"

#include <filesystem>

#include "toml++/toml.hpp"

#include "software.h"

SoftwareConfig::SoftwareConfig() {
}

SoftwareConfig SoftwareConfig::loadFromFile(const char* path) {
    if (!std::filesystem::exists(path)) {
        return SoftwareConfig {};
    }

    toml::parse_result result = toml::parse_file(path);
    return loadFromTOML(&result);
}

SoftwareConfig SoftwareConfig::loadFromTOML(toml::table* tbl) {
    SoftwareConfig config{};

    for (auto& element : *((*tbl)["software"].as_array())) {
        toml::table* softwareTable = element.as_table();
        
        config.software.push_back(Software::loadFromTOML(softwareTable));
    }

    return config;
}

void SoftwareConfig::downloadMetas() {
    for (Software sw : software) {
        metas.push_back(Meta::download(sw.metaUrl));
    }
}

Meta SoftwareConfig::findMeta(std::string& name) { 
    for (Meta meta : metas) {
        if (name == meta.name) {
            return meta;
        }
    }
    
    return {};
 }
