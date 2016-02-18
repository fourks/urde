#ifndef __PSHAG_CLINERENDERERSHADERS_HPP__
#define __PSHAG_CLINERENDERERSHADERS_HPP__

#include "CGraphics.hpp"
#include "boo/graphicsdev/GL.hpp"
#include "boo/graphicsdev/D3D.hpp"
#include "boo/graphicsdev/Metal.hpp"

namespace pshag
{
class CLineRenderer;

class CLineRendererShaders
{
public:
    struct IDataBindingFactory
    {
        virtual void BuildShaderDataBinding(CLineRenderer& renderer, boo::IShaderPipeline* pipeline,
                                            boo::ITexture* texture)=0;
    };

private:
    static boo::IShaderPipeline* m_texAlpha;
    static boo::IShaderPipeline* m_texAdditive;

    static boo::IShaderPipeline* m_noTexAlpha;
    static boo::IShaderPipeline* m_noTexAdditive;

    static std::unique_ptr<IDataBindingFactory> m_bindFactory;
    static boo::GraphicsDataToken m_gfxToken;

public:
    static IDataBindingFactory* Initialize(boo::GLDataFactory& factory);
#if _WIN32
    static IDataBindingFactory* Initialize(boo::ID3DDataFactory& factory);
#elif BOO_HAS_METAL
    static IDataBindingFactory* Initialize(boo::MetalDataFactory& factory);
#endif

    static void Initialize();
    static void Shutdown();
    static void BuildShaderDataBinding(CLineRenderer& renderer, boo::ITexture* texture, bool additive);
};

}

#endif // __PSHAG_CLINERENDERERSHADERS_HPP__
