#include "CWorldShadowShader.hpp"

namespace urde
{

static const char* VS =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"layout(location=0) in vec4 posIn;\n"
"\n"
"UBINDING0 uniform ColoredQuadUniform\n"
"{\n"
"    mat4 xf;\n"
"    vec4 color;\n"
"};\n"
"\n"
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"};\n"
"\n"
"SBINDING(0) out VertToFrag vtf;\n"
"void main()\n"
"{\n"
"    vtf.color = color;\n"
"    gl_Position = xf * vec4(posIn.xyz, 1.0);\n"
"}\n";

static const char* FS =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"};\n"
"\n"
"SBINDING(0) in VertToFrag vtf;\n"
"layout(location=0) out vec4 colorOut;\n"
"void main()\n"
"{\n"
"    colorOut = vtf.color;\n"
"}\n";

URDE_DECL_SPECIALIZE_SHADER(CWorldShadowShader)

static boo::ObjToken<boo::IVertexFormat> s_VtxFmt;
static boo::ObjToken<boo::IShaderPipeline> s_Pipeline;
static boo::ObjToken<boo::IShaderPipeline> s_ZPipeline;

struct CWorldShadowShaderGLDataBindingFactory : TShader<CWorldShadowShader>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding> BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                                                                  CWorldShadowShader& filter)
    {
        boo::GLDataFactory::Context& cctx = static_cast<boo::GLDataFactory::Context&>(ctx);

        const boo::VertexElementDescriptor VtxVmt[] =
        {
            {filter.m_vbo.get(), nullptr, boo::VertexSemantic::Position4}
        };
        boo::ObjToken<boo::IGraphicsBuffer> bufs[] = {filter.m_uniBuf.get()};
        boo::PipelineStage stages[] = {boo::PipelineStage::Vertex};
        boo::ObjToken<boo::IVertexFormat> vtxFmt = ctx.newVertexFormat(1, VtxVmt);
        filter.m_dataBind = cctx.newShaderDataBinding(s_Pipeline,
            vtxFmt, filter.m_vbo.get(), nullptr, nullptr,
            1, bufs, stages, nullptr, nullptr, 0, nullptr, nullptr, nullptr);
        filter.m_zDataBind = cctx.newShaderDataBinding(s_ZPipeline,
            vtxFmt, filter.m_vbo.get(), nullptr, nullptr,
            1, bufs, stages, nullptr, nullptr, 0, nullptr, nullptr, nullptr);
        return nullptr;
    }
};

#if BOO_HAS_VULKAN
struct CWorldShadowShaderVulkanDataBindingFactory : TShader<CWorldShadowShader>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding>
    BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                           CWorldShadowShader& filter)
    {
        boo::VulkanDataFactory::Context& cctx = static_cast<boo::VulkanDataFactory::Context&>(ctx);

        boo::ObjToken<boo::IGraphicsBuffer> bufs[] = {filter.m_uniBuf.get()};
        filter.m_dataBind = cctx.newShaderDataBinding(s_Pipeline, s_VtxFmt,
            filter.m_vbo.get(), nullptr, nullptr, 1, bufs,
            nullptr, nullptr, nullptr, 0, nullptr, nullptr, nullptr);
        filter.m_zDataBind = cctx.newShaderDataBinding(s_ZPipeline, s_VtxFmt,
            filter.m_vbo.get(), nullptr, nullptr, 1, bufs,
            nullptr, nullptr, nullptr, 0, nullptr, nullptr, nullptr);
        return nullptr;
    }
};
#endif

TShader<CWorldShadowShader>::IDataBindingFactory*
CWorldShadowShader::Initialize(boo::GLDataFactory::Context& ctx)
{
    const char* uniNames[] = {"ColoredQuadUniform"};
    s_Pipeline = ctx.newShaderPipeline(VS, FS, 0, nullptr, 1, uniNames, boo::BlendFactor::SrcAlpha,
                                       boo::BlendFactor::InvSrcAlpha, boo::Primitive::TriStrips,
                                       boo::ZTest::None, false, true, false, boo::CullMode::None);
    s_ZPipeline = ctx.newShaderPipeline(VS, FS, 0, nullptr, 1, uniNames, boo::BlendFactor::SrcAlpha,
                                        boo::BlendFactor::InvSrcAlpha, boo::Primitive::TriStrips,
                                        boo::ZTest::LEqual, true, true, false, boo::CullMode::None);
    return new CWorldShadowShaderGLDataBindingFactory;
}

template <>
void CWorldShadowShader::Shutdown<boo::GLDataFactory>()
{
    s_Pipeline.reset();
    s_ZPipeline.reset();
}

#if BOO_HAS_VULKAN
TShader<CWorldShadowShader>::IDataBindingFactory*
CWorldShadowShader::Initialize(boo::VulkanDataFactory::Context& ctx)
{
    const boo::VertexElementDescriptor VtxVmt[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4}
    };
    s_VtxFmt = ctx.newVertexFormat(1, VtxVmt);
    s_Pipeline = ctx.newShaderPipeline(VS, FS, s_VtxFmt, boo::BlendFactor::SrcAlpha,
                                       boo::BlendFactor::InvSrcAlpha, boo::Primitive::TriStrips,
                                       boo::ZTest::None, false, true, false, boo::CullMode::None);
    s_ZPipeline = ctx.newShaderPipeline(VS, FS, s_VtxFmt, boo::BlendFactor::SrcAlpha,
                                        boo::BlendFactor::InvSrcAlpha, boo::Primitive::TriStrips,
                                        boo::ZTest::LEqual, true, true, false, boo::CullMode::None);
    return new CWorldShadowShaderVulkanDataBindingFactory;
}

template <>
void CWorldShadowShader::Shutdown<boo::VulkanDataFactory>()
{
    s_VtxFmt.reset();
    s_Pipeline.reset();
    s_ZPipeline.reset();
}
#endif

}

