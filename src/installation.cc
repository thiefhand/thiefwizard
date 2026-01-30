#include "installation.h"

#include <string>

Installation Installation::loadFromTOML(toml::table* tbl) {
    Installation install{};

    install.name = tbl->get_as<std::string>("name")->value_or("??");
    install.exePath = tbl->get_as<std::string>("exe_path")->value_or("");
    install.version = tbl->get_as<std::string>("version")->value_or("");

    return install;
}

toml::table Installation::saveToTOML() {
    toml::table tbl = toml::table{};

    tbl.emplace("name", name);
    tbl.emplace("exe_path", exePath);
    tbl.emplace("version", version);

    return tbl;
}

void Installation::execute() { std::system(exePath.c_str()); }

static int evalVersion(std::string versionStr) {
    std::stringstream ss{ versionStr };
    int value = 0;
    int power = 100;
    for (std::string token; std::getline(ss, token, '.');) {
        int i = std::stoi(token);
        value += i * power;

        power /= 10;
    }
    return value;
}

bool Installation::isBehind(std::string versionStr) {
    return evalVersion(version) < evalVersion(versionStr);
}
