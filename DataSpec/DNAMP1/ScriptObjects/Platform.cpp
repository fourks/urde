#include "Platform.hpp"
#include "hecl/Blender/Connection.hpp"

namespace DataSpec::DNAMP1
{

zeus::CAABox Platform::getVISIAABB(hecl::blender::Token& btok) const
{
    hecl::blender::Connection& conn = btok.getBlenderConnection();
    zeus::CAABox aabbOut;

    if (model)
    {
        hecl::ProjectPath path = UniqueIDBridge::TranslatePakIdToPath(model);
        conn.openBlend(path);
        hecl::blender::DataStream ds = conn.beginData();
        auto aabb = ds.getMeshAABB();
        aabbOut = zeus::CAABox(aabb.first, aabb.second);
    }
    else if (animationParameters.animationCharacterSet)
    {
        hecl::ProjectPath path = UniqueIDBridge::TranslatePakIdToPath(
            animationParameters.animationCharacterSet);
        conn.openBlend(path.getWithExtension(_S(".blend"), true));
        hecl::blender::DataStream ds = conn.beginData();
        auto aabb = ds.getMeshAABB();
        aabbOut = zeus::CAABox(aabb.first, aabb.second);
    }

    if (aabbOut.min.x > aabbOut.max.x)
        return {};

    zeus::CTransform xf = ConvertEditorEulerToTransform4f(scale, orientation, location);
    return aabbOut.getTransformedAABox(xf);
}

}
