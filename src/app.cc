#include "app.h"

#include <stdio.h>
#include <algorithm>

#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_sdl3.h>
#include <glad/gl.h>
#include <imgui.h>
#include "ims_icons.h"

#include "sw_list.h"

#define APP_MAKE_WINDOW_FAILURE 1

App::App() {
    int result = make_window();
    SDL_assert(result == 0 && "Failed to make window!");

    init_imgui();

    open_window(new SWList(this));

    config = Config::loadFromFile("config.toml");
}

void App::run() {
    while (is_open()) {
        /* Handle events */
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_sdl_event(event);
        }

        /* Do GUI */
        do_gui();

        /* Put the GUI on screen */
        draw();

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
            SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
        }
    }
}

bool App::is_open() { return m_open; }

int App::make_window() {
    /* Attributes for window */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    /* Make the window */
    m_window_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_window =
        SDL_CreateWindow("ThiefWizard", 1, 1,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_FULLSCREEN);
    if (m_window == nullptr) {
        printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
        return APP_MAKE_WINDOW_FAILURE;
    }
    m_gl_context = SDL_GL_CreateContext(m_window);
    if (m_gl_context == nullptr) {
        printf("Error: SDL_GL_CreateContext(): %s\n", SDL_GetError());
        return APP_MAKE_WINDOW_FAILURE;
    }
    int version = gladLoadGL(SDL_GL_GetProcAddress);
    if (version == 0) {
        printf("Error: Failed to initialize OpenGL context\n");
        return APP_MAKE_WINDOW_FAILURE;
    }

    SDL_GL_MakeCurrent(m_window, m_gl_context);
    SDL_GL_SetSwapInterval(1);  // Enable vsync
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    SDL_HideWindow(m_window);

    m_open = true;

    return 0;
}

void App::handle_sdl_event(SDL_Event event) {
    ImGui_ImplSDL3_ProcessEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT: {
            m_open = false;
            break;
        }
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED: {
            if (event.window.windowID == SDL_GetWindowID(m_window)) {
                m_open = false;
            }
            break;
        }
    }
}

void App::draw() {
    ImGuiIO& io = ImGui::GetIO();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(m_window);
}

void App::destroy_window() {
    SDL_GL_DestroyContext(m_gl_context);
    SDL_DestroyWindow(m_window);
    SDL_Quit();

    m_open = false;
}

void App::init_imgui() {
    /* ImGui init */
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(m_window_scale);  // Bake a fixed style scale. (until we have a solution for dynamic style
                                          // scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = m_window_scale;  // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this
                                          // unnecessary. We leave both here for documentation purpose)

    ImGui_ImplSDL3_InitForOpenGL(m_window, m_gl_context);
    ImGui_ImplOpenGL3_Init(NULL);

    /* Load and register icon font. */
    io.Fonts->AddFontDefault();

    ImWchar editor_renderer_mdi_icon_ranges[3] = { ICON_MIN_MS, ICON_MAX_MS, 0 };
    auto config = new ImFontConfig();
	config->GlyphOffset = ImVec2(2, 5);
	config->MergeMode = true;
    io.Fonts->AddFontFromFileTTF("res/msi.ttf", 19, config, editor_renderer_mdi_icon_ranges);
}

void App::do_gui() {
    /* Do GUI */
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // ImGui::ShowDemoWindow();

    for (Window* window : m_windows) {
        /* Pressing X on a window will set this to false.*/
        bool open = true;

        /* Generate unique name for window. */
        const char* base_name = window->get_name();
        char unique_title[1024] = { 0 };
        snprintf(unique_title, 1024, "%s##%p", base_name, (void*)window);
        
        /* Set up and render window. */
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);
        ImGui::Begin(unique_title, &open);
        {
            window->do_gui();
        }
        ImGui::End();

        /* Close the window if open was set to false. */
        if (!open || window->wants_close()) {
            close_window(window);
        }
    }

    ImGui::Render();

    /* Move queued windows to actual window list */
    m_windows.insert(m_windows.end(), m_windowOpenQueue.begin(), m_windowOpenQueue.end());
    m_windowOpenQueue.clear();

    /* Close the app if we've closed all windows. */
    if (m_windows.size() <= 0) {
        m_open = false;
    }
}

void App::deinit_imgui() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void App::queue_open_window(Window* window) { m_windowOpenQueue.push_back(window); }

void App::open_window(Window* window) { m_windows.push_back(window); }

void App::close_window(Window* window) {
    m_windows.erase(std::remove(m_windows.begin(), m_windows.end(), window), m_windows.end());
}