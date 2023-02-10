#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <bx/math.h>

#include <SDL.h>
#include <SDL_syswm.h>

#include <bims.h>
#include <bims/imgui_impl_bgfx.h>
#include <bims/imgui_impl_sdl.h>
#include <bims/file-ops.h>
#include <bims/shader-utils.h>

#include "imgui.h"

struct ParamsData
{
    float   timeStep;
    int32_t dispatchSize;
    float   particleIntensity;    float   particleSize;

    float   particlePower;
    int32_t baseSeed;
    float   initialSpeed;
    int32_t maxNeighbors;

    float   maxVelocity;
    float   maxForce;	
    float   sensorDistance;
    float   desiredSeparation;

    float   edgeAvoidanceThreshold;
    float   edgeAvoidanceMax;
    float   wanderDistance;
    float   sepMult;

    float   aliMult;
    float   cohMult;
    float   edgeAvoidanceMult;
    float   wanderMult;
};


void initializeParams(ParamsData* _params)
{
    _params->timeStep = 0.06f;
    _params->dispatchSize = 32;
    _params->particleIntensity = 1.0f;
    _params->particleSize = 0.01f;

    _params->particlePower = 0.001f;
    _params->baseSeed = 42;
    _params->initialSpeed = 3.2f;
    _params->maxNeighbors = 1000;

    _params->maxVelocity = 1.0f;
    _params->maxForce = 1.0f;
    _params->sensorDistance = 50.0f;
    _params->desiredSeparation = 15.0f;

    _params->edgeAvoidanceThreshold = 0.1f;
    _params->edgeAvoidanceMax = 1.0f;
    _params->wanderDistance = 0.84f;
    _params->sepMult = 0.586f;

    _params->aliMult = 0.189f;
    _params->cohMult = 0.479f;
    _params->edgeAvoidanceMult = 0.136f;
    _params->wanderMult = 0.25f;
}

static const float s_quadVertices[] =
{
     1.0f,  1.0f,
    -1.0f,  1.0f,
    -1.0f, -1.0f,
     1.0f, -1.0f,
};

void SetupImGuiStyle()
{
	// Material Flat style by ImJC1C from ImThemes
	ImGuiStyle& style = ImGui::GetStyle();
	
	style.Alpha = 1.0f;
	style.DisabledAlpha = 0.5f;
	style.WindowPadding = ImVec2(8.0f, 8.0f);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_Left;
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4.0f, 3.0f);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(8.0f, 4.0f);
	style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
	style.CellPadding = ImVec2(4.0f, 2.0f);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 14.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 0.0f;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 0.0f;
	style.TabMinWidthForCloseButton = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Left;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
	
	style.Colors[ImGuiCol_Text] = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.8313725590705872f, 0.8470588326454163f, 0.8784313797950745f, 0.501960813999176f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.1587982773780823f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.1725490242242813f, 0.1921568661928177f, 0.2352941185235977f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.501960813999176f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.01960784383118153f, 0.01960784383118153f, 0.01960784383118153f, 0.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.5333333611488342f, 0.5333333611488342f, 0.5333333611488342f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.3333333432674408f, 0.3333333432674408f, 0.3333333432674408f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6000000238418579f, 0.6000000238418579f, 0.6000000238418579f, 1.0f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.239215686917305f, 0.5215686559677124f, 0.8784313797950745f, 1.0f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9803921580314636f, 1.0f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 0.501960813999176f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.4274509847164154f, 0.4274509847164154f, 0.4980392158031464f, 0.5f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 0.7799999713897705f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.09803921729326248f, 0.4000000059604645f, 0.7490196228027344f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 0.250980406999588f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.1529411822557449f, 0.1725490242242813f, 0.2117647081613541f, 1.0f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.3098039329051971f, 0.6235294342041016f, 0.9333333373069763f, 1.0f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.6078431606292725f, 0.6078431606292725f, 0.6078431606292725f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.0f, 0.4274509847164154f, 0.3490196168422699f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.8980392217636108f, 0.6980392336845398f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.0f, 0.6000000238418579f, 0.0f, 1.0f);
	style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 1.0f);
	style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
	style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 0.5021458864212036f);
	style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.0f, 1.0f, 1.0f, 0.03862661123275757f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 1.0f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(1.0f, 1.0f, 0.0f, 0.8999999761581421f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.2588235437870026f, 0.5882353186607361f, 0.9764705896377563f, 1.0f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.2039215713739395f, 0.2313725501298904f, 0.2823529541492462f, 0.7529411911964417f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.105882354080677f, 0.1137254908680916f, 0.1372549086809158f, 0.7529411911964417f);
}

static const uint16_t s_quadIndices[] = { 0, 1, 2, 2, 3, 0, };

const uint32_t kThreadGroupUpdateSize = 512;
const uint32_t kMaxParticleCount      = 32 * 1024;

class Shaderscope : public bims::Application
{
    void initialize()
    {
        SetupImGuiStyle();
        
        // Check device capabilities

        const bgfx::Caps* caps = bgfx::getCaps();
        m_computeSupported = !!(caps->supported & BGFX_CAPS_COMPUTE);
        m_indirectSupported = !!(caps->supported & BGFX_CAPS_DRAW_INDIRECT);

        if (!m_computeSupported || !m_indirectSupported)
        {
            printf("GPU does not support compute shaders or indirect rendering.\n");
            m_quit = true;
            return;
        }

        // Create static quad vertex layout and buffers

        bgfx::VertexLayout quadVertexLayout;
        quadVertexLayout.begin()
        .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
        .end();

        bgfx::VertexLayout pos_col_vert_layout;

        // Create static vertex and index buffers.
        // Static data can be passed with bgfx::makeRef
        m_vbh = bgfx::createVertexBuffer(bgfx::makeRef(s_quadVertices, sizeof(s_quadVertices) ), quadVertexLayout);
        m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(s_quadIndices, sizeof(s_quadIndices) ));

        // Create shader program for rendering and setup compute buffers

        // Create particle program from shaders.
        m_particleProgram = bims::loadProgram("boids_vs", "boids_fs");

        // Setup compute buffers
        // Brute force multiple vec4 buffers. Refactor to different layout or struct buffers later.

        // Layout
        bgfx::VertexLayout computeVertexLayout;
        computeVertexLayout.begin()
            .add(bgfx::Attrib::TexCoord0, 4, bgfx::AttribType::Float)
            .end();

        m_currPositionBuffer0 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_currPositionBuffer1 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_prevPositionBuffer0 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_prevPositionBuffer1 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

        m_currVelocityBuffer0 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_currVelocityBuffer1 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_prevVelocityBuffer0 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);
        m_prevVelocityBuffer1 = bgfx::createDynamicVertexBuffer(1 << 15, computeVertexLayout, BGFX_BUFFER_COMPUTE_READ_WRITE);

        u_params = bgfx::createUniform("u_params", bgfx::UniformType::Vec4, 5);

        m_initInstancesProgram   = bgfx::createProgram(bims::loadShader("boids_cs_init"), true);
        m_updateInstancesProgram = bgfx::createProgram(bims::loadShader("boids_cs_update"), true);

        // Setup indirect rendering
        m_indirectProgram = BGFX_INVALID_HANDLE;
        m_indirectBuffer  = BGFX_INVALID_HANDLE;

        if (m_indirectSupported)
        {
            m_indirectProgram = bgfx::createProgram(bims::loadShader("boids_cs_indirect"), true);
            m_indirectBuffer  = bgfx::createIndirectBuffer(2);
        }

        initializeParams(&m_paramsData);

        bgfx::setUniform(u_params, &m_paramsData, 5);
        bgfx::setBuffer(0, m_prevPositionBuffer0, bgfx::Access::Write);
        bgfx::setBuffer(1, m_currPositionBuffer0, bgfx::Access::Write);
        bgfx::setBuffer(2, m_prevVelocityBuffer0, bgfx::Access::Write);
        bgfx::setBuffer(3, m_currVelocityBuffer0, bgfx::Access::Write);
        bgfx::dispatch(0, m_initInstancesProgram, kMaxParticleCount / kThreadGroupUpdateSize, 1, 1);

        m_useIndirect = false;
    }

    void update()
    {
        ImGui::NewFrame();

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

        if (m_computeSupported)
        {
            ImGui::SetNextWindowPos(
                    ImVec2(getWidth() - getWidth() / 5.0f - 10.0f, 10.0f)
                , ImGuiCond_FirstUseEver
                );
            ImGui::SetNextWindowSize(
                    ImVec2(getWidth() / 5.0f, getHeight() / 1.5f)
                , ImGuiCond_FirstUseEver
                );
            ImGui::Begin("Settings"
                , NULL
                , 0
                );

            bool    reset = false;

            ImGui::SliderInt("Random seed", &m_paramsData.baseSeed, 0, 100);

            if (ImGui::Button("Reset") )
            {
                reset = true;
            }

            ImGui::Separator();

            ImGui::SliderInt("Particle count (x512)", &m_paramsData.dispatchSize, 1, 64);
            ImGui::SliderFloat("Time step", &m_paramsData.timeStep, 0.0f, 0.1f);

            ImGui::Separator();
            
            //ImGui::SliderFloat("Particle intensity", &m_paramsData.particleIntensity, 0.0f, 1.0f);
            ImGui::SliderFloat("Particle size", &m_paramsData.particleSize, 0.0f, 1.0f);
            //ImGui::SliderFloat("Particle power", &m_paramsData.particlePower, 0.001f, 16.0f);

            //ImGui::SliderFloat("Max velocity", &m_paramsData.maxVelocity, 0.0f, 1.0f);
            //ImGui::SliderFloat("Max force", &m_paramsData.maxForce, 0.0f, 1.0f);

            ImGui::SliderFloat("Sensor distance", &m_paramsData.sensorDistance, 0.0f, 100.0f);
            ImGui::SliderFloat("Desired separation", &m_paramsData.desiredSeparation, 0.0f, 100.0f);

            //ImGui::SliderFloat("Edge threshold", &m_paramsData.edgeAvoidanceThreshold, 0.0f, 100.0f);
            //ImGui::SliderFloat("Max edge avoidance", &m_paramsData.edgeAvoidanceMax, 0.0f, 100.0f);
            ImGui::SliderFloat("Wander distance", &m_paramsData.wanderDistance, 0.0f, 2.0f);

            ImGui::SliderFloat("Separation mult", &m_paramsData.sepMult, 0.0f, 1.0f);
            ImGui::SliderFloat("Alignment mult", &m_paramsData.aliMult, 0.0f, 1.0f);
            ImGui::SliderFloat("Cohesion mult", &m_paramsData.cohMult, 0.0f, 1.0f);
            ImGui::SliderFloat("Center mult", &m_paramsData.edgeAvoidanceMult, 0.0f, 1.0f);
            ImGui::SliderFloat("Wander mult", &m_paramsData.wanderMult, 0.0f, 1.0f);

            ImGui::Separator();

            if (m_indirectSupported)
            {
                ImGui::Checkbox("Use draw/dispatch indirect", &m_useIndirect);
            }

            ImGui::End();

            if (reset)
            {
                bgfx::setBuffer(0, m_prevPositionBuffer0, bgfx::Access::Write);
                bgfx::setBuffer(1, m_currPositionBuffer0, bgfx::Access::Write);
                bgfx::setBuffer(2, m_prevVelocityBuffer0, bgfx::Access::Write);
                bgfx::setBuffer(3, m_currVelocityBuffer0, bgfx::Access::Write);
                bgfx::setUniform(u_params, &m_paramsData, 5);
                bgfx::dispatch(0, m_initInstancesProgram, kMaxParticleCount / kThreadGroupUpdateSize, 1, 1);
            }

            if (m_useIndirect)
            {
                bgfx::setUniform(u_params, &m_paramsData, 5);
                bgfx::setBuffer(0, m_indirectBuffer, bgfx::Access::Write);
                bgfx::dispatch(0, m_indirectProgram);
            }

            bgfx::setBuffer(0, m_prevPositionBuffer0, bgfx::Access::Read);
            bgfx::setBuffer(1, m_currPositionBuffer0, bgfx::Access::Read);
            bgfx::setBuffer(2, m_prevPositionBuffer1, bgfx::Access::Write);
            bgfx::setBuffer(3, m_currPositionBuffer1, bgfx::Access::Write);
            bgfx::setBuffer(4, m_prevVelocityBuffer0, bgfx::Access::Read);
            bgfx::setBuffer(5, m_currVelocityBuffer0, bgfx::Access::Read);
            bgfx::setBuffer(6, m_prevVelocityBuffer1, bgfx::Access::Write);
            bgfx::setBuffer(7, m_currVelocityBuffer1, bgfx::Access::Write);
            bgfx::setUniform(u_params, &m_paramsData, 5);

            if (m_useIndirect)
            {
                bgfx::dispatch(0, m_updateInstancesProgram, m_indirectBuffer, 1);
            }
            else
            {
                bgfx::dispatch(0, m_updateInstancesProgram, uint16_t(m_paramsData.dispatchSize), 1, 1);
            }

            // Swap all buffers
            bx::swap(m_currPositionBuffer0, m_currPositionBuffer1);
            bx::swap(m_prevPositionBuffer0, m_prevPositionBuffer1);
            bx::swap(m_currVelocityBuffer0, m_currVelocityBuffer1);
            bx::swap(m_prevVelocityBuffer0, m_prevVelocityBuffer1);

            // MVP

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

            // Set vertex and index buffer.
            bgfx::setVertexBuffer(0, m_vbh);
            bgfx::setIndexBuffer(m_ibh);
            bgfx::setInstanceDataBuffer(m_currPositionBuffer0
                , 0
                , m_paramsData.dispatchSize * kThreadGroupUpdateSize
                );

            // Set render states.
            bgfx::setState(0
                | BGFX_STATE_WRITE_RGB
                | BGFX_STATE_BLEND_ADD
                | BGFX_STATE_DEPTH_TEST_ALWAYS
                );

            // Submit primitive for rendering to view 0.
            if (m_useIndirect)
            {
                bgfx::submit(0, m_particleProgram, m_indirectBuffer, 0);
            }
            else
            {
                bgfx::submit(0, m_particleProgram);
            }
        }
        
        ImGui::Render();
        ImGui_Implbgfx_RenderDrawLists(ImGui::GetDrawData());
    }

    int shutdown()
    {
        if (m_computeSupported)
        {
            if (m_indirectSupported)
            {
                bgfx::destroy(m_indirectProgram);
                bgfx::destroy(m_indirectBuffer);
            }

            bgfx::destroy(u_params);
            bgfx::destroy(m_currPositionBuffer0);
            bgfx::destroy(m_currPositionBuffer1);
            bgfx::destroy(m_prevPositionBuffer0);
            bgfx::destroy(m_prevPositionBuffer1);
            bgfx::destroy(m_currVelocityBuffer0);
            bgfx::destroy(m_currVelocityBuffer1);
            bgfx::destroy(m_prevVelocityBuffer0);
            bgfx::destroy(m_prevVelocityBuffer1);
            bgfx::destroy(m_updateInstancesProgram);
            bgfx::destroy(m_initInstancesProgram);
            bgfx::destroy(m_ibh);
            bgfx::destroy(m_vbh);
            bgfx::destroy(m_particleProgram);
        }

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
    bool m_useIndirect;
    bool m_computeSupported;
    bool m_indirectSupported;

    ParamsData m_paramsData;

    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle  m_ibh;
    bgfx::ProgramHandle m_particleProgram;
    bgfx::ProgramHandle m_indirectProgram;
    bgfx::ProgramHandle m_initInstancesProgram;
    bgfx::ProgramHandle m_updateInstancesProgram;
    bgfx::IndirectBufferHandle m_indirectBuffer;
    bgfx::DynamicVertexBufferHandle m_currPositionBuffer0;
    bgfx::DynamicVertexBufferHandle m_currPositionBuffer1;
    bgfx::DynamicVertexBufferHandle m_prevPositionBuffer0;
    bgfx::DynamicVertexBufferHandle m_prevPositionBuffer1;
    bgfx::DynamicVertexBufferHandle m_currVelocityBuffer0;
    bgfx::DynamicVertexBufferHandle m_currVelocityBuffer1;
    bgfx::DynamicVertexBufferHandle m_prevVelocityBuffer0;
    bgfx::DynamicVertexBufferHandle m_prevVelocityBuffer1;
    bgfx::UniformHandle u_params;

    int64_t m_timeOffset;
};

int main(int argc, char** argv)
{
    Shaderscope app;
    return app.run(argc, argv);
}
