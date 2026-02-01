#include "installation.h"

#include <filesystem>
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

void Installation::execute() {
    std::filesystem::path parentPath = std::filesystem::path{ exePath }.parent_path();
    std::filesystem::current_path(parentPath);
    std::string exeName = std::filesystem::path{ exePath }.filename().string();

    printf("CWD: %s\n", std::filesystem::current_path().c_str());
    printf("Running: %s\n", exeName.c_str());

    #ifdef linux
    std::system(("./" + exeName).c_str());
    #endif

    #ifdef _WIN32
    std::system(exeName.c_str());
    #endif
}

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

bool Installation::isBehind(std::string versionStr) { return evalVersion(version) < evalVersion(versionStr); }
