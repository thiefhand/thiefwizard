#include "window.h"

Window::Window(App *app) : m_app(app) {}

App* Window::getApp() {
    return m_app;
}