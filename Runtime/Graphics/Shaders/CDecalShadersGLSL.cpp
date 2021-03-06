#include "CDecalShaders.hpp"
#include "Particle/CDecal.hpp"
#include "Graphics/CModel.hpp"

namespace urde
{

static const char* VS_GLSL_TEX =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"layout(location=0) in vec4 posIn[4];\n"
"layout(location=4) in vec4 colorIn;\n"
"layout(location=5) in vec4 uvsIn[4];\n"
"\n"
"UBINDING0 uniform DecalUniform\n"
"{\n"
"    mat4 mvp;\n"
"    vec4 moduColor;\n"
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
"    vtf.color = colorIn * moduColor;\n"
"    vtf.uv = uvsIn[gl_VertexID].xy;\n"
"    gl_Position = mvp * posIn[gl_VertexID];\n"
"}\n";

static const char* FS_GLSL_TEX =
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

static const char* FS_GLSL_TEX_REDTOALPHA =
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
"    colorOut = vtf.color;\n"
"    colorOut.a = texture(tex, vtf.uv).r;\n"
"}\n";

static const char* VS_GLSL_NOTEX =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"layout(location=0) in vec4 posIn[4];\n"
"layout(location=4) in vec4 colorIn;\n"
"\n"
"UBINDING0 uniform DecalUniform\n"
"{\n"
"    mat4 mvp;\n"
"    vec4 moduColor;\n"
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
"    vtf.color = colorIn * moduColor;\n"
"    gl_Position = mvp * posIn[gl_VertexID];\n"
"}\n";

static const char* FS_GLSL_NOTEX =
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

struct OGLDecalDataBindingFactory : TShader<CDecalShaders>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding> BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                                                                  CDecalShaders& shaders)
    {
        CQuadDecal& decal = shaders.m_decal;
        const SQuadDescr* desc = decal.m_desc;

        boo::ObjToken<boo::IVertexFormat> vtxFmt;
        CUVElement* texr = desc->x14_TEX.get();
        int texCount = 0;
        boo::ObjToken<boo::ITexture> textures[1];

        if (texr)
        {
            textures[0] = texr->GetValueTexture(0).GetObj()->GetBooTexture();
            texCount = 1;
            if (decal.m_instBuf)
            {
                const boo::VertexElementDescriptor TexFmtTex[] =
                {
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 0},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 1},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 2},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 3}
                };
                vtxFmt = ctx.newVertexFormat(9, TexFmtTex);
            }
        }
        else
        {
            if (decal.m_instBuf)
            {
                const boo::VertexElementDescriptor TexFmtNoTex[] =
                {
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
                    {decal.m_instBuf.get(), nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced}
                };
                vtxFmt = ctx.newVertexFormat(5, TexFmtNoTex);
            }
        }

        if (decal.m_instBuf)
        {
            boo::ObjToken<boo::IGraphicsBuffer> uniforms[] = {decal.m_uniformBuf.get()};

            if (shaders.m_regPipeline)
                decal.m_normalDataBind = ctx.newShaderDataBinding(shaders.m_regPipeline, vtxFmt, nullptr,
                                                                decal.m_instBuf.get(), nullptr, 1, uniforms,
                                                                nullptr, texCount, textures, nullptr, nullptr);
            if (shaders.m_redToAlphaPipeline)
                decal.m_redToAlphaDataBind = ctx.newShaderDataBinding(shaders.m_redToAlphaPipeline, vtxFmt, nullptr,
                                                                    decal.m_instBuf.get(), nullptr, 1, uniforms,
                                                                    nullptr, texCount, textures, nullptr, nullptr);
        }

        return nullptr;
    }
};

static const char* UniNames[] = {"DecalUniform"};
static const char* TexNames[] = {"tex"};

TShader<CDecalShaders>::IDataBindingFactory* CDecalShaders::Initialize(boo::GLDataFactory::Context& ctx)
{
    m_texZTestNoZWrite = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX, 1, TexNames, 1, UniNames,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                               true, false, boo::CullMode::None);
    m_texAdditiveZTest = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX, 1, TexNames, 1, UniNames,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                               true, false, boo::CullMode::None);
    m_texRedToAlphaZTest = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX_REDTOALPHA, 1, TexNames, 1, UniNames,
                                                 boo::BlendFactor::One, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, true, boo::CullMode::None);
    m_noTexZTestNoZWrite = ctx.newShaderPipeline(VS_GLSL_NOTEX, FS_GLSL_NOTEX, 0, nullptr, 1, UniNames,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, false, boo::CullMode::None);
    m_noTexAdditiveZTest = ctx.newShaderPipeline(VS_GLSL_NOTEX, FS_GLSL_NOTEX, 0, nullptr, 1, UniNames,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, false, boo::CullMode::None);

    return new struct OGLDecalDataBindingFactory;
}

template <>
void CDecalShaders::Shutdown<boo::GLDataFactory>()
{
    m_texZTestNoZWrite.reset();
    m_texAdditiveZTest.reset();
    m_texRedToAlphaZTest.reset();

    m_noTexZTestNoZWrite.reset();
    m_noTexAdditiveZTest.reset();
}

#if BOO_HAS_VULKAN
struct VulkanDecalDataBindingFactory : TShader<CDecalShaders>::IDataBindingFactory
{
    boo::ObjToken<boo::IShaderDataBinding>
    BuildShaderDataBinding(boo::IGraphicsDataFactory::Context& ctx,
                           CDecalShaders& shaders)
    {
        CQuadDecal& decal = shaders.m_decal;
        const SQuadDescr* desc = decal.m_desc;

        CUVElement* texr = desc->x14_TEX.get();
        int texCount = 0;
        boo::ObjToken<boo::ITexture> textures[1];

        if (texr)
        {
            textures[0] = texr->GetValueTexture(0).GetObj()->GetBooTexture();
            texCount = 1;
        }

        if (decal.m_instBuf)
        {
            boo::ObjToken<boo::IGraphicsBuffer> uniforms[] = {decal.m_uniformBuf.get()};

            if (shaders.m_regPipeline)
                decal.m_normalDataBind = ctx.newShaderDataBinding(shaders.m_regPipeline, nullptr, nullptr,
                                                                decal.m_instBuf.get(), nullptr, 1, uniforms,
                                                                nullptr, texCount, textures, nullptr, nullptr);
            if (shaders.m_redToAlphaPipeline)
                decal.m_redToAlphaDataBind = ctx.newShaderDataBinding(shaders.m_redToAlphaPipeline, nullptr, nullptr,
                                                                    decal.m_instBuf.get(), nullptr, 1, uniforms,
                                                                    nullptr, texCount, textures, nullptr, nullptr);
        }

        return nullptr;
    }
};

TShader<CDecalShaders>::IDataBindingFactory* CDecalShaders::Initialize(boo::VulkanDataFactory::Context& ctx)
{
    static const boo::VertexElementDescriptor TexFmtTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::UV4 | boo::VertexSemantic::Instanced, 3}
    };
    m_vtxFormatTex = ctx.newVertexFormat(9, TexFmtTex);

    static const boo::VertexElementDescriptor TexFmtNoTex[] =
    {
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 0},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 1},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 2},
        {nullptr, nullptr, boo::VertexSemantic::Position4 | boo::VertexSemantic::Instanced, 3},
        {nullptr, nullptr, boo::VertexSemantic::Color | boo::VertexSemantic::Instanced}
    };
    m_vtxFormatNoTex = ctx.newVertexFormat(5, TexFmtNoTex);

    m_texZTestNoZWrite = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX, m_vtxFormatTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                               true, false, boo::CullMode::None);
    m_texAdditiveZTest = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX, m_vtxFormatTex,
                                               boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                               boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                               true, false, boo::CullMode::None);
    m_texRedToAlphaZTest = ctx.newShaderPipeline(VS_GLSL_TEX, FS_GLSL_TEX_REDTOALPHA, m_vtxFormatTex,
                                                 boo::BlendFactor::One, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, true, boo::CullMode::None);
    m_noTexZTestNoZWrite = ctx.newShaderPipeline(VS_GLSL_NOTEX, FS_GLSL_NOTEX, m_vtxFormatNoTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, false, boo::CullMode::None);
    m_noTexAdditiveZTest = ctx.newShaderPipeline(VS_GLSL_NOTEX, FS_GLSL_NOTEX, m_vtxFormatNoTex,
                                                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::One,
                                                 boo::Primitive::TriStrips, boo::ZTest::LEqual, false,
                                                 true, false, boo::CullMode::None);

    return new struct VulkanDecalDataBindingFactory;
}

template <>
void CDecalShaders::Shutdown<boo::VulkanDataFactory>()
{
    m_vtxFormatTex.reset();
    m_vtxFormatNoTex.reset();

    m_texZTestNoZWrite.reset();
    m_texAdditiveZTest.reset();
    m_texRedToAlphaZTest.reset();

    m_noTexZTestNoZWrite.reset();
    m_noTexAdditiveZTest.reset();
}
#endif

}
