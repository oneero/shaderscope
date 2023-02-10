#include <bims.h>
#include <bims/shader-utils.h>
#include <bgfx/bgfx.h>

const std::string shader_root = "shader/build/boids/";

static bgfx::ShaderHandle bims::createShader(const std::string& shader, const char* name)
{
    const bgfx::Memory* mem = bgfx::copy(shader.data(), shader.size());
    const bgfx::ShaderHandle handle = bgfx::createShader(mem);
    if (name != "") bgfx::setName(handle, name);
    return handle;
}

bgfx::ShaderHandle bims::loadShader(const std::string& filename, const char* name)
{
    std::string shader;
    std::string path = shader_root + filename + ".bin"; // unnecessary allocation?
    if (!fileops::read_file(path, shader)) {
        std::cout << "Could not find shader: " << path << ".bin \n";
    }

    return bims::createShader(shader, name);
}

bgfx::ProgramHandle bims::loadProgram(const std::string& vFilename, const std::string& fFilename)
{
    bgfx::ShaderHandle vShader = bims::loadShader(vFilename);
    bgfx::ShaderHandle fShader = bims::loadShader(fFilename);
    return bgfx::createProgram(vShader, fShader, true);
}