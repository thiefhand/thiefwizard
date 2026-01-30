#pragma once

#include <vector>

#include <SDL3/SDL.h>

#include "config.h"
#include "window.h"

class App {
   public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;

    /* Interface */
    void run();
    bool is_open();

    void queue_open_window(Window* window);
    void open_window(Window* window);
    template <typename T>
    void open_window();
    void close_window(Window* window);

    Config config;

   private:
    bool m_open;
    float m_window_scale;
    SDL_Window* m_window;
    SDL_GLContext m_gl_context;
    std::vector<Window*> m_windows;
    std::vector<Window*> m_windowOpenQueue;

    int make_window();
    void handle_sdl_event(SDL_Event event);
    void draw();
    void destroy_window();

    void init_imgui();
    void do_gui();
    void deinit_imgui();
};

template <typename T>
inline void App::open_window() {
    Window* window = new T(this);
    open_window(window);
}
