#ifndef __URDE_CPAKFILE_HPP__
#define __URDE_CPAKFILE_HPP__

#include <vector>
#include "RetroTypes.hpp"
#include "CStringExtras.hpp"
#include "CDvdFile.hpp"
#include "CDvdRequest.hpp"
#include "CFactoryMgr.hpp"

namespace urde
{

class CPakFile : public CDvdFile
{
    friend class CResLoader;
public:
    struct SResInfo
    {
        CAssetId x0_id;
        bool x4_compressed : 1;
        CFactoryMgr::ETypeTable x4_typeIdx : 7;
        u32 x5_offsetDiv32 : 27;
        u32 x7_sizeDiv32 : 27;
        SResInfo(CAssetId id, FourCC fcc, u32 offset, u32 size, u32 flags)
        : x0_id(id)
        {
            x4_compressed = flags != 0;
            x4_typeIdx = CFactoryMgr::FourCCToTypeIdx(fcc);
            x5_offsetDiv32 = offset / 32;
            x7_sizeDiv32 = size / 32;
        }
        u32 GetOffset() const { return x5_offsetDiv32 * 32; }
        u32 GetSize() const { return x7_sizeDiv32 * 32; }
        FourCC GetType() const { return CFactoryMgr::TypeIdxToFourCC(x4_typeIdx); }
        bool IsCompressed() const { return x4_compressed; }
        CAssetId GetId() const { return x0_id; }
    };
private:
    union
    {
        struct
        {
            bool x28_24_buildDepList;
            bool x28_25_aramFile;
            bool x28_26_worldPak;
            bool x28_27_stashedInARAM;
        };
        u32 _dummy = 0;
    };
    enum class EAsyncPhase
    {
        Warmup = 0,
        InitialHeader = 1,
        DataLoad = 2,
        Loaded = 3
    } x2c_asyncLoadPhase = EAsyncPhase::Warmup;
    std::shared_ptr<IDvdRequest> x30_dvdReq; // Used to be auto_ptr
    std::vector<u8> x38_headerData;
    u32 x48_resTableOffset = 0;
    u32 x4c_resTableCount = 0;
    int x50_aramBase = -1;
    std::vector<std::pair<std::string, SObjectTag>> x54_nameList;
    std::vector<CAssetId> x64_depList;
    std::vector<SResInfo> x74_resList;
    mutable s32 x84_currentSeek = -1;
    void LoadResourceTable(athena::io::MemoryReader& r);
    void DataLoad();
    void InitialHeaderLoad();
    void Warmup();
public:
    CPakFile(std::string_view filename, bool buildDepList, bool worldPak);
    ~CPakFile();
    const std::vector<std::pair<std::string, SObjectTag>>& GetNameList() const { return x54_nameList; }
    const std::vector<CAssetId>& GetDepList() const { return x64_depList; }
    const SObjectTag* GetResIdByName(std::string_view name) const;
    const SResInfo* GetResInfoForLoadPreferForward(CAssetId id) const;
    const SResInfo* GetResInfoForLoadDirectionless(CAssetId id) const;
    const SResInfo* GetResInfo(CAssetId id) const;
    u32 GetFakeStaticSize() const { return 0; }
    void AsyncIdle();
};

}

#endif // __URDE_CPAKFILE_HPP__
