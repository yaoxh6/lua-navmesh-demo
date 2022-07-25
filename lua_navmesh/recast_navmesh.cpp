#include "recast_navmesh.h"
#include "lua_log.h"

const FVector FVector::ZeroVector(0.0f, 0.0f, 0.0f);
FVector::FVector(dtReal InX, dtReal InY, dtReal InZ)
    : x(InX)
    , y(InY)
    , z(InZ)
{
}
FVector::FVector()
    : x(0)
    , y(0)
    , z(0) 
{};
FVector Unreal2RecastPoint(const dtReal* UnrealPoint) { return FVector(-UnrealPoint[0], UnrealPoint[2], -UnrealPoint[1]); }

FVector Unreal2RecastPoint(const FVector& UnrealPoint) { return FVector(-UnrealPoint.x, UnrealPoint.z, -UnrealPoint.y); }

FVector Recast2UnVector(const dtReal* RecastPoint) { return FVector(-RecastPoint[0], -RecastPoint[2], RecastPoint[1]); }

bool FVector::operator==(const FVector& V) const { return x == V.x && y == V.y && z == V.z; }

const FVector recast_navmesh::DefaultNavQueryExtent(50.0f, 50.0f, 250.0f);
recast_navmesh::recast_navmesh()
    : includeFlag(0)
    , excludeFlag(0)
    , NavMesh(0)
    , NavQuery(0)
{
    queryFilter = dtQueryFilter();
    NavQuery = nullptr;
}

recast_navmesh::~recast_navmesh()
{
    if (NavMesh) {
        dtFreeNavMesh(NavMesh);
        NavMesh = nullptr;
    }
    if (NavQuery) {
        dtFreeNavMeshQuery(NavQuery);
        NavQuery = nullptr;
    }
}

bool recast_navmesh::LoadNavMeshFile(const char* InNavDataPath)
{
    if (!NavMesh) {
        NavMesh = dtAllocNavMesh();
        if (!NavMesh) {
            LUA_LOG_ERR("recast_navmesh::LoadNavMeshFile dtAllocNavMesh failed\n");
            return false;
        }
    }

    FILE* pd = fopen(InNavDataPath, "rb");
    if (!pd) {
        LUA_LOG_ERR("recast_navmesh::LoadNavMeshFile open file %s failed\n", InNavDataPath);
        return false;
    }

    bool bLoadRet = LoadNavMeshFileInner(NavMesh, pd);
    fclose(pd);
    return bLoadRet;
}

bool recast_navmesh::LoadNavMeshFileInner(dtNavMesh* NavMesh, FILE* pd)
{
    NavMeshSetHeader Header;
    auto count = fread((uint8_t*)(&Header), sizeof(Header), 1, pd);
    if (count != 1) {
        LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner read NavMeshSetHeader failed\n");
        return false;
    }

    if (Header.magic != NavmeshSetMagic || Header.version != NavmeshSetVersion || Header.numTiles <= 0) {
        LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner NavMeshSetHeader check failed\n");
        return false;
    }

    dtStatus Status = NavMesh->init(&Header.params);
    if (!dtStatusSucceed(Status)) {
        LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner NavMesh init failed\n");
        return false;
    }
    for (int i = 0; i < Header.numTiles; ++i) {
        NavMeshTileHeader TileHeader;
        auto count = fread((uint8_t*)(&TileHeader), sizeof(TileHeader), 1, pd);
        if (count != 1 || TileHeader.dataSize <= 0 || TileHeader.dataSize > MaxDataSize) {
            LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner NavMeshTileHeader check failed\n");
            return false;
        }
        uint8_t* data = reinterpret_cast<uint8_t*>(dtAlloc(TileHeader.dataSize, DT_ALLOC_PERM_NAVMESH));
        if (!data) {
            LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner NavMeshTileHeader dtAlloc failed\n");
            return false;
        }
        count = fread(data, TileHeader.dataSize, 1, pd);
        if (count != 1) {
            LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner NavMeshTileHeader read failed\n");
            return false;
        }
        Status = NavMesh->addTile(data, TileHeader.dataSize, DT_TILE_FREE_DATA, TileHeader.tileRef, nullptr);
        if (!dtStatusSucceed(Status)) {
            LUA_LOG_ERR("recast_navmesh::LoadNavMeshFileInner addTile failed\n");
            return false;
        }
    }
    return true;
}

bool recast_navmesh::InitNavQuery(uint16_t includeFlag, uint16_t excludeFlag, FVector inNavQueryExtent)
{
    if (includeFlag == 0) {
        includeFlag = NAVQUERY_DEFAULT_INCLUDE_FLAG;
    }
    queryFilter.setIncludeFlags(includeFlag);
    queryFilter.setExcludeFlags(excludeFlag);
    if (inNavQueryExtent == FVector::ZeroVector) {
        NavQueryExtent = DefaultNavQueryExtent;
    } else {
        NavQueryExtent = inNavQueryExtent;
    }
    if (NavQuery) {
        dtFreeNavMeshQuery(NavQuery);
        NavQuery = nullptr;
    }
    NavQuery = dtAllocNavMeshQuery();
    if (!NavQuery) {
        LUA_LOG_ERR("recast_navmesh::InitNavQuery NavQuery dtAllocNavMeshQuery failed\n");
        return false;
    }
    dtStatus initStatus = NavQuery->init(NavMesh, MAX_NAV_SEARCH_NODES);
    if (!dtStatusSucceed(initStatus)) {
        LUA_LOG_ERR("recast_navmesh::InitNavQuery NavQuery init failed\n");
        return false;
    }
    return true;
}
    
bool recast_navmesh::GetPathRef(FVector& UnrealStart, FVector& UnrealEnd, dtStatus& FindPathStatus, dtQueryResult& PathResult, FVector& RecastStartPos, FVector& RecastEndPos, dtPolyRef& StartPoly, dtPolyRef& EndPoly)
{
    const FVector RecastStartPosToProject = Unreal2RecastPoint(UnrealStart);
    const FVector RecastEndPosToProject = Unreal2RecastPoint(UnrealEnd);

    const dtReal Extent[3] = { NavQueryExtent.x, NavQueryExtent.z, NavQueryExtent.y };
    StartPoly = INVALID_NAVNODEREF;

    NavQuery->findNearestPoly(&RecastStartPosToProject.x, Extent, &queryFilter, &StartPoly, &RecastStartPos.x);
    if (StartPoly == INVALID_NAVNODEREF) {
        LUA_LOG_ERR("recast_navmesh::GetPathRef findNearestPoly x=%f, y=%f, z=%f, failed\n", UnrealStart.x, UnrealStart.y, UnrealStart.z);
        return false;
    }

    EndPoly = INVALID_NAVNODEREF;
    NavQuery->findNearestPoly(&RecastEndPosToProject.x, Extent, &queryFilter, &EndPoly, &RecastEndPos.x);
    if (EndPoly == INVALID_NAVNODEREF) {
        LUA_LOG_ERR("recast_navmesh::GetPathRef findNearestPoly x=%f, y=%f, z=%f, failed\n", UnrealEnd.x, UnrealEnd.y, UnrealEnd.z);
        return false;
    }

    FindPathStatus = NavQuery->findPath(StartPoly, EndPoly, &RecastStartPos.x, &RecastEndPos.x, FLT_MAX, &queryFilter, PathResult, 0);
    if (!dtStatusSucceed(FindPathStatus)) {
        LUA_LOG_ERR("recast_navmesh::GetPathRef findPath failed\n");
        return false;
    }
    return true;
}

bool recast_navmesh::GetPathPoint(dtStatus& FindPathStatus, dtQueryResult& PathResult, FVector& RecastStartPos, FVector& RecastEndPos, NavNodeRef StartPolyID, NavNodeRef EndPolyID)
{
    int32 PathSize = PathResult.size();
    std::vector<NavNodeRef> PathCorridor; // 寻路从Ref得到Location
    PathCorridor.resize(PathSize);
    if (PathSize == 1 && dtStatusDetail(FindPathStatus, DT_PARTIAL_RESULT)) {
        FVector RecastHandPlacedPathEnd;
        NavQuery->closestPointOnPolyBoundary(StartPolyID, &RecastEndPos.x, &RecastHandPlacedPathEnd.x);

        PathPoints.push_back(FNavLocation(Recast2UnVector(&RecastStartPos.x), StartPolyID));
        PathPoints.push_back( FNavLocation(Recast2UnVector(&RecastHandPlacedPathEnd.x), StartPolyID));
        PathCorridor.push_back(PathResult.getRef(0));
        return true;
    }

    PathCorridor.resize(PathSize);
    for (int i = 0; i < PathSize; ++i)
    {
        PathCorridor[i] = PathResult.getRef(i);
    }

    dtQueryResult StringPullResult;
    const dtStatus StringPullStatus = NavQuery->findStraightPath(&RecastStartPos.x, &RecastEndPos.x, PathCorridor.data(), PathCorridor.size(), StringPullResult, DT_STRAIGHTPATH_AREA_CROSSINGS);
    PathPoints.clear();
    if (!dtStatusSucceed(StringPullStatus))
    {
        LUA_LOG_ERR("recast_navmesh::GetPathPoint NavQuery findStraightPath failed\n");
        return false;
    }
    PathPoints.resize(StringPullResult.size());
    FNavLocation* CurVert = PathPoints.data();
    for (int32 VertIdx = 0; VertIdx < StringPullResult.size(); ++VertIdx)
    {
        const dtReal* CurRecastVert = StringPullResult.getPos(VertIdx);
        CurVert->Location = Recast2UnVector(CurRecastVert);
        CurVert->NodeRef = StringPullResult.getRef(VertIdx);
        CurVert++;
    }
    PathPoints.back().NodeRef = PathCorridor.back();
    return true;
}

bool recast_navmesh::FindPath(FVector& UnrealStart, FVector& UnrealEnd)
{
    dtStatus FindPathStatus;
    dtQueryResult PathResult;
    FVector RecastStartPos;
    FVector RecastEndPos;
    dtPolyRef StartPoly;
    dtPolyRef EndPoly;
    bool getPathRefRes = GetPathRef(UnrealStart, UnrealEnd, FindPathStatus, PathResult, RecastStartPos, RecastEndPos, StartPoly, EndPoly);
    if (!getPathRefRes) {
        return getPathRefRes;
    }
    bool getPathPointRes = GetPathPoint(FindPathStatus, PathResult, RecastStartPos, RecastEndPos, StartPoly, EndPoly);
    if (!getPathPointRes) {
        return getPathPointRes;
    }
    return true;
}

void recast_navmesh::SetFlags(uint16_t includeFlag, uint16_t excludeFlag) { 
    queryFilter.setIncludeFlags(includeFlag);
    queryFilter.setExcludeFlags(excludeFlag);
}

void recast_navmesh::SetExtent(FVector inNavQueryExtent) { 
    NavQueryExtent = inNavQueryExtent;
}
