#include <iostream>
#include <cstdint>
#include <vector>
#include "tinyxml2/tinyxml2.h"
#include "logvisor/logvisor.hpp"

#ifndef _WIN32
#include <cstdlib>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <cerrno>
#else
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN 1
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>
#include <cwchar>
#if UNICODE
#define IS_UCS2 1
#endif
#endif


static logvisor::Module Log("AssetNameParser");
// TODO: Clean this up
#undef bswap16
#undef bswap32
#undef bswap64

/* Type-sensitive byte swappers */
template <typename T>
static inline T bswap16(T val)
{
#if __GNUC__
    return __builtin_bswap16(val);
#elif _WIN32
    return _byteswap_ushort(val);
#else
    return (val = (val << 8) | ((val >> 8) & 0xFF));
#endif
}

template <typename T>
static inline T bswap32(T val)
{
#if __GNUC__
    return __builtin_bswap32(val);
#elif _WIN32
    return _byteswap_ulong(val);
#else
    val = (val & 0x0000FFFF) << 16 | (val & 0xFFFF0000) >> 16;
    val = (val & 0x00FF00FF) << 8 | (val & 0xFF00FF00) >> 8;
    return val;
#endif
}

template <typename T>
static inline T bswap64(T val)
{
#if __GNUC__
    return __builtin_bswap64(val);
#elif _WIN32
    return _byteswap_uint64(val);
#else
    return ((val & 0xFF00000000000000ULL) >> 56) |
           ((val & 0x00FF000000000000ULL) >> 40) |
           ((val & 0x0000FF0000000000ULL) >> 24) |
           ((val & 0x000000FF00000000ULL) >>  8) |
           ((val & 0x00000000FF000000ULL) <<  8) |
           ((val & 0x0000000000FF0000ULL) << 24) |
           ((val & 0x000000000000FF00ULL) << 40) |
           ((val & 0x00000000000000FFULL) << 56);
#endif
}


#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
static inline int16_t SBig(int16_t val) {return bswap16(val);}
static inline uint16_t SBig(uint16_t val) {return bswap16(val);}
static inline int32_t SBig(int32_t val) {return bswap32(val);}
static inline uint32_t SBig(uint32_t val) {return bswap32(val);}
static inline int64_t SBig(int64_t val) {return bswap64(val);}
static inline uint64_t SBig(uint64_t val) {return bswap64(val);}
static inline float SBig(float val)
{
    int32_t ival = bswap32(*((int32_t*)(&val)));
    return *((float*)(&ival));
}
static inline double SBig(double val)
{
    int64_t ival = bswap64(*((int64_t*)(&val)));
    return *((double*)(&ival));
}
#ifndef SBIG
#define SBIG(q) ( ( (q) & 0x000000FF ) << 24 | ( (q) & 0x0000FF00 ) <<  8 \
                | ( (q) & 0x00FF0000 ) >>  8 | ( (q) & 0xFF000000 ) >> 24 )
#endif

static inline int16_t SLittle(int16_t val) {return val;}
static inline uint16_t SLittle(uint16_t val) {return val;}
static inline int32_t SLittle(int32_t val) {return val;}
static inline uint32_t SLittle(uint32_t val) {return val;}
static inline int64_t SLittle(int64_t val) {return val;}
static inline uint64_t SLittle(uint64_t val) {return val;}
static inline float SLittle(float val) {return val;}
static inline double SLittle(double val) {return val;}
#ifndef SLITTLE
#define SLITTLE(q) (q)
#endif
#else
static inline int16_t SLittle(int16_t val) {return bswap16(val);}
static inline uint16_t SLittle(uint16_t val) {return bswap16(val);}
static inline int32_t SLittle(int32_t val) {return bswap32(val);}
static inline uint32_t SLittle(uint32_t val) {return bswap32(val);}
static inline int64_t SLittle(int64_t val) {return bswap64(val);}
static inline uint64_t SLittle(uint64_t val) {return bswap64(val);}
static inline float SLittle(float val)
{
    int32_t ival = bswap32(*((int32_t*)(&val)));
    return *((float*)(&ival));
}
static inline double SLittle(double val)
{
    int64_t ival = bswap64(*((int64_t*)(&val)));
    return *((double*)(&ival));
}
#ifndef SLITTLE
#define SLITTLE(q) ( ( (q) & 0x000000FF ) << 24 | ( (q) & 0x0000FF00 ) <<  8 \
                   | ( (q) & 0x00FF0000 ) >>  8 | ( (q) & 0xFF000000 ) >> 24 )
#endif

static inline int16_t SBig(int16_t val) {return val;}
static inline uint16_t SBig(uint16_t val) {return val;}
static inline int32_t SBig(int32_t val) {return val;}
static inline uint32_t SBig(uint32_t val) {return val;}
static inline int64_t SBig(int64_t val) {return val;}
static inline uint64_t SBig(uint64_t val) {return val;}
static inline float SBig(float val) {return val;}
static inline double SBig(double val) {return val;}
#ifndef SBIG
#define SBIG(q) (q)
#endif
#endif

class FourCC
{
protected:
    union
    {
        char fcc[4];
        uint32_t num;
    };
public:
    FourCC() /* Sentinel FourCC */
    : num(0) {}
    FourCC(const FourCC& other)
    {num = other.num;}
    FourCC(const char* name)
    : num(*(uint32_t*)name) {}
    FourCC(uint32_t n)
    : num(n) {}
    bool operator==(const FourCC& other) const {return num == other.num;}
    bool operator!=(const FourCC& other) const {return num != other.num;}
    bool operator==(const char* other) const {return num == *(uint32_t*)other;}
    bool operator!=(const char* other) const {return num != *(uint32_t*)other;}
    bool operator==(int32_t other) const { return num == other;}
    bool operator!=(int32_t other) const { return num != other;}
    bool operator==(uint32_t other) const {return num == other;}
    bool operator!=(uint32_t other) const {return num != other;}
    std::string toString() const {return std::string(fcc, 4);}
    uint32_t toUint32() const {return num;}
    operator uint32_t() const {return num;}
};

struct SAsset
{
    FourCC type;
    uint64_t id;
    std::string name;
    std::string dir;
};

enum class FileLockType
{
    None = 0,
    Read,
    Write
};

#if IS_UCS2
typedef wchar_t SystemChar;
static inline size_t StrLen(const SystemChar* str) { return wcslen(str); }
typedef std::wstring SystemString;
#ifndef _S
#define _S(val) L##val
#endif
typedef struct _stat Sstat;
#else
typedef char SystemChar;
static inline size_t StrLen(const SystemChar* str) { return strlen(str); }
typedef std::string SystemString;
#ifndef _S
#define _S(val) val
#endif
typedef struct stat Sstat;
#endif

static inline int StrCmp(const SystemChar* str1, const SystemChar* str2)
{
#if IS_UCS2
    return wcscmp(str1, str2);
#else
    return strcmp(str1, str2);
#endif
}

static inline FILE* Fopen(const SystemChar* path, const SystemChar* mode, FileLockType lock = FileLockType::None)
{
#if IS_UCS2
    FILE* fp = _wfopen(path, mode);
    if (!fp)
        return nullptr;
#else
    FILE* fp = fopen(path, mode);
    if (!fp)
        return nullptr;
#endif

    if (lock != FileLockType::None)
    {
#if _WIN32
        OVERLAPPED ov = {};
        LockFileEx((HANDLE)(uintptr_t)_fileno(fp), (lock == FileLockType::Write) ? LOCKFILE_EXCLUSIVE_LOCK : 0, 0, 0, 1,
                   &ov);
#else
        if (flock(fileno(fp), ((lock == FileLockType::Write) ? LOCK_EX : LOCK_SH) | LOCK_NB))
            fprintf(stderr, "flock %s: %s", path, strerror(errno));
#endif
    }

    return fp;
}


#if _WIN32
int wmain(int argc, const wchar_t* argv[])
#else
int main(int argc, const char* argv[])
#endif
{
    logvisor::RegisterStandardExceptions();
    logvisor::RegisterConsoleLogger();
    if (argc < 3)
    {
        Log.report(logvisor::Error, _S("Usage: %s <input> <output>"), argv[0]);
        return 1;
    }

    SystemString inPath = argv[1];
    SystemString outPath = argv[2];

    tinyxml2::XMLDocument doc;
    std::vector<SAsset> assets;
    FILE* docF = Fopen(inPath.c_str(), _S("rb"));
    if (!doc.LoadFile(docF))
    {
        const tinyxml2::XMLElement* elm = doc.RootElement();
        if (strcmp(elm->Name(), "AssetNameMap"))
        {
            Log.report(logvisor::Fatal, _S("Invalid database supplied"));
            return 1;
        }

        elm = elm->FirstChildElement("AssetNameMap");
        if (elm == nullptr)
        {
            Log.report(logvisor::Fatal, _S("Malformed AssetName database"));
            return 1;
        }

        elm = elm->FirstChildElement("Asset");

        while (elm)
        {
            const tinyxml2::XMLElement* keyElm = elm->FirstChildElement("Key");
            const tinyxml2::XMLElement* valueElm = elm->FirstChildElement("Value");

            if (!keyElm || !valueElm)
            {
                Log.report(logvisor::Fatal, _S("Malformed Asset entry, [Key,Value] required"));
                return 0;
            }

            const tinyxml2::XMLElement* nameElm = valueElm->FirstChildElement("Name");
            const tinyxml2::XMLElement* dirElm = valueElm->FirstChildElement("Directory");
            const tinyxml2::XMLElement* typeElm = valueElm->FirstChildElement("Type");

            if (!nameElm || !dirElm || ! typeElm)
            {
                Log.report(logvisor::Fatal, _S("Malformed Value entry, [Name,Directory,Type] required"));
                return 0;
            }
            assets.emplace_back();
            SAsset& asset = assets.back();
            asset.type = typeElm->GetText();
            asset.id = strtoull(keyElm->GetText(), nullptr, 16);
            asset.name = nameElm->GetText();
            asset.dir = dirElm->GetText();
            elm = elm->NextSiblingElement("Asset");
        }

        FILE* f = Fopen(outPath.c_str(), _S("wb"));
        if (!f)
        {
            Log.report(logvisor::Fatal, _S("Unable to open destination"));
            return 0;
        }

        uint32_t assetCount = SBig(uint32_t(assets.size()));
        FourCC sentinel(SBIG('AIDM'));
        fwrite(&sentinel, 1, 4, f);
        fwrite(&assetCount, 1, 4, f);
        for (const SAsset& asset : assets)
        {
            fwrite(&asset.type, 1, 4, f);
            uint64_t id = SBig(asset.id);
            fwrite(&id, 1, 8, f);
            uint32_t tmp = SBig(uint32_t(asset.name.length()));
            fwrite(&tmp, 1, 4, f);
            fwrite(asset.name.c_str(), 1, SBig(tmp), f);
            tmp = SBig(uint32_t(asset.dir.length()));
            fwrite(&tmp, 1, 4, f);
            fwrite(asset.dir.c_str(), 1, SBig(tmp), f);
        }
        fflush(f);
        fclose(f);
        fclose(docF);
        return 0;
    }

    if (docF)
        fclose(docF);

    Log.report(logvisor::Fatal, _S("failed to load"));
    return 1;
}
