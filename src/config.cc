#include "config.h"

#include <filesystem>

Config Config::loadFromFile(const char* path) {
    if (!std::filesystem::exists(path)) {
        return Config {};
    }

    toml::parse_result result = toml::parse_file(path);
    return loadFromTOML(&result);
}

Config Config::loadFromTOML(toml::table* tbl) {
    Config config{};

    toml::table* config_table = tbl->get_as<toml::table>("config");

    config.installTo = config_table->get_as<std::string>("install_to")->value_or("");
    // software.archiveUrl = tbl->get_as<std::string>("archive_url")->value_or("");

    return config;
}
