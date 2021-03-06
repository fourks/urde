#include "CRadarPaintShader.hpp"
#include "TShader.hpp"
#include "Graphics/CTexture.hpp"

namespace urde
{

static const char* VS =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"layout(location=0) in vec4 posIn[4];\n"
"layout(location=4) in vec4 uvIn[4];\n"
"layout(location=8) in vec4 colorIn;\n"
"\n"
"UBINDING0 uniform RadarPaintUniform\n"
"{\n"
"    mat4 xf;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"    vec2 uv;\n"
"};\n"
"\n"
"SBINDING(0) out VertToFrag vtf;\n"
"void main()\n"
"{\n"
"    vec3 pos = posIn[gl_VertexID].xyz;\n"
"    vtf.uv = uvIn[gl_VertexID].xy;\n"
"    vtf.color = colorIn;\n"
"    gl_Position = xf * vec4(pos, 1.0);\n"
"}\n";

static const char* FS =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"    vec2 uv;\n"
"};\n"
"\n"
"SBINDING(0) in VertToFrag vtf;\n"
"layout(location=0) out vec4 colorOut;\n"
"TBINDING0 uniform sampler2D tex;\n"
"void main()\n"
"{\n"
"    colorOut = vtf.color * texture(tex, vtf.uv);\n"
"}\n";

URDE_DECL_SPECIALIZE_SHADER(CRadarPaintShader)

static boo::ObjToken<boo::IVertexFormat> s_VtxFmt;
static boo::ObjToken<boo::IShaderPipeline> s_Pipeline;

struct CRadarPaintShaderGLDataBindingFactory : TShader<CRadarPaintShader>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding> BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                                                                  CRadarPaintShader& filter)
    {
        boo::GLDataFactory::Context& cctx = static_cast<boo::GLDataFactory::Context&>(ctx);

        const boo::VertexElementDescriptor VtxVmt[] =
        {
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Position4, 0},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Position4, 1},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Position4, 2},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Position4, 3},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::UV4, 0},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::UV4, 1},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::UV4, 2},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::UV4, 3},
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Color}
        };
        boo::ObjToken<boo::IVertexFormat> vtxFmt = ctx.newVertexFormat(9, VtxVmt);
        boo::ObjToken<boo::IGraphicsBuffer> bufs[] = {filter.m_uniBuf.get()};
        boo::PipelineStage stages[] = {boo::PipelineStage::Vertex};
        boo::ObjToken<boo::ITexture> texs[] = {filter.m_tex->GetBooTexture()};
        return cctx.newShaderDataBinding(s_Pipeline,
                                         vtxFmt, nullptr, filter.m_vbo.get(), nullptr,
                                         1, bufs, stages, nullptr, nullptr, 1, texs, nullptr, nullptr);
    }
};

#if BOO_HAS_VULKAN
struct CRadarPaintShaderVulkanDataBindingFactory : TShader<CRadarPaintShader>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding>
    BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                           CRadarPaintShader& filter)
    {
        boo::VulkanDataFactory::Context& cctx = static_cast<boo::VulkanDataFactory::Context&>(ctx);

        boo::ObjToken<boo::IGraphicsBuffer> bufs[] = {filter.m_uniBuf.get()};
        boo::ObjToken<boo::ITexture> texs[] = {filter.m_tex->GetBooTexture()};
        return cctx.newShaderDataBinding(s_Pipeline, s_VtxFmt,
                                         nullptr, filter.m_vbo.get(), nullptr, 1, bufs,
                                         nullptr, nullptr, nullptr, 1, texs, nullptr, nullptr);
    }
};
#endif

TShader<CRadarPaintShader>::IDataBindingFactory*
CRadarPaintShader::Initialize(boo::GLDataFactory::Context& ctx)
{
    const char* uniNames[] = {"RadarPaintUniform"};
    const char* texNames[] = {"tex"};
    s_Pipeline = ctx.newShaderPipeline(VS, FS, 1, texNames, 1, uniNames, boo::BlendFactor::SrcAlpha,
                                       boo::BlendFactor::One, boo::Primitive::TriStrips,
                                       boo::ZTest::None, false, true, false, boo::CullMode::None);
    return new CRadarPaintShaderGLDataBindingFactory;
}

template <>
void CRadarPaintShader::Shutdown<boo::GLDataFactory>()
{
    s_Pipeline.reset();
}

#if BOO_HAS_VULKAN
TShader<CRadarPaintShader>::IDataBindingFactory*
CRadarPaintShader::Initialize(boo::VulkanDataFactory::Context& ctx)
{
    const boo::VertexElementDescriptor VtxVmt[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4, 3},
        {nullptr, nullptr, boo::VertexSemantic::UV4, 0},
        {nullptr, nullptr, boo::VertexSemantic::UV4, 1},
        {nullptr, nullptr, boo::VertexSemantic::UV4, 2},
        {nullptr, nullptr, boo::VertexSemantic::UV4, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color}
    };
    s_VtxFmt = ctx.newVertexFormat(9, VtxVmt);
    s_Pipeline = ctx.newShaderPipeline(VS, FS, s_VtxFmt, boo::BlendFactor::SrcAlpha,
                                       boo::BlendFactor::One, boo::Primitive::TriStrips,
                                       boo::ZTest::None, false, true, false, boo::CullMode::None);
    return new CRadarPaintShaderVulkanDataBindingFactory;
}

template <>
void CRadarPaintShader::Shutdown<boo::VulkanDataFactory>()
{
    s_VtxFmt.reset();
    s_Pipeline.reset();
}
#endif

}
