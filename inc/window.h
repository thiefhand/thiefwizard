#pragma once

class App;

class Window {
   public:
    explicit Window(App* app);
    Window(const Window&) = delete;

    Window& operator=(const Window&) = delete;

    virtual const char* get_name() = 0;
    virtual void do_gui() = 0;
    virtual bool wants_close() = 0;

    App* getApp();

   protected:
    App* m_app;
};