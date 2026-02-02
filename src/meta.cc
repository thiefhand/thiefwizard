#include "meta.h"

#include "curl/curl.h"

Meta Meta::loadFromTOML(toml::table* tbl) {
    Meta meta{};

    toml::table* meta_table = tbl->get_as<toml::table>("meta");

    meta.name = meta_table->get_as<std::string>("name")->value_or("???");
    meta.description = meta_table->get_as<std::string>("description")->value_or("???");
    meta.version = meta_table->get_as<std::string>("version")->value_or("?");
    meta.exePath = meta_table->get_as<std::string>("exe_path")->value_or("");

    return meta;
}

struct MemoryStruct {
    void* memory;
    size_t size;
};

static size_t write_to_memory(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = (struct MemoryStruct*)userp;

    void* ptr = realloc(mem->memory, mem->size + realsize + 1);
    if (!ptr) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(((char*)mem->memory)[mem->size]), contents, realsize);
    mem->size += realsize;
    ((char*)mem->memory)[mem->size] = 0;

    return realsize;
}

Meta Meta::download(std::string& url) {
    printf("Downloading metadata from '%s'...\n", url.c_str());

    /* Download a target to [target].tar. */
    MemoryStruct mem{};
    mem.memory = malloc(1);
    mem.size = 0;

    CURL* curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_to_memory);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mem);

    CURLcode result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        fprintf(stderr, "Metadata download failed: %s\n", curl_easy_strerror(result));
        curl_easy_cleanup(curl);
        return Meta {};
    }

    curl_easy_cleanup(curl);

    toml::table tbl = toml::parse((char*)mem.memory);

    Meta meta = Meta::loadFromTOML(&tbl);

    return meta;
}