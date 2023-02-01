#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bims.h>
#include <bims/imgui_impl_bgfx.h>
#include <bims/imgui_impl_sdl.h>
#include <bims/file-ops.h>

#include "imgui.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static PosColorVertex cube_vertices[] = {
	{-1.0f, 1.0f, 1.0f, 0xff000000},   {1.0f, 1.0f, 1.0f, 0xff0000ff},
	{-1.0f, -1.0f, 1.0f, 0xff00ff00},  {1.0f, -1.0f, 1.0f, 0xff00ffff},
	{-1.0f, 1.0f, -1.0f, 0xffff0000},  {1.0f, 1.0f, -1.0f, 0xffff00ff},
	{-1.0f, -1.0f, -1.0f, 0xffffff00}, {1.0f, -1.0f, -1.0f, 0xffffffff},
};

static const uint16_t cube_tri_list[] = {
	0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6,
	1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

class Shaderscope : public bims::Application
{
	void initialize()
	{
		bgfx::VertexLayout pos_col_vert_layout;

		pos_col_vert_layout.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
			.end();

		m_vbh = bgfx::createVertexBuffer(
			bgfx::makeRef(cube_vertices, sizeof(cube_vertices)),
			pos_col_vert_layout);

		m_ibh = bgfx::createIndexBuffer(
			bgfx::makeRef(cube_tri_list, sizeof(cube_tri_list)));

		const std::string shader_root = "shader/build/";

		std::string vshader;
		if (!fileops::read_file(shader_root + "v_simple.bin", vshader)) {
			printf("Could not find shader vertex shader (ensure shaders have been compiled).\n"
			"Run compile-shaders-<platform>.sh/bat\n");
			return;
		}

		std::string fshader;
		if (!fileops::read_file(shader_root + "f_simple.bin", fshader)) {
			printf("Could not find shader fragment shader (ensure shaders have been compiled).\n"
			"Run compile-shaders-<platform>.sh/bat\n");
			return;
		}

		bgfx::ShaderHandle vsh = bims::createShader(vshader, "vshader");
		bgfx::ShaderHandle fsh = bims::createShader(fshader, "fshader");
		m_program = bgfx::createProgram(vsh, fsh, true);
	}

	void update()
	{
		if (!ImGui::GetIO().WantCaptureMouse) {
			// simple input code for orbit camera
			int mouse_x, mouse_y;
			const int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);
			if ((buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0) {
				int delta_x = mouse_x - prev_mouse_x;
				int delta_y = mouse_y - prev_mouse_y;
				cam_yaw += float(-delta_x) * rot_scale;
				cam_pitch += float(-delta_y) * rot_scale;
			}
			prev_mouse_x = mouse_x;
			prev_mouse_y = mouse_y;
		}

		float cam_rotation[16];
		bx::mtxRotateXYZ(cam_rotation, cam_pitch, cam_yaw, 0.0f);

		float cam_translation[16];
		bx::mtxTranslate(cam_translation, 0.0f, 0.0f, -5.0f);

		float cam_transform[16];
		bx::mtxMul(cam_transform, cam_translation, cam_rotation);

		float view[16];
		bx::mtxInverse(view, cam_transform);

		float proj[16];
		bx::mtxProj(
			proj, 60.0f, float(getWidth()) / float(getHeight()), 0.1f,
			100.0f, bgfx::getCaps()->homogeneousDepth);

		bgfx::setViewTransform(0, view, proj);

		float model[16];
		bx::mtxIdentity(model);
		bgfx::setTransform(model);

		bgfx::setVertexBuffer(0, m_vbh);
		bgfx::setIndexBuffer(m_ibh);

		bgfx::submit(0, m_program);
	}

	int shutdown()
	{
		bgfx::destroy(m_vbh);
		bgfx::destroy(m_ibh);
		bgfx::destroy(m_program);

		ImGui_ImplSDL2_Shutdown();
		ImGui_Implbgfx_Shutdown();

		ImGui::DestroyContext();
		bgfx::shutdown();

		SDL_DestroyWindow(m_window);
		SDL_Quit();

		return 0;
	}

public:
	Shaderscope() : bims::Application() {}

private:
	bgfx::ProgramHandle m_program;
	bgfx::VertexBufferHandle m_vbh;
	bgfx::IndexBufferHandle m_ibh;

};

int main(int argc, char** argv)
{
	Shaderscope app;
	return app.run(argc, argv);
}
