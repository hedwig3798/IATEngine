#pragma once
#include "Component.h"
#include "NavGeom.h"

struct rcHeightfield;
struct rcCompactHeightfield;
struct rcContourSet;
struct rcPolyMesh;
struct rcConfig;
struct rcPolyMeshDetail;

class rcContext;
class dtNavMesh;
class dtNavMeshQuery;

class dtQueryFilter;

#define MAX_POLYS 1024

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP
};
enum SamplePolyFlags
{
	SAMPLE_POLYFLAGS_WALK = 0x01,		// Ability to walk (ground, grass, road)
	SAMPLE_POLYFLAGS_SWIM = 0x02,		// Ability to swim (water).
	SAMPLE_POLYFLAGS_DOOR = 0x04,		// Ability to move through doors.
	SAMPLE_POLYFLAGS_JUMP = 0x08,		// Ability to jump.
	SAMPLE_POLYFLAGS_DISABLED = 0x10,		// Disabled polygon
	SAMPLE_POLYFLAGS_ALL = 0xffff	// All abilities.
};

namespace Truth
{
	class NavMeshGenerater
	{
		GENERATE_CLASS_TYPE_INFO(NavMeshGenerater);

	private:
		rcHeightfield* m_solid;
		rcCompactHeightfield* m_chf;
		rcContourSet* m_cset;
		rcPolyMesh* m_pmesh;
		rcConfig* m_cfg;
		rcPolyMeshDetail* m_dmesh;
		rcContext* m_ctx;

		dtNavMesh* m_navMesh;
		dtNavMeshQuery* m_navQuery;

		unsigned char* m_triareas;

		NavGeom m_geom;

		dtQueryFilter* m_filter;


	public:
		Vector3 m_boundMin;
		Vector3 m_boundMax;
		std::vector<Vector3> m_verteces;
		std::vector<int> m_triangle;

		float m_cellSize;
		float m_cellHeight;
		float m_agentHeight;
		float m_agentRadius;
		float m_agentMaxClimb;
		float m_agentMaxSlope;
		float m_regionMinSize;
		float m_regionMergeSize;
		float m_edgeMaxLen;
		float m_edgeMaxError;
		float m_vertsPerPoly;
		float m_detailSampleDist;
		float m_detailSampleMaxError;
		int m_partitionType;

		bool m_filterLowHangingObstacles;
		bool m_filterLedgeSpans;
		bool m_filterWalkableLowHeightSpans;

		bool m_keepInterResults;


	public:
		NavMeshGenerater();
		~NavMeshGenerater();

		void Initalize(std::wstring _path);
		void Initalize(const std::vector<float>& _points, const std::vector<uint32>& _indices);

		Vector3 FindPath(Vector3 _start, Vector3 _end, Vector3 _size);

		void Destroy();

	private:
		void Initalize();
	};
}