#include "NavMeshGenerater.h"
#include <recastnavigation/Recast.h>
#include <recastnavigation/DetourNavMesh.h>
#include <recastnavigation/DetourNavMeshBuilder.h>
#include <recastnavigation/DetourDebugDraw.h>
#include <recastnavigation/DetourCommon.h>
#include <recastnavigation/DetourPathCorridor.h>

Truth::NavMeshGenerater::NavMeshGenerater()
	: m_cellSize(0.3f)
	, m_cellHeight(0.2f)
	, m_agentMaxSlope(50.0f)
	, m_agentHeight(1.0f)
	, m_agentMaxClimb(1.0f)
	, m_agentRadius(0.01f)
	, m_edgeMaxLen(12.0f)
	, m_edgeMaxError(1.3f)
	, m_regionMinSize(0.1f)
	, m_regionMergeSize(1.0f)
	, m_vertsPerPoly(3.0f)
	, m_detailSampleDist(2.0f)
	, m_detailSampleMaxError(1.0f)
	, m_partitionType(0)
	, m_filterLowHangingObstacles(true)
	, m_filterLedgeSpans(true)
	, m_filterWalkableLowHeightSpans(true)
	, m_keepInterResults(true)
	, m_geom(NavGeom())
	, m_triareas(nullptr)
	, m_solid(nullptr)
	, m_pmesh(nullptr)
	, m_navMesh(nullptr)
	, m_dmesh(nullptr)
	, m_cset(nullptr)
	, m_chf(nullptr)
	, m_cfg(nullptr)
{
	m_ctx = new rcContext;
	m_navQuery = dtAllocNavMeshQuery();
	m_filter = new dtQueryFilter;

	m_filter->setIncludeFlags(SAMPLE_POLYFLAGS_ALL ^ SAMPLE_POLYFLAGS_DISABLED);
	m_filter->setExcludeFlags(0);

	m_filter->setAreaCost(SAMPLE_POLYAREA_GROUND, 1.0f);
	m_filter->setAreaCost(SAMPLE_POLYAREA_WATER, 10.0f);
	m_filter->setAreaCost(SAMPLE_POLYAREA_ROAD, 1.0f);
	m_filter->setAreaCost(SAMPLE_POLYAREA_DOOR, 1.0f);
	m_filter->setAreaCost(SAMPLE_POLYAREA_GRASS, 2.0f);
	m_filter->setAreaCost(SAMPLE_POLYAREA_JUMP, 1.5f);
}

Truth::NavMeshGenerater::~NavMeshGenerater()
{

}

void Truth::NavMeshGenerater::Initalize(std::wstring _path)
{
	m_geom.Load(_path);
	Initalize();
}

void Truth::NavMeshGenerater::Initalize(const std::vector<float>& _points, const std::vector<uint32>& _indices)
{
	m_geom.Load(_points, _indices);
	Initalize();
}

Vector3 Truth::NavMeshGenerater::FindPath(Vector3 _start, Vector3 _end, Vector3 _size)
{
	return _end;
	if (!m_navMesh)
		return _end;

	float spos[3] = { _start.x, _start.y, _start.z };
	// float spos[3] = { 10.0f, 10.0f, 10.0f };
	float epos[3] = { _end.x, _end.y, _end.z };
	float polyPickExt[3] = { 100, 100, 100 };

	dtPolyRef startRef = 0;
	dtPolyRef endRef = 0;
	dtPolyRef* polys = new dtPolyRef[MAX_POLYS];

	float* straightPath = new float[MAX_POLYS * 3];
	unsigned char* straightPathFlags = new unsigned char[MAX_POLYS];
	dtPolyRef* straightPathPolys = new dtPolyRef[MAX_POLYS];
	int straightPathOptions = 0;

	float testPoint[3] = { -1.0f, -1.0f, -1.0f };

	int npolys;
	int nstraightPath;
	dtStatus pathFindStatus;

	// m_navQuery->findRandomPoint(m_filter, frand, &startRef, spos);

	pathFindStatus = m_navQuery->findNearestPoly(spos, polyPickExt, m_filter, &startRef, testPoint);

	pathFindStatus = m_navQuery->findNearestPoly(epos, polyPickExt, m_filter, &endRef, testPoint);

	pathFindStatus = DT_FAILURE;

	if (startRef && endRef)
	{
		m_navQuery->findPath(startRef, endRef, spos, epos, m_filter, polys, &npolys, MAX_POLYS);
		nstraightPath = 0;
		if (npolys)
		{
			// In case of partial path, make sure the end point is clamped to the last polygon.
			float epos_[3];
			dtVcopy(epos_, epos);
			if (polys[npolys - 1] != endRef)
				m_navQuery->closestPointOnPoly(polys[npolys - 1], epos, epos_, 0);

			pathFindStatus = m_navQuery->findStraightPath(spos, epos_, polys, npolys,
				straightPath, straightPathFlags,
				straightPathPolys, &nstraightPath, MAX_POLYS, straightPathOptions);
		}
	}
	else
	{
		npolys = 0;
		nstraightPath = 0;
	}
	Vector3 result;
	if (!(pathFindStatus & DT_FAILURE))
	{
		result = Vector3(straightPath[3], straightPath[4], straightPath[5]);
	}
	else
	{
		result = _end;
	}

	delete[] polys;
	delete[] straightPath;
	delete[] straightPathFlags;
	delete[] straightPathPolys;

	return result;
}

void Truth::NavMeshGenerater::Destroy()
{
	delete m_solid;
	delete m_chf;
	delete m_cset;
	delete m_pmesh;
	delete m_cfg;
	delete m_dmesh;
	delete m_ctx;
	delete m_navMesh;
	delete m_navQuery;
	delete m_filter;

	m_solid = nullptr;
	m_chf = nullptr;
	m_cset = nullptr;
	m_pmesh = nullptr;
	m_cfg = nullptr;
	m_dmesh = nullptr;
	m_ctx = nullptr;
	m_navMesh = nullptr;
	m_navQuery = nullptr;
	m_filter = nullptr;

	m_geom.Destroy();
}

void Truth::NavMeshGenerater::Initalize()
{
	m_cfg = new rcConfig;

	const float* bmin = m_geom.m_bmin.data();
	const float* bmax = m_geom.m_bmax.data();
	const float* verts = m_geom.m_ver.data();
	const int32 nverts = static_cast<int32>(m_geom.m_ver.size()) / 3;
	const int32* tris = m_geom.m_inx.data();
	const int32 ntris = static_cast<int32>(m_geom.m_inx.size()) / 3;

	memset(m_cfg, 0, sizeof(m_cfg));
	m_cfg->cs = m_cellSize;
	m_cfg->ch = m_cellHeight;
	m_cfg->walkableSlopeAngle = m_agentMaxSlope;
	m_cfg->walkableHeight = static_cast<int32>(ceilf(m_agentHeight / m_cfg->ch));
	m_cfg->walkableClimb = static_cast<int32>(floorf(m_agentMaxClimb / m_cfg->ch));
	m_cfg->walkableRadius = static_cast<int32>(ceilf(m_agentRadius / m_cfg->cs));
	m_cfg->maxEdgeLen = static_cast<int32>(m_edgeMaxLen / m_cellSize);
	m_cfg->maxSimplificationError = m_edgeMaxError;
	m_cfg->minRegionArea = static_cast<int32>(rcSqr(m_regionMinSize));		// Note: area = size*size
	m_cfg->mergeRegionArea = static_cast<int32>(rcSqr(m_regionMergeSize));	// Note: area = size*size
	m_cfg->maxVertsPerPoly = static_cast<int32>(m_vertsPerPoly);
	m_cfg->detailSampleDist = m_detailSampleDist < 0.9f ? 0 : m_cellSize * m_detailSampleDist;
	m_cfg->detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	rcVcopy(m_cfg->bmin, bmin);
	rcVcopy(m_cfg->bmax, bmax);
	rcCalcGridSize(m_cfg->bmin, m_cfg->bmax, m_cfg->cs, &m_cfg->width, &m_cfg->height);

	m_solid = rcAllocHeightfield();
	bool c = rcCreateHeightfield(m_ctx, *m_solid, m_cfg->width, m_cfg->height, m_cfg->bmin, m_cfg->bmax, m_cfg->cs, m_cfg->ch);

	m_triareas = new unsigned char[ntris];
	memset(m_triareas, 0, ntris * sizeof(unsigned char));

	rcMarkWalkableTriangles(m_ctx, m_cfg->walkableSlopeAngle, verts, nverts, tris, ntris, m_triareas);
	c = rcRasterizeTriangles(m_ctx, verts, nverts, tris, m_triareas, ntris, *m_solid, m_cfg->walkableClimb);

	if (!m_keepInterResults)
	{
		delete[] m_triareas;
		m_triareas = 0;
	}

	if (m_filterLowHangingObstacles)
		rcFilterLowHangingWalkableObstacles(m_ctx, m_cfg->walkableClimb, *m_solid);
	if (m_filterLedgeSpans)
		rcFilterLedgeSpans(m_ctx, m_cfg->walkableHeight, m_cfg->walkableClimb, *m_solid);
	if (m_filterWalkableLowHeightSpans)
		rcFilterWalkableLowHeightSpans(m_ctx, m_cfg->walkableHeight, *m_solid);

	m_chf = rcAllocCompactHeightfield();
	c = rcBuildCompactHeightfield(m_ctx, m_cfg->walkableHeight, m_cfg->walkableClimb, *m_solid, *m_chf);

	c = rcErodeWalkableArea(m_ctx, m_cfg->walkableRadius, *m_chf);

	if (!m_keepInterResults)
	{
		rcFreeHeightField(m_solid);
		m_solid = 0;
	}

	c = rcErodeWalkableArea(m_ctx, m_cfg->walkableRadius, *m_chf);

	c = rcBuildDistanceField(m_ctx, *m_chf);
	c = rcBuildRegions(m_ctx, *m_chf, 0, m_cfg->minRegionArea, m_cfg->mergeRegionArea);

	m_cset = rcAllocContourSet();
	c = rcBuildContours(m_ctx, *m_chf, m_cfg->maxSimplificationError, m_cfg->maxEdgeLen, *m_cset);

	m_pmesh = rcAllocPolyMesh();
	c = rcBuildPolyMesh(m_ctx, *m_cset, m_cfg->maxVertsPerPoly, *m_pmesh);

	m_dmesh = rcAllocPolyMeshDetail();
	c = rcBuildPolyMeshDetail(m_ctx, *m_pmesh, *m_chf, m_cfg->detailSampleDist, m_cfg->detailSampleMaxError, *m_dmesh);

	if (!m_keepInterResults)
	{
		rcFreeCompactHeightfield(m_chf);
		m_chf = 0;
		rcFreeContourSet(m_cset);
		m_cset = 0;
	}


	if (m_cfg->maxVertsPerPoly <= DT_VERTS_PER_POLYGON)
	{
		unsigned char* navData = 0;
		int navDataSize = 0;

		// Update poly flags from areas.
		for (int i = 0; i < m_pmesh->npolys; ++i)
		{
			if (m_pmesh->areas[i] == RC_WALKABLE_AREA)
				m_pmesh->areas[i] = SAMPLE_POLYAREA_GROUND;

			if (m_pmesh->areas[i] == SAMPLE_POLYAREA_GROUND ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_GRASS ||
				m_pmesh->areas[i] == SAMPLE_POLYAREA_ROAD)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_WATER)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_SWIM;
			}
			else if (m_pmesh->areas[i] == SAMPLE_POLYAREA_DOOR)
			{
				m_pmesh->flags[i] = SAMPLE_POLYFLAGS_WALK | SAMPLE_POLYFLAGS_DOOR;
			}
		}


		dtNavMeshCreateParams params;
		memset(&params, 0, sizeof(params));
		params.verts = m_pmesh->verts;
		params.vertCount = m_pmesh->nverts;
		params.polys = m_pmesh->polys;
		params.polyAreas = m_pmesh->areas;
		params.polyFlags = m_pmesh->flags;
		params.polyCount = m_pmesh->npolys;
		params.nvp = m_pmesh->nvp;
		params.detailMeshes = m_dmesh->meshes;
		params.detailVerts = m_dmesh->verts;
		params.detailVertsCount = m_dmesh->nverts;
		params.detailTris = m_dmesh->tris;
		params.detailTriCount = m_dmesh->ntris;
		params.walkableHeight = m_agentHeight;
		params.walkableRadius = m_agentRadius;
		params.walkableClimb = m_agentMaxClimb;
		rcVcopy(params.bmin, m_pmesh->bmin);
		rcVcopy(params.bmax, m_pmesh->bmax);
		params.cs = m_cfg->cs;
		params.ch = m_cfg->ch;
		params.buildBvTree = true;

		// 		params.offMeshConVerts = m_geom->getOffMeshConnectionVerts();
		// 		params.offMeshConRad = m_geom->getOffMeshConnectionRads();
		// 		params.offMeshConDir = m_geom->getOffMeshConnectionDirs();
		// 		params.offMeshConAreas = m_geom->getOffMeshConnectionAreas();
		// 		params.offMeshConFlags = m_geom->getOffMeshConnectionFlags();
		// 		params.offMeshConUserID = m_geom->getOffMeshConnectionId();
		// 		params.offMeshConCount = m_geom->getOffMeshConnectionCount();

		if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not build Detour navmesh.");
		}

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh)
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not create Detour navmesh");
		}

		dtStatus status;

		status = m_navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
		if (dtStatusFailed(status))
		{
			dtFree(navData);
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh");
		}

		status = m_navQuery->init(m_navMesh, 2048);
		if (dtStatusFailed(status))
		{
			m_ctx->log(RC_LOG_ERROR, "Could not init Detour navmesh query");
		}
	}
}

