#include "CineCamera.h"
#include "Camera.h"
#include "MathUtil.h"
#include <yaml-cpp/yaml.h>
#include "GraphicsManager.h"
#include "Transform.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::CineCamera)

Truth::CineCamera::CineCamera()
	: m_isMove(false)
	, m_currentNode(0)
	, m_nextNode(1)
	, m_dt(0)
	, m_isEnd(true)
	, m_dataPath{}
	, m_alias("CineCamera")
{

}

Truth::CineCamera::~CineCamera()
{

}

void Truth::CineCamera::Update()
{
	// �� �� ī�޶� �̵��� ���� ���� ��
	if (!m_isMove && m_isEnd)
		return;

	// �� �� ī�޶� �̵� ���
	CameraNode& current = m_node[m_currentNode];
	CameraNode& next = m_node[m_nextNode];
	m_dt += GetDeltaTime();
	// ���ӽð��� ���� ���� ���� �̵��Ѵ�.
	while (current.m_delayTime <= m_dt)
	{
		m_dt -= current.m_delayTime;
		m_currentNode++;
		m_nextNode++;
		if (m_nextNode >= m_node.size())
		{
			m_isEnd = true;
			m_isMove = false;
			m_currentNode = 0;
			m_nextNode = 1;
			m_dt = 0.f;
			return;
		}
	}

	// ���� ī�޶� ��������
	auto camera = m_mainCamera.lock();
	float rate = m_dt / current.m_delayTime;

	// �̵� ó��
	switch (current.m_moveMode)
	{
	case CINE_CAMERA_MOVE_MODE::DIRECT:
	{
		camera->m_position = next.m_position;
		break;
	}
	case CINE_CAMERA_MOVE_MODE::LERP:
	{
		Vector3 pos = Vector3::Lerp(current.m_position, next.m_position, rate);
		camera->GetOwner().lock()->m_transform->m_position = pos;
		DEBUG_PRINT((std::to_string(pos.x) + ' ' + std::to_string(pos.y) + ' ' + std::to_string(pos.z) + '\n').c_str());
		break;
	}
	case CINE_CAMERA_MOVE_MODE::CURVE:
	{
		std::vector<Vector3> curvePoint;
		curvePoint.push_back(current.m_position);
		for (auto p : current.m_curvePositionPoint)
			curvePoint.push_back(p);
		curvePoint.push_back(next.m_position);
		camera->m_position = MathUtil::GetBezjePoint(curvePoint, rate);
		break;
	}
	default:
		break;
	}

	// ȸ�� ó��
	switch (current.m_rotaionMode)
	{
	case CINE_CAMERA_ROTATION_MODE::DIRECT:
	{
		camera->m_look = next.m_look;
		break;
	}
	case CINE_CAMERA_ROTATION_MODE::LERP:
	{
		/// TODO: make sLerp
		Vector3 look = Vector3::Lerp(current.m_look, next.m_look, rate);
		camera->m_look = look;
		break;
	}
	case CINE_CAMERA_ROTATION_MODE::CURVE:
	{
		std::vector<Vector3> curvePoint;
		curvePoint.push_back(current.m_look);
		for (auto p : current.m_curveRotationPoint)
			curvePoint.push_back(p);
		curvePoint.push_back(next.m_look);
		camera->m_look = MathUtil::GetBezjePoint(curvePoint, rate);
		break;
	}
	default:
		break;
	}
}

/// <summary>
/// �ʱ�ȭ, �����Ͱ� �ִٸ� �����´�.
/// </summary>
void Truth::CineCamera::Awake()
{
	if (fs::exists(m_dataPath))
		LoadData(m_dataPath);

	m_managers.lock()->Graphics()->AddCineCamera(m_alias, ::Cast<CineCamera>(shared_from_this()));
}

/// <summary>
/// �� �� ī�޶��� �̵� ����
/// </summary>
void Truth::CineCamera::Play()
{
	if (!m_mainCamera.expired() && m_node.size() >= 1 && m_isEnd)
	{
		m_isMove = true;
		m_isEnd = false;
		m_currentNode = 0;
		m_nextNode = 1;
	}
}

/// <summary>
/// ī�޶� �̵� ������ ��� ������ �ε�
/// </summary>
/// <param name="_dataPath">������ ���� ���</param>
void Truth::CineCamera::LoadData(const fs::path& _dataPath)
{
	std::ifstream fin(_dataPath);
	YAML::Node node = YAML::Load(fin);

	// ��Ʈ ��� �Ľ�
	const YAML::Node& rootNode = node["NodeData"];
	if (!rootNode.IsDefined() || !rootNode.IsSequence())
	{
		assert(false && "Cannot find YAML Node Data");
		return;
	}

	m_dataPath = _dataPath.generic_string();

	// ��Ʈ ������ ��忡 �ִ� ������ �����´�.
	// ��Ʈ ������ ���� �迭�� �����Ǿ��ִ�.
	for (YAML::const_iterator it = rootNode.begin(); it != rootNode.end(); ++it)
	{
		CameraNode cNode;

		// ���� ��ġ ����
		const YAML::Node& posNode = (*it)["position"];
		if (posNode.IsDefined())
			cNode.m_position = {
				posNode["x"].as<float>(),
				posNode["y"].as<float>(),
				posNode["z"].as<float>()
		};

		// ���� ȸ�� ����
		const YAML::Node& rotNode = (*it)["rotation"];
		if (rotNode.IsDefined())
			cNode.m_look = {
				rotNode["x"].as<float>(),
				rotNode["y"].as<float>(),
				rotNode["z"].as<float>()
		};

		// ������ ���
		if ((*it)["positionMode"].IsDefined())
			cNode.m_moveMode = static_cast<CINE_CAMERA_MOVE_MODE>((*it)["positionMode"].as<uint32>());
		// ȸ�� ���
		if ((*it)["rotationMode"].IsDefined())
			cNode.m_rotaionMode = static_cast<CINE_CAMERA_ROTATION_MODE>((*it)["rotationMode"].as<uint32>());
		// �����ӿ� �ɸ��� �ð�
		if ((*it)["delayTime"].IsDefined())
			cNode.m_delayTime = (*it)["delayTime"].as<float>();

		// Ŀ�� ��忡�� ����� ������
		const YAML::Node& posCurveData = (*it)["positionCurveData"];
		if (posCurveData.IsDefined() && posCurveData.IsSequence())
		{
			for (YAML::const_iterator it = posCurveData.begin(); it != posCurveData.end(); ++it)
			{
				Vector3 point
				{
					(*it)["x"].as<float>(),
					(*it)["y"].as<float>(),
					(*it)["z"].as<float>()
				};
				cNode.m_curvePositionPoint.push_back(point);
			}
		}

		// Ŀ�� ��忡�� ����� ������
		const YAML::Node& rotcurveData = (*it)["rotationCurveData"];
		if (rotcurveData.IsDefined() && rotcurveData.IsSequence())
		{
			for (YAML::const_iterator it = rotcurveData.begin(); it != rotcurveData.end(); ++it)
			{
				Vector3 point
				{
					(*it)["x"].as<float>(),
					(*it)["y"].as<float>(),
					(*it)["z"].as<float>()
				};
				cNode.m_curveRotationPoint.push_back(point);
			}
		}
		m_node.push_back(cNode);
	}
}
