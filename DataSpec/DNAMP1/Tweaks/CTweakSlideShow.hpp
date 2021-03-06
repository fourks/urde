#ifndef _DNAMP1_CTWEAKSLIDESHOW_HPP_
#define _DNAMP1_CTWEAKSLIDESHOW_HPP_

#include "../../DNACommon/Tweaks/ITweakSlideShow.hpp"
#include "zeus/CColor.hpp"

namespace DataSpec::DNAMP1
{

struct CTweakSlideShow final : ITweakSlideShow
{
    AT_DECL_DNA_YAML

    String<-1> x4_pakName;
    String<-1> x14_fontAssetName;
    DNAColor x24_fontColor;
    DNAColor x28_outlineColor;
    Value<float> x2c_scanPercentInterval;
    Value<float> x30_;
    Value<float> x34_;
    Value<float> x38_;
    Value<float> x3c_;
    DNAColor x40_;
    Value<float> x44_;
    Value<float> x48_;
    Value<float> x4c_;
    Value<float> x50_;
    Value<float> x54_;
    Value<float> x58_;

    CTweakSlideShow() = default;
    CTweakSlideShow(athena::io::IStreamReader& in) { read(in); }

    std::string_view GetFont() const { return x14_fontAssetName; }
    const zeus::CColor& GetFontColor() const { return x24_fontColor; }
    const zeus::CColor& GetOutlineColor() const { return x28_outlineColor; }
    float GetScanPercentInterval() const { return x2c_scanPercentInterval; }
    float GetX54() const { return x54_; }
};

}

#endif // _DNAMP1_CTWEAKSLIDESHOW_HPP_
