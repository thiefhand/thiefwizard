#include "sw_list.h"

#include <thread>

#include "glad/gl.h"
#include "imgui.h"

#include "app.h"
#include "ims_icons.h"
#include "sw_config.h"

// Simple helper function to load an image into a OpenGL texture with common settings
static bool LoadTextureFromMemory(const char* path, GLuint* out_texture, uint32_t* out_width, uint32_t* out_height) {
    // Load from file
    SDL_Surface* surface = SDL_LoadPNG(path);
    if (surface == NULL) return false;

    uint32_t width = surface->w;
    uint32_t height = surface->h;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Upload pixels into texture
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

    SDL_DestroySurface(surface);

    *out_texture = image_texture;
    *out_width = width;
    *out_height = height;

    return true;
}

SWList::SWList(App* app) : Window::Window(app) {
    bool success = LoadTextureFromMemory("res/thiefhand_icon_transparent.png", &m_logo_icon_texture, &m_logo_icon_width,
                                         &m_logo_icon_height);
    if (!success) {
        printf("Failed to load icon resource.\n");
        m_wants_close = true;
    }

    /* Populate software list. */
    swConfig = SoftwareConfig::loadFromFile("software.toml");
    swConfig.downloadMetas();

    /* Load up install database. */
    installDb = InstallDB::loadFromFile("install_db.toml");
}

static void installSoftware(SWList* list, Software sw, Meta meta) {
    list->workStatus = WorkStatus::DOWNLOADING;
    std::string archivePath = sw.downloadArchive(list->getApp()->config.installTo);

    list->workStatus = WorkStatus::INSTALLING;
    std::string exePath = sw.installArchive(list->getApp()->config.installTo, archivePath, meta);

    /* Register installation. */
    list->installDbMutex.lock();
    {
        Installation* install = list->installDb.findInstallation(meta.name);
        if (install == nullptr) {
            /* Create a new entry if we're not replacing an existing one. */
            install = &list->installDb.installs.emplace_back();
        }

        install->name = meta.name;
        install->exePath = exePath;
        install->version = meta.version;

        list->installDb.saveToFile("install_db.toml");
    }
    list->installDbMutex.unlock();

    list->workStatus = WorkStatus::NOT_WORKING;
}

static void uninstallSoftware(SWList* list, Software sw, Meta meta) {
    list->workStatus = WorkStatus::UNINSTALLING;

    sw.uninstall(list->getApp()->config.installTo);

    /* Register installation. */
    list->installDbMutex.lock();
    {
        list->installDb.removeInstallation(meta.name);

        list->installDb.saveToFile("install_db.toml");
    }
    list->installDbMutex.unlock();

    list->workStatus = WorkStatus::NOT_WORKING;
}

void SWList::installSoftwareAsync(Software sw, Meta meta) {
    if (m_work_thread.joinable()) m_work_thread.join();
    m_work_thread = std::thread(installSoftware, this, sw, meta);
}

void SWList::uninstallSoftwareAsync(Software sw, Meta meta) {
    if (m_work_thread.joinable()) m_work_thread.join();
    m_work_thread = std::thread(uninstallSoftware, this, sw, meta);
}

void SWList::do_gui() {
    /* Wait for install DB to be accessible. */
    installDbMutex.lock();

    /* Header */
    {
        ImGui::Image(m_logo_icon_texture, ImVec2(64, 64));
        ImGui::SameLine();
        ImGui::BeginGroup();
        {
            ImGui::TextColored(ImColor(255, 128, 0), "LOGAN VERSION");
            ImGui::Text(ICON_MS_MAGIC_BUTTON " ThiefWizard by Thiefhand");
            ImGui::Text("(c) 2026-");
            ImGui::Text("by Angus Goucher");

            switch (workStatus) {
                case WorkStatus::DOWNLOADING:
                    ImGui::TextColored(ImColor(128, 255, 128), "Downloading...");
                    break;

                case WorkStatus::INSTALLING:
                    ImGui::TextColored(ImColor(128, 128, 255), "Installing...");
                    break;

                case WorkStatus::UNINSTALLING:
                    ImGui::TextColored(ImColor(255, 180, 255), "Uninstalling...");
                    break;

                default:
                break;
            }
        }
        ImGui::EndGroup();

        ImGui::Separator();
    }

    /* List */
    {
        if (ImGui::BeginTable("Software List", 2,
                              ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchProp)) {
            ImGui::TableSetupColumn("Software Name");
            ImGui::TableSetupColumn("Installed", 0, 0.5f);
            ImGui::TableHeadersRow();

            for (Software sw : swConfig.software) {
                bool installed = installDb.isInstalled(sw.name);
                Meta meta = swConfig.findMeta(sw.name);

                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                if (installed) {
                    Installation* install = installDb.findInstallation(sw.name);

                    if (ImGui::Selectable(sw.name.c_str(), false, 0)) {
                        install->execute();
                    }
                    ImGui::SetItemTooltip("%s\n%s", meta.description.c_str(), ICON_MS_PLAY_ARROW " Click to run.");
                }
                else {
                    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(150, 150, 150, 255));
                    {
                        ImGui::BeginDisabled(workStatus != WorkStatus::NOT_WORKING);
                        if (ImGui::Selectable(sw.name.c_str(), false, 0)) {
                            installSoftwareAsync(sw, meta);
                        }
                        ImGui::EndDisabled();
                    }
                    ImGui::PopStyleColor();
                    ImGui::SetItemTooltip("%s\n%s", meta.description.c_str(),
                                          ICON_MS_HIGHLIGHT_MOUSE_CURSOR " Click to install.");
                }

                ImGui::TableSetColumnIndex(1);
                if (installed) {
                    Installation* install = installDb.findInstallation(sw.name);
                    bool behind = install->isBehind(meta.version);

                    ImGui::BeginDisabled(workStatus != WorkStatus::NOT_WORKING);
                    if (ImGui::SmallButton(ICON_MS_REMOVE)) {
                        uninstallSoftwareAsync(sw, meta);
                    }
                    ImGui::EndDisabled();
                    
                    ImGui::SameLine();

                    if (behind) {
                        ImGui::BeginDisabled(workStatus != WorkStatus::NOT_WORKING);
                        if (ImGui::SmallButton(ICON_MS_UPGRADE)) {
                            uninstallSoftwareAsync(sw, meta);
                            installSoftwareAsync(sw, meta);
                        }
                        ImGui::EndDisabled();
                        ImGui::SameLine();
                        ImGui::TextColored(ImColor(255, 255, 128, 255), ICON_MS_EXCLAMATION " %s < %s",
                                           install->version.c_str(), meta.version.c_str());
                    }
                    else {
                        ImGui::Text(ICON_MS_CHECK " %s", install->version.c_str());
                    }
                }
                else {
                    ImGui::TextColored(ImColor(150, 150, 150, 255), ICON_MS_CLOSE " %s", meta.version.c_str());
                }
            }

            ImGui::EndTable();
        }
    }

    /* Return install DB access. */
    installDbMutex.unlock();
}

bool SWList::wants_close() { return m_wants_close; }
