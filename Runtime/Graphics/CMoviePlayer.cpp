#include "CMoviePlayer.hpp"
#include "boo/graphicsdev/GLSLMacros.hpp"
#include "specter/View.hpp"
#include "CGraphics.hpp"
#include "Audio/g721.h"
#include "Audio/dsp.h"
#include "CDvdRequest.hpp"
#include <turbojpeg.h>

#define THP_BUFFER_FRAMES 30

namespace urde
{

static const char* VS_GLSL_YUV =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"layout(location=0) in vec3 posIn;\n"
"layout(location=1) in vec2 uvIn;\n"
SPECTER_GLSL_VIEW_VERT_BLOCK
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"    vec2 uv;\n"
"};\n"
"out VertToFrag vtf;\n"
"void main()\n"
"{\n"
"    vtf.uv = uvIn;\n"
"    vtf.color = mulColor;\n"
"    gl_Position = mv * vec4(posIn, 1.0);\n"
"}\n";

static const char* FS_GLSL_YUV =
"#version 330\n"
BOO_GLSL_BINDING_HEAD
"struct VertToFrag\n"
"{\n"
"    vec4 color;\n"
"    vec2 uv;\n"
"};\n"
"in VertToFrag vtf;\n"
"TBINDING0 uniform sampler2D texs[3];\n"
"layout(location=0) out vec4 colorOut;\n"
"void main()\n"
"{\n"
"    vec3 yuv;\n"
"    yuv.r = texture(texs[0], vtf.uv).r;\n"
"    yuv.g = texture(texs[1], vtf.uv).r;\n"
"    yuv.b = texture(texs[2], vtf.uv).r;\n"
"    yuv.r = 1.1643*(yuv.r-0.0625);\n"
"    yuv.g = yuv.g-0.5;\n"
"    yuv.b = yuv.b-0.5;\n"
"    colorOut = vec4(yuv.r+1.5958*yuv.b, yuv.r-0.39173*yuv.g-0.81290*yuv.b, yuv.r+2.017*yuv.g, 1.0) * vtf.color;\n"
"}\n";

static const char* VS_HLSL_YUV =
"struct VertData\n"
"{\n"
"    float3 posIn : POSITION;\n"
"    float2 uvIn : UV;\n"
"};\n"
SPECTER_HLSL_VIEW_VERT_BLOCK
"struct VertToFrag\n"
"{\n"
"    float4 position : SV_Position;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"VertToFrag main(in VertData v)\n"
"{\n"
"    VertToFrag vtf;\n"
"    vtf.uv = v.uvIn;\n"
"    vtf.color = mulColor;\n"
"    vtf.position = mul(mv, float4(v.posIn, 1.0));\n"
"    return vtf;\n"
"}\n";

static const char* FS_HLSL_YUV =
"struct VertToFrag\n"
"{\n"
"    float4 position : SV_Position;\n"
"    float4 color : COLOR;\n"
"    float2 uv : UV;\n"
"};\n"
"Texture2D texs[3] : register(t0);\n"
"SamplerState samp : register(s0);\n"
"float4 main(in VertToFrag vtf) : SV_Target0\n"
"{\n"
"    float3 yuv;\n"
"    yuv.r = texs[0].Sample(samp, vtf.uv).r;\n"
"    yuv.g = texs[1].Sample(samp, vtf.uv).r;\n"
"    yuv.b = texs[2].Sample(samp, vtf.uv).r;\n"
"    yuv.r = 1.1643*(yuv.r-0.0625);\n"
"    yuv.g = yuv.g-0.5;\n"
"    yuv.b = yuv.b-0.5;\n"
"    return float4(yuv.r+1.5958*yuv.b, yuv.r-0.39173*yuv.g-0.81290*yuv.b, yuv.r+2.017*yuv.g, 1.0) * vtf.color;\n"
"}\n";

static const char* VS_METAL_YUV =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"struct VertData\n"
"{\n"
"    float3 posIn [[ attribute(0) ]];\n"
"    float2 uvIn [[ attribute(1) ]];\n"
"};\n"
SPECTER_METAL_VIEW_VERT_BLOCK
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"vertex VertToFrag vmain(VertData v [[ stage_in ]], constant SpecterViewBlock& view [[ buffer(2) ]])\n"
"{\n"
"    VertToFrag vtf;\n"
"    vtf.uv = v.uvIn;\n"
"    vtf.color = view.mulColor;\n"
"    vtf.position = view.mv * float4(v.posIn, 1.0);\n"
"    return vtf;\n"
"}\n";

static const char* FS_METAL_YUV =
"#include <metal_stdlib>\n"
"using namespace metal;\n"
"constexpr sampler samp(address::repeat);\n"
"struct VertToFrag\n"
"{\n"
"    float4 position [[ position ]];\n"
"    float4 color;\n"
"    float2 uv;\n"
"};\n"
"fragment float4 fmain(VertToFrag vtf [[ stage_in ]],"
"                      texture2d<float> tex0 [[ texture(0) ]],\n"
"                      texture2d<float> tex1 [[ texture(1) ]],\n"
"                      texture2d<float> tex2 [[ texture(2) ]])\n"
"{\n"
"    float3 yuv;\n"
"    yuv.r = tex0.sample(samp, vtf.uv).r;\n"
"    yuv.g = tex1.sample(samp, vtf.uv).r;\n"
"    yuv.b = tex2.sample(samp, vtf.uv).r;\n"
"    yuv.r = 1.1643*(yuv.r-0.0625);\n"
"    yuv.g = yuv.g-0.5;\n"
"    yuv.b = yuv.b-0.5;\n"
"    return float4(yuv.r+1.5958*yuv.b, yuv.r-0.39173*yuv.g-0.81290*yuv.b, yuv.r+2.017*yuv.g, 1.0) * vtf.color;\n"
"}\n";

static boo::GraphicsDataToken GraphicsData;
static boo::IVertexFormat* YUVVTXFmt = nullptr;
static boo::IShaderPipeline* YUVShaderPipeline = nullptr;
static tjhandle TjHandle = nullptr;

void CMoviePlayer::Initialize()
{
    static const char* BlockNames[] = {"SpecterViewBlock"};

    if (!CGraphics::g_BooFactory->bindingNeedsVertexFormat())
    {
        boo::VertexElementDescriptor texvdescs[] =
        {
            {nullptr, nullptr, boo::VertexSemantic::Position4},
            {nullptr, nullptr, boo::VertexSemantic::UV4}
        };
        YUVVTXFmt = CGraphics::g_BooFactory->newVertexFormat(2, texvdescs);
    }

    switch (CGraphics::g_BooFactory->platform())
    {
    case boo::IGraphicsDataFactory::Platform::OGL:
        YUVShaderPipeline = static_cast<boo::GLDataFactory*>(CGraphics::g_BooFactory)->newShaderPipeline
                (VS_GLSL_YUV, FS_GLSL_YUV, 3, "texs", 1, BlockNames,
                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                 false, false, false);
        break;
#if _WIN32
    case boo::IGraphicsDataFactory::Platform::D3D11:
    case boo::IGraphicsDataFactory::Platform::D3D12:
        YUVShaderPipeline = static_cast<boo::ID3DDataFactory*>(CGraphics::g_BooFactory)->newShaderPipeline
                (VS_HLSL_YUV, FS_HLSL_YUV, ComPtr<ID3DBlob>(), ComPtr<ID3DBlob>(), ComPtr<ID3DBlob>(), YUVVTXFmt,
                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                 false, false, false);
        break;
#endif
#if BOO_HAS_METAL
    case boo::IGraphicsDataFactory::Platform::Metal:
        YUVShaderPipeline = static_cast<boo::MetalDataFactory*>(CGraphics::g_BooFactory)->newShaderPipeline
                (VS_METAL_YUV, FS_METAL_YUV, YUVVTXFmt, 1,
                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                 false, false, false);
        break;
#endif
#if BOO_HAS_VULKAN
    case boo::IGraphicsDataFactory::Platform::Vulkan:
        YUVShaderPipeline = static_cast<boo::VulkanDataFactory*>(CGraphics::g_BooFactory)->newShaderPipeline
                (VS_GLSL_YUV, FS_GLSL_YUV, YUVVTXFmt,
                 boo::BlendFactor::SrcAlpha, boo::BlendFactor::InvSrcAlpha,
                 false, false, false);
        break;
#endif
    default: break;
    }

    GraphicsData = CGraphics::CommitResources();
    TjHandle = tjInitDecompress();
}

void CMoviePlayer::Shutdown()
{
    GraphicsData.doDestroy();
    tjDestroy(TjHandle);
}

void CMoviePlayer::THPHeader::swapBig()
{
    magic = hecl::SBig(magic);
    version = hecl::SBig(version);
    maxBufferSize = hecl::SBig(maxBufferSize);
    maxAudioSamples = hecl::SBig(maxAudioSamples);
    fps = hecl::SBig(fps);
    numFrames = hecl::SBig(numFrames);
    firstFrameSize = hecl::SBig(firstFrameSize);
    dataSize = hecl::SBig(dataSize);
    componentDataOffset = hecl::SBig(componentDataOffset);
    offsetsDataOffset = hecl::SBig(offsetsDataOffset);
    firstFrameOffset = hecl::SBig(firstFrameOffset);
    lastFrameOffset = hecl::SBig(lastFrameOffset);
}

void CMoviePlayer::THPComponents::swapBig()
{
    numComponents = hecl::SBig(numComponents);
}

void CMoviePlayer::THPVideoInfo::swapBig()
{
    width = hecl::SBig(width);
    height = hecl::SBig(height);
}

void CMoviePlayer::THPAudioInfo::swapBig()
{
    numChannels = hecl::SBig(numChannels);
    sampleRate = hecl::SBig(sampleRate);
    numSamples = hecl::SBig(numSamples);
}

void CMoviePlayer::THPFrameHeader::swapBig()
{
    nextSize = hecl::SBig(nextSize);
    prevSize = hecl::SBig(prevSize);
    imageSize = hecl::SBig(imageSize);
    audioSize = hecl::SBig(audioSize);
}

void CMoviePlayer::THPAudioFrameHeader::swapBig()
{
    channelSize = hecl::SBig(channelSize);
    numSamples = hecl::SBig(numSamples);
    for (int i=0 ; i<2 ; ++i)
    {
        for (int j=0 ; j<8 ; ++j)
        {
            channelCoefs[i][j][0] = hecl::SBig(channelCoefs[i][j][0]);
            channelCoefs[i][j][1] = hecl::SBig(channelCoefs[i][j][1]);
        }
        channelPrevs[i][0] = hecl::SBig(channelPrevs[i][0]);
        channelPrevs[i][1] = hecl::SBig(channelPrevs[i][1]);
    }
}

/* Slightly modified THPAudioDecode present in SDK, always interleaves */
u32 CMoviePlayer::THPAudioDecode(s16* buffer, const u8* audioFrame, bool stereo)
{
    THPAudioFrameHeader header = *((const THPAudioFrameHeader*)audioFrame);
    header.swapBig();
    audioFrame += sizeof(THPAudioFrameHeader);

    if (stereo)
    {
        for (int i=0 ; i<2 ; ++i)
        {
            unsigned samples = header.numSamples;
            s16* bufferCur = buffer+i;
            const u8* audioFrameCur = audioFrame;
            int16_t prev1 = header.channelPrevs[i][0];
            int16_t prev2 = header.channelPrevs[i][1];
            for (int f=0 ; f<header.channelSize/8 ; ++f)
            {
                DSPDecompressFrameStereoStride(bufferCur, audioFrameCur,
                                               header.channelCoefs[i], &prev1, &prev2, samples);
                samples -= 14;
                bufferCur += 28;
                audioFrameCur += 8;
            }
        }
    }
    else
    {
        unsigned samples = header.numSamples;
        s16* bufferCur = buffer;
        const u8* audioFrameCur = audioFrame;
        int16_t prev1 = header.channelPrevs[0][0];
        int16_t prev2 = header.channelPrevs[0][1];
        for (int f=0 ; f<header.channelSize/8 ; ++f)
        {
            DSPDecompressFrameStereoDupe(bufferCur, audioFrameCur,
                                         header.channelCoefs[0], &prev1, &prev2, samples);
            samples -= 14;
            bufferCur += 28;
            audioFrameCur += 8;
        }
    }

    return header.numSamples;
}

CMoviePlayer::CMoviePlayer(const char* path, float preLoadSeconds, bool loop, bool deinterlace)
: CDvdFile(path), xec_preLoadSeconds(preLoadSeconds), xf4_24_loop(loop), m_deinterlace(deinterlace)
{
    u8 buf[64];
    SyncRead(buf, 64);
    memcpy(&x28_thpHead, buf, 48);
    x28_thpHead.swapBig();

    u32 cur = x28_thpHead.componentDataOffset;
    SyncSeekRead(buf, 32, ESeekOrigin::Begin, cur);
    memcpy(&x58_thpComponents, buf, 20);
    cur += 20;
    x58_thpComponents.swapBig();

    for (int i=0 ; i<x58_thpComponents.numComponents ; ++i)
    {
        switch (x58_thpComponents.comps[i])
        {
        case THPComponents::Type::Video:
            SyncSeekRead(buf, 32, ESeekOrigin::Begin, cur);
            memcpy(&x6c_videoInfo, buf, 8);
            cur += 8;
            x6c_videoInfo.swapBig();
            break;
        case THPComponents::Type::Audio:
            SyncSeekRead(buf, 32, ESeekOrigin::Begin, cur);
            memcpy(&x74_audioInfo, buf, 12);
            cur += 12;
            x74_audioInfo.swapBig();
            xf4_25_hasAudio = true;
            break;
        default: break;
        }
    }

    xe4_totalSeconds = x28_thpHead.numFrames / x28_thpHead.fps;
    if (xec_preLoadSeconds < 0.f)
    {
        /* Load whole video */
        xec_preLoadSeconds = xe4_totalSeconds;
        xf0_preLoadFrames = x28_thpHead.numFrames;
    }
    else if (xec_preLoadSeconds > 0.f)
    {
        /* Pre-load video portion */
        u32 frame = xec_preLoadSeconds * x28_thpHead.fps;
        xf0_preLoadFrames = std::min(frame, x28_thpHead.numFrames);
        xec_preLoadSeconds = std::min(xe4_totalSeconds, xec_preLoadSeconds);
    }

    if (xf0_preLoadFrames > 0)
        xa0_bufferQueue.reserve(xf0_preLoadFrames);

    m_blockBuf = CGraphics::g_BooFactory->newDynamicBuffer(boo::BufferUse::Vertex, sizeof(m_viewVertBlock), 1);

    boo::IVertexFormat* vtxFmt = YUVVTXFmt;
    if (CGraphics::g_BooFactory->bindingNeedsVertexFormat())
    {
        boo::VertexElementDescriptor texvdescs[] =
        {
            {m_blockBuf, nullptr, boo::VertexSemantic::Position4},
            {m_blockBuf, nullptr, boo::VertexSemantic::UV4}
        };
        vtxFmt = CGraphics::g_BooFactory->newVertexFormat(2, texvdescs);
    }

    x80_textures.reserve(3);
    for (int i=0 ; i<3 ; ++i)
    {
        x80_textures.emplace_back();
        CTHPTextureSet& set = x80_textures.back();
        if (deinterlace)
        {
            set.Y[0] = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width,
                                                                  x6c_videoInfo.height / 2,
                                                                  boo::TextureFormat::I8);
            set.Y[1] = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width,
                                                                  x6c_videoInfo.height / 2,
                                                                  boo::TextureFormat::I8);
            set.U = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width / 2,
                                                               x6c_videoInfo.height / 2,
                                                               boo::TextureFormat::I8);
            set.V = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width / 2,
                                                               x6c_videoInfo.height / 2,
                                                               boo::TextureFormat::I8);

            boo::IGraphicsBuffer* bufs[] = {m_blockBuf};
            for (int j=0 ; j<2 ; ++j)
            {
                boo::ITexture* texs[] = {set.Y[j], set.U, set.V};
                set.binding[j] = CGraphics::g_BooFactory->newShaderDataBinding(YUVShaderPipeline, vtxFmt, m_blockBuf,
                                                                               nullptr, nullptr, 1, bufs, 3, texs);
            }
        }
        else
        {
            set.Y[0] = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width,
                                                                  x6c_videoInfo.height,
                                                                  boo::TextureFormat::I8);
            set.U = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width / 2,
                                                               x6c_videoInfo.height / 2,
                                                               boo::TextureFormat::I8);
            set.V = CGraphics::g_BooFactory->newDynamicTexture(x6c_videoInfo.width / 2,
                                                               x6c_videoInfo.height / 2,
                                                               boo::TextureFormat::I8);

            boo::IGraphicsBuffer* bufs[] = {m_blockBuf};
            boo::ITexture* texs[] = {set.Y[0], set.U, set.V};
            set.binding[0] = CGraphics::g_BooFactory->newShaderDataBinding(YUVShaderPipeline, vtxFmt, m_blockBuf,
                                                                           nullptr, nullptr, 1, bufs, 3, texs);
        }
        if (xf4_25_hasAudio)
            set.audioBuf.reset(new s16[x28_thpHead.maxAudioSamples * 2]);
    }

    m_yuvBuf.reset(new uint8_t[tjBufSizeYUV(x6c_videoInfo.width, x6c_videoInfo.height, TJ_420)]);

    m_token = CGraphics::CommitResources();
    PostDVDReadRequestIfNeeded();
}

void CMoviePlayer::VerifyCallbackStatus()
{
}
void CMoviePlayer::DisableStaticAudio()
{
}
void CMoviePlayer::SetStaticAudioVolume(int vol)
{
}
void CMoviePlayer::SetStaticAudio(const void* data, u32 length, u32 loopStart, u32 loopEnd)
{
}
void CMoviePlayer::MixAudio(short* out, const short* in, u32 length)
{
}
void CMoviePlayer::MixStaticAudio(short* out, const short* in, u32 length)
{
}
void CMoviePlayer::StaticMyAudioCallback()
{
}
void CMoviePlayer::Rewind()
{
    if (x98_request)
    {
        x98_request->PostCancelRequest();
        x98_request.reset();
    }

    xb0_nextReadSize = x28_thpHead.firstFrameSize;
    xb4_nextReadOff = x28_thpHead.firstFrameOffset;
    xb8_readSize = x28_thpHead.firstFrameSize;
    xbc_readOff = x28_thpHead.firstFrameOffset;

    xc0_loadedFrames = 0;
    xc4_ = 0;
    xc8_curFrame = 0;
    xcc_decodedTexSlot = 0;
    xd0_ = -1;
    xd4_ = -1;
    xd8_ = 0;
    xdc_frameRem = 0.f;
    xe8_curSeconds = 0.f;
}

void CMoviePlayer::DrawFrame(const CVector3f& a, const CVector3f& b,
                             const CVector3f& c, const CVector3f& d)
{
}

void CMoviePlayer::Update(float dt)
{
    if (xc0_loadedFrames < xf0_preLoadFrames)
    {
        if (x98_request && x98_request->IsComplete())
        {
            ReadCompleted();
            if (xc0_loadedFrames >= xa0_bufferQueue.size() &&
                xc0_loadedFrames < xf0_preLoadFrames &&
                xa0_bufferQueue.size() < x28_thpHead.numFrames)
            {
                PostDVDReadRequestIfNeeded();
            }
        }
    }
    else
    {
        if (x98_request)
        {
            bool flag = false;
            if (xc4_ >= xa0_bufferQueue.size() && xc0_loadedFrames >= xa0_bufferQueue.size())
                flag = true;
            if (x98_request->IsComplete() && xd8_ < 2 && flag)
            {
                DecodeFromRead(x90_requestBuf.get());
                ReadCompleted();
                PostDVDReadRequestIfNeeded();
                ++xd8_;
                ++xc4_;
                if (xc4_ >= x28_thpHead.numFrames && xf4_24_loop)
                    xc4_ = 0;
            }
        }
    }

    if (!x98_request && xe0_playMode == EPlayMode::Playing &&
        xa0_bufferQueue.size() < x28_thpHead.numFrames)
    {
        PostDVDReadRequestIfNeeded();
    }

    if (xd8_ < 2)
    {
        if (xe0_playMode == EPlayMode::Playing && xc4_ < xf0_preLoadFrames)
        {
            u32 minFrame = std::min(u32(xa0_bufferQueue.size()) - 1, xc4_);
            if (minFrame == -1)
                return;
            std::unique_ptr<uint8_t[]>& frameData = xa0_bufferQueue[minFrame];
            DecodeFromRead(frameData.get());
            ++xd8_;
            ++xc4_;
            if (xc4_ >= x28_thpHead.numFrames && xf4_24_loop)
                xc4_ = 0;
        }
    }

    if (xd8_ <= 0 || xe0_playMode != EPlayMode::Playing)
        return;

    xe8_curSeconds += dt;
    if (xf4_24_loop)
        xe8_curSeconds = std::fmod(xe8_curSeconds, xe4_totalSeconds);
    else
        xe8_curSeconds = std::min(xe4_totalSeconds, xe8_curSeconds);

    float frameDt = 1.f / x28_thpHead.fps;
    float rem = xdc_frameRem - dt;
    if (rem <= 0.f)
    {
        if (!xf4_26_fieldFlip)
        {
            ++xd0_;
            if (xd0_ >= x80_textures.size())
                xd0_ = 0;
            if (xd4_ == -1)
                xd4_ = 0;
            --xd8_;
            ++xc8_curFrame;
            if (xc8_curFrame == x28_thpHead.numFrames && xf4_24_loop)
                xc8_curFrame = 0;
            rem += frameDt;
            xfc_ = 0;
        }
        else
        {
            rem += dt;
            xf4_26_fieldFlip = false;
        }
    }
    xdc_frameRem = rem;
}

void CMoviePlayer::DecodeFromRead(const void* data)
{
    const u8* inptr = (u8*)data;
    CTHPTextureSet& tex = x80_textures[xcc_decodedTexSlot];

    THPFrameHeader frameHeader = *static_cast<const THPFrameHeader*>(data);
    frameHeader.swapBig();
    inptr += 8 + x58_thpComponents.numComponents * 4;

    for (int i=0 ; i<x58_thpComponents.numComponents ; ++i)
    {
        switch (x58_thpComponents.comps[i])
        {
        case THPComponents::Type::Video:
        {
            tjDecompressToYUV(TjHandle, (u8*)inptr, frameHeader.imageSize, m_yuvBuf.get(), 0);
            inptr += frameHeader.imageSize;

            uintptr_t planeSize = x6c_videoInfo.width * x6c_videoInfo.height;
            uintptr_t planeSizeHalf = planeSize / 2;
            uintptr_t planeSizeQuarter = planeSizeHalf / 2;

            if (m_deinterlace)
            {
                /* Deinterlace into 2 discrete 60-fps half-res textures */
                u8* mappedData = (u8*)tex.Y[0]->map(planeSizeHalf);
                for (unsigned y=0 ; y<x6c_videoInfo.height/2 ; ++y)
                {
                    memcpy(mappedData + x6c_videoInfo.width*y,
                           m_yuvBuf.get() + x6c_videoInfo.width*(y*2),
                           x6c_videoInfo.width);
                }
                tex.Y[0]->unmap();

                mappedData = (u8*)tex.Y[1]->map(planeSizeHalf);
                for (unsigned y=0 ; y<x6c_videoInfo.height/2 ; ++y)
                {
                    memcpy(mappedData + x6c_videoInfo.width*y,
                           m_yuvBuf.get() + x6c_videoInfo.width*(y*2+1),
                           x6c_videoInfo.width);
                }
                tex.Y[1]->unmap();

                tex.U->load(m_yuvBuf.get() + planeSize, planeSizeQuarter);
                tex.V->load(m_yuvBuf.get() + planeSize + planeSizeQuarter, planeSizeQuarter);
            }
            else
            {
                /* Direct planar load */
                tex.Y[0]->load(m_yuvBuf.get(), planeSize);
                tex.U->load(m_yuvBuf.get() + planeSize, planeSizeQuarter);
                tex.V->load(m_yuvBuf.get() + planeSize + planeSizeQuarter, planeSizeQuarter);
            }

            break;
        }
        case THPComponents::Type::Audio:
            THPAudioDecode(tex.audioBuf.get(), (u8*)inptr, x74_audioInfo.numChannels == 2);
            inptr += frameHeader.audioSize;
            break;
        default: break;
        }
    }

    ++xcc_decodedTexSlot;
    if (xcc_decodedTexSlot == x80_textures.size())
        xcc_decodedTexSlot = 0;
}

void CMoviePlayer::ReadCompleted()
{
}

void CMoviePlayer::PostDVDReadRequestIfNeeded()
{
    if (xc0_loadedFrames < x28_thpHead.numFrames)
    {
        x90_requestBuf.reset(new uint8_t[xb0_nextReadSize]);
        x98_request = AsyncSeekRead(x90_requestBuf.get(), xb0_nextReadSize,
                                    ESeekOrigin::Begin, xb4_nextReadOff);
    }
}

}
