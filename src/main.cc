#include "app.h"

#include <curl/curl.h>

int main(int argc, const char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_EVENTS);

    CURLcode result = curl_global_init(CURL_GLOBAL_ALL);
    if (result) {
        return result;
    }

    App* app = new App();
    app->run();

    curl_global_cleanup();
    return 0;
}
