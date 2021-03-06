#ifndef __URDE_COBBTREE_HPP__
#define __URDE_COBBTREE_HPP__
#include "RetroTypes.hpp"
#include "CCollisionEdge.hpp"
#include "CCollisionSurface.hpp"
#include "zeus/CVector3f.hpp"
#include "zeus/COBBox.hpp"

namespace urde
{
class CCollidableOBBTreeGroupContainer;

class COBBTree
{
public:
    struct SIndexData
    {
        std::vector<u32> x0_materials;
        std::vector<u8> x10_vertMaterials;
        std::vector<u8> x20_edgeMaterials;
        std::vector<u8> x30_surfaceMaterials;
        std::vector<CCollisionEdge> x40_edges;
        std::vector<u16> x50_surfaceIndices;
        std::vector<zeus::CVector3f> x60_vertices;
        SIndexData()=default;
        SIndexData(CInputStream&);
    };

    class CLeafData
    {
        std::vector<u16> x0_surface;
    public:
        CLeafData()=default;
        CLeafData(const std::vector<u16>&);
        CLeafData(CInputStream&);

        const std::vector<u16>& GetSurfaceVector() const;
        size_t GetMemoryUsage() const;
    };

    class CNode
    {
        zeus::COBBox x0_obb;
        bool x3c_isLeaf = false;
        std::unique_ptr<CNode> x40_left;
        std::unique_ptr<CNode> x44_right;
        std::unique_ptr<CLeafData> x48_leaf;
        bool x4c_hit;
    public:
        CNode() = default;
        CNode(const CNode&)=default;
        CNode(const zeus::CTransform&, const zeus::CVector3f&, const CNode*, const CNode*, const CLeafData*);
        CNode(CInputStream&);

        bool WasHit() const { return x4c_hit; }
        void SetHit(bool h) { x4c_hit = h; }
        const CNode& GetLeft() const { return *x40_left; }
        const CNode& GetRight() const { return *x44_right; }
        const CLeafData& GetLeafData() const { return *x48_leaf; }
        const zeus::COBBox& GetOBB() const { return x0_obb; }
        size_t GetMemoryUsage() const;
        bool IsLeaf() const { return x3c_isLeaf; }
    };

private:

    u32 x0_magic = 0;
    u32 x4_version = 0;
    u32 x8_memsize = 0;
    /* CSimpleAllocator xc_ We're not using this but lets keep track*/
    SIndexData x18_indexData;
    std::unique_ptr<CNode> x88_root;
public:
    COBBTree()=default;
    COBBTree(const COBBTree::SIndexData&, const CNode*);
    COBBTree(CInputStream&);

    static std::unique_ptr<CCollidableOBBTreeGroupContainer>
    BuildOrientedBoundingBoxTree(const zeus::CVector3f&, const zeus::CVector3f&);
    CCollisionSurface GetSurface(u16 idx) const;
    const u16* GetTriangleEdgeIndices(u16 idx) const { return &x18_indexData.x50_surfaceIndices[idx * 3]; }
    void GetTriangleVertexIndices(u16 idx, u16 indicesOut[3]) const;
    const CCollisionEdge& GetEdge(int idx) const { return x18_indexData.x40_edges[idx]; }
    const zeus::CVector3f& GetVert(int idx) const { return x18_indexData.x60_vertices[idx]; }
    u32 GetVertMaterial(u16 idx) const { return x18_indexData.x0_materials[x18_indexData.x10_vertMaterials[idx]]; }
    u32 GetEdgeMaterial(u16 idx) const { return x18_indexData.x0_materials[x18_indexData.x20_edgeMaterials[idx]]; }
    CCollisionSurface GetTransformedSurface(u16 idx, const zeus::CTransform& xf) const;
    zeus::CAABox CalculateLocalAABox() const;
    zeus::CAABox CalculateAABox(const zeus::CTransform&) const;
    const CNode& GetRoot() const { return *x88_root; }
};
}

#endif // __URDE_COBBTREE_HPP__
