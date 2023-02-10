#pragma once

#include <bims.h>
#include <bgfx/bgfx.h>

namespace bims
{
    static bgfx::ShaderHandle createShader(const std::string& shader, const char* name = "");

    ///
    bgfx::ShaderHandle loadShader(const std::string& filename, const char* name = "");

    ///
    bgfx::ProgramHandle loadProgram(const std::string& vFilename, const std::string& fFilename);
}