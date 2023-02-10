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

bims::Application::Application(uint32_t width, uint32_t height)
{
    m_width = width;
    m_height = height;
}

int bims::Application::run(int argc, char** argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    const int width = m_width;
    const int height = m_height;
    m_window = SDL_CreateWindow(
        argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width,
        height, SDL_WINDOW_SHOWN);

    if (m_window == nullptr) {
        printf("Window could not be created. SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    if (!SDL_GetWindowWMInfo(m_window, &wmi)) {
        printf(
            "SDL_SysWMinfo could not be retrieved. SDL_Error: %s\n",
            SDL_GetError());
        return 1;
    }
    bgfx::renderFrame(); // single threaded mode

    bgfx::PlatformData pd{};
#if BX_PLATFORM_WINDOWS
    pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_OSX
    pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_LINUX
    pd.ndt = wmi.info.x11.display;
    pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?

    bgfx::Init bgfx_init;
    bgfx_init.type = bgfx::RendererType::Count; // auto choose renderer
    bgfx_init.resolution.width = width;
    bgfx_init.resolution.height = height;
    bgfx_init.resolution.reset = BGFX_RESET_VSYNC;
    bgfx_init.platformData = pd;
    bgfx::init(bgfx_init);

    bgfx::setViewClear(
        0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x000000FF, 1.0f, 0);
    bgfx::setViewRect(0, 0, 0, width, height);

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    ImGui_Implbgfx_Init(255);
#if BX_PLATFORM_WINDOWS
    ImGui_ImplSDL2_InitForD3D(m_window);
#elif BX_PLATFORM_OSX
    ImGui_ImplSDL2_InitForMetal(window);
#elif BX_PLATFORM_LINUX
    ImGui_ImplSDL2_InitForOpenGL(window, nullptr);
#endif // BX_PLATFORM_WINDOWS ? BX_PLATFORM_OSX ? BX_PLATFORM_LINUX ?

    initialize();

    while (!m_quit) {
        for (SDL_Event currentEvent; SDL_PollEvent(&currentEvent) != 0;) {
			ImGui_ImplSDL2_ProcessEvent(&currentEvent);
			if (currentEvent.type == SDL_QUIT) {
				m_quit = true;
				break;
			}
		}

		ImGui_Implbgfx_NewFrame();
		ImGui_ImplSDL2_NewFrame();

		update();

        bgfx::frame();
	}
}

uint32_t bims::Application::getWidth() const
{
	return m_width;
}

uint32_t bims::Application::getHeight() const
{
	return m_height;
}