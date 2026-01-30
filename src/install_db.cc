#include "install_db.h"

#include <filesystem>

InstallDB InstallDB::loadFromFile(const char* path) {
    if (!std::filesystem::exists(path)) {
        return InstallDB {};
    }

    toml::parse_result result = toml::parse_file(path);
    return loadFromTOML(&result);
}

InstallDB InstallDB::loadFromTOML(toml::table* tbl) {
    InstallDB db{};

    auto array = (*tbl).get_as<toml::array>("installation");

    if (array != NULL) {
        for (auto& element : *(array->as_array())) {
            toml::table* installTable = element.as_table();

            db.installs.push_back(Installation::loadFromTOML(installTable));
        }
    }

    return db;
}

void InstallDB::saveToFile(const char* path) {
    /* Generate TOML. */
    toml::table outputTbl = {};
    toml::array installsArr = {};

    for (Installation install : installs) {
        installsArr.push_back(install.saveToTOML());
    }

    outputTbl.emplace("installation", installsArr);

    /* Convert TOML to string. */
    std::stringstream ss;
    ss << toml::toml_formatter { outputTbl };
    std::string tomlStr = ss.str();

    /* Write to file. */
    FILE* outputFile = fopen(path, "wb");
    fwrite(tomlStr.c_str(), sizeof(char), tomlStr.size(), outputFile);
    fclose(outputFile);
}

bool InstallDB::isInstalled(std::string& swName) {
    for (Installation install : installs) {
        if (install.name == swName) {
            return true;
        }
    }

    return false;
}

Installation* InstallDB::findInstallation(std::string& name) {
    for (Installation& install : installs) {
        if (install.name == name) {
            return &install;
        }
    }

    return nullptr;
}

void InstallDB::removeInstallation(std::string& name) {
    for (auto i = installs.begin(); i != installs.end(); ++i) {
        if ((*i).name == name) {
            installs.erase(i);
            return;
        }
    }
}
