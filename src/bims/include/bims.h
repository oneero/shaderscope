#pragma once

#include <bims.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bims/imgui_impl_bgfx.h>
#include <bims/imgui_impl_sdl.h>
#include <bims/file-ops.h>

#include "imgui.h"

namespace bims
{
    static bgfx::ShaderHandle createShader(const std::string& shader, const char* name)
    {
        const bgfx::Memory* mem = bgfx::copy(shader.data(), shader.size());
        const bgfx::ShaderHandle handle = bgfx::createShader(mem);
        bgfx::setName(handle, name);
        return handle;
    }

    class Application
    {
    public:
        Application(uint32_t width = 1280, uint32_t height = 768);

        int run(int argc, char** argv);

        uint32_t getWidth() const;
		uint32_t getHeight() const;

        virtual void initialize() {};
        virtual void update() {};
        virtual int shutdown() { return 0; };

    protected:
        SDL_Window* m_window;

        // temporary
        float cam_pitch = 0.0f;
        float cam_yaw = 0.0f;
        float rot_scale = 0.01f;

        int prev_mouse_x = 0;
        int prev_mouse_y = 0;
        bool m_quit = false;

    private:
        uint32_t m_width;
		uint32_t m_height;
    };
}