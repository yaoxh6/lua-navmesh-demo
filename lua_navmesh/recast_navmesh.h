#pragma once
/*
移植UE的navmesh功能
目前功能只包含最简单的功能，输入两个点，得到路径
不支持功能：
1.不支持动态navmesh；
2.不支持customLink，即不支持在两个navmesh之间自定义链接
3.不支持分层navigation，即在大的navmesh下面还有一层小的navmesh
思路：
1.读取客户端navmesh文件，保存为NavMesh变量
2.通过NavMesh，初始化NavQuery
3.设置excludeFlag，includeFlag以及queryExtent
4.通过findPath得到所有路径的Ref
5.通过findStraightPath，根据Ref得到PathPoint
*/

#include "luna.h"
#include "Detour/DetourNavMesh.h"
#include "Detour/DetourAlloc.h" 
#include "Detour/DetourNavMeshQuery.h"
#include <vector>

static const int MaxDataSize = 100 * 1024 * 1024;
static const int NavmeshSetMagic = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; // MSET
static const int NavmeshSetVersion = 'U' << 24 | 'E' << 16 | 2;
static const uint32_t MAX_NAV_SEARCH_NODES = 2048;

#define INVALID_NAVNODEREF (0)
#define RECAST_STRAIGHTPATH_OFFMESH_CONNECTION 0x04
#define RECAST_MAX_AREAS 64
#define RECAST_DEFAULT_AREA (RECAST_MAX_AREAS - 1)
#define NAVQUERY_DEFAULT_INCLUDE_FLAG 0b111111111111111
#define DT_LARGE_WORLD_COORDINATES_DISABLED 0
#if DT_LARGE_WORLD_COORDINATES_DISABLED
typedef float dtReal;
#else // DT_LARGE_WORLD_COORDINATES_DISABLED
typedef double dtReal;
#endif // DT_LARGE_WORLD_COORDINATES_DISABLED

struct FVector {
    dtReal x;
    dtReal y;
    dtReal z;
    static const FVector ZeroVector;
    FVector();
    FVector(dtReal InX, dtReal InY, dtReal InZ);
    bool operator==(const FVector& V) const;
};

FVector Unreal2RecastPoint(const dtReal* UnrealPoint);
FVector Unreal2RecastPoint(const FVector& UnrealPoint);
FVector Recast2UnVector(const dtReal* R);

#if defined(__LP64__)
// LP64 (Linux/OS X): UE4 will define its uint64 type as "unsigned long long" so we need to match this
typedef unsigned long long UE4Type_uint64;
#else
#include <stdint.h>
typedef uint64_t UE4Type_uint64;
#endif

typedef UE4Type_uint64 NavNodeRef;
struct NavMeshSetHeader {
    int magic;
    int version;
    int numTiles;
    dtNavMeshParams params;
};             


struct NavMeshTileHeader {
    dtTileRef tileRef;
    int dataSize;
};

/** Describes a point in navigation data */
struct FNavLocation {
    /** location relative to path's base */
    FVector Location;

    /** node reference in navigation data */
    NavNodeRef NodeRef;

    FNavLocation()
        : Location(FVector::ZeroVector)
        , NodeRef(INVALID_NAVNODEREF)
    {
    }
    explicit FNavLocation(const FVector& InLocation, NavNodeRef InNodeRef = INVALID_NAVNODEREF)
        : Location(InLocation)
        , NodeRef(InNodeRef)
    {
    }

    /** checks if location has associated navigation node ref */
    bool HasNodeRef() const { return NodeRef != INVALID_NAVNODEREF; }

    operator FVector() const { return Location; }

    bool operator==(const FNavLocation& Other) const { return Location == Other.Location && NodeRef == Other.NodeRef; }
};

class recast_navmesh
{
public:
    recast_navmesh();
    ~recast_navmesh();
    /**
    * 读取客户端 ".navmesh"文件，并且赋值到NavMesh中
    * @reference:AI/Navigation/ESRecastNavMesh.cpp, AESRecastNavMesh::LoadNavMeshFile
    * @param[in] InNavDataPath - .navmesh文件路径
    * @return 是否读取成功
    */
    bool LoadNavMeshFile(const char* InNavDataPath);

    /**
     * 初始化NavQuery用于查询寻路相关,内部需要使用NavMesh
     * * @reference:NavMesh/PImpRecastNavMesh.cpp FPImplRecastNavMesh::FindPath INITIALIZE_NAVQUERY(NavQuery, InQueryFilter.GetMaxSearchNodes(), LinkFilter)
     * @param[in] includeFlag - 查询包含Flag
     * @param[in] excludeFlag - 查询排除Flag
     * @param[in] inNavQueryExtent - 查询范围的扩展
     * @return 是否初始化NavQuery成功
     */
    bool InitNavQuery(uint16_t includeFlag, uint16_t excludeFlag, FVector inNavQueryExtent);

    /**
     * 根据两个点得到中间路径，内部需要使用NavQuery
     * @param[in] UnrealStart - 查询起始点
     * @param[in] UnrealEnd - 查询结束点
     * @return 是否查询成功, 查询结果保存在PathPoints里
     */
    bool FindPath(FVector& UnrealStart, FVector& UnrealEnd);


    void SetFlags(uint16_t includeFlag, uint16_t excludeFlag);

    void SetExtent(FVector inNavQueryExtent);

    std::vector<FNavLocation>& GetPathPoint() { return PathPoints; }

private:
    // 客户端默认的NavQueryExtent
    static const FVector DefaultNavQueryExtent;
    // 查询包含的Flag
    uint16_t includeFlag;
    // 查询排除的Flag
    uint16_t excludeFlag;
    // 查询includeFlag和excludeFlag汇总
    dtQueryFilter queryFilter;
    // 影响查询的范围，和客户端保持一致即可,默认(50, 50, 250)
    FVector NavQueryExtent;
    // 导航网格数据,直接从客户端导出的".navmesh"中读取
    dtNavMesh* NavMesh;
    // 导航查询
    dtNavMeshQuery* NavQuery;
    // 查询结果
    std::vector<FNavLocation> PathPoints;
    // 读取客户端".navmesh"文件，并且赋值到NavMesh中
    bool LoadNavMeshFileInner(dtNavMesh* NavMesh, FILE* pb);

    /**
     * 根据两个点得到中间路径的Ref
     * @reference:NavMesh/PImpRecastNavMesh.cpp FPImplRecastNavMesh::FindPath
     * @param UnrealStart[in] - 起始位置
     * @param UnrealEnd[in] - 结束位置
     * @param FindPathStatus[out] - 查找路径的结果状态
     * @param PathResult[out] - 查找路径的结果Ref合集
     * @RecastStartPos[out] - 经过坐标系转换的起始位置
     * @RecastEndPos[out] - 经过坐标系转换的结束位置
     * @StartPoly[out] - 起始位置的Ref
     * @EndPoly[out] - 结束位置的Ref
     * @return 返回查询是否成功
     */
    bool GetPathRef(FVector& UnrealStart, FVector& UnrealEnd, dtStatus& FindPathStatus, dtQueryResult& PathResult, FVector& RecastStartPos, FVector& RecastStartEndPos, dtPolyRef& StartPoly, dtPolyRef& EndPoly);
    
    /**
     * 根据路径的Ref得到实际的路径点
     * @reference:NavMesh/PImpRecastNavMesh.cpp FPImplRecastNavMesh::PostProcessPathInternal
     * @reference:NavMesh/PImpRecastNavMesh.cpp FPImplRecastNavMesh::FindStraightPath
     * @param FindPathStatus[in] - 查找路径的结果状态
     * @param PathResult[in] - 查找路径的结果Ref合集
     * @RecastStartPos[in] - 经过坐标系转换的起始位置
     * @RecastEndPos[in] - 经过坐标系转换的结束位置
     * @StartPolyID[in] - 起始位置的Ref
     * @EndPolyID[in] - 结束位置的Ref
     * @return 返回查询是否成功，查询点的结果保存在PathPoints
     */
    bool GetPathPoint(dtStatus& FindPathStatus, dtQueryResult& PathResult, FVector& RecastStartPos, FVector& RecastEndPos, NavNodeRef StartPolyID, NavNodeRef EndPolyID);
};