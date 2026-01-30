#pragma once

#include <atomic>
#include <vector>
#include <thread>
#include <mutex>

#include "SDL3/SDL.h"
#include "glad/gl.h"

#include "install_db.h"
#include "software.h"
#include "sw_config.h"
#include "window.h"

enum class WorkStatus {
    NOT_WORKING,
    DOWNLOADING,
    INSTALLING,
    UNINSTALLING,
};

class SWList : public Window {
   public:
    SWList(App* app);

    SoftwareConfig swConfig;
    std::mutex installDbMutex;
    InstallDB installDb;
    std::atomic<WorkStatus> workStatus = WorkStatus::NOT_WORKING;

    void do_gui();
    const char* get_name() { return "Software List"; }
    bool wants_close();

   private:
    GLuint m_logo_icon_texture;
    uint32_t m_logo_icon_width;
    uint32_t m_logo_icon_height;

    bool m_wants_close = false;
    std::thread m_work_thread;

    // void installSoftware(Software sw, Meta meta);
    // void uninstallSoftware(Software sw, Meta meta);
    void installSoftwareAsync(Software sw, Meta meta);
    void uninstallSoftwareAsync(Software sw, Meta meta);
};