#include "ParticleManager.h"
#include "IParticleSystem.h"
#include "GraphicsManager.h"
#include "IParticleMaterial.h"
#include "TimeManager.h"
#include <yaml-cpp/yaml.h>

Truth::ParticleManager::ParticleManager()
{

}

Truth::ParticleManager::~ParticleManager()
{

}

void Truth::ParticleManager::Initalize(std::shared_ptr<GraphicsManager> _grapics, std::shared_ptr<TimeManager> _timeManager)
{
	m_grapics = _grapics;
	m_timeManager = _timeManager;
	ReloadAllParticle();
}

void Truth::ParticleManager::Update()
{
	for (auto& pool : m_particleMap)
	{
		for (auto& e : pool.second->m_pool)
		{
			e->SetDeltaTime(m_timeManager->GetDT());
		}
	}

}

void Truth::ParticleManager::Finalize()
{
	Reset();
}

void Truth::ParticleManager::CreateEmptyParticle()
{
	YAML::Node node;
	YAML::Emitter emitter;
	emitter << YAML::BeginMap;

	emitter << YAML::Key << "MaxCount" << YAML::Value << 10;
	emitter << YAML::Key << "MatName" << YAML::Value << "defaultMat";

	emitter << YAML::Key << "EBlendingMode" << YAML::Value << 1;
	emitter << YAML::Key << "Shader" << YAML::Value << "SwordSlash";
	emitter << YAML::Key << "Texture0" << YAML::Value << "../Resources/Textures/0_Particle/Smoke12.png";
	emitter << YAML::Key << "Texture1" << YAML::Value << "../Resources/Textures/0_Particle/Crater62.png";
	emitter << YAML::Key << "Texture2" << YAML::Value << "../Resources/Textures/0_Particle/Noise43b.png";
	emitter << YAML::Key << "Mesh" << YAML::Value << "0_Particle/Slash";

	emitter << YAML::Key << "StartColor" << YAML::BeginSeq <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::Value << 1.0f <<
		YAML::EndSeq;

	emitter << YAML::Key << "StartLifetime" << YAML::Value << 1.0f;


	emitter << YAML::Key << "Loop" << YAML::Value << false;
	emitter << YAML::Key << "Duration" << YAML::Value << 2.0f;
	emitter << YAML::Key << "StopEnd" << YAML::Value << true;
	emitter << YAML::Key << "RenderMode" << YAML::Value << 1;
	emitter << YAML::Key << "RotationOverLifetime" << YAML::Value << true;

	emitter << YAML::Key << "RotationOverLifetimeAxisYControlPoints" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.5f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData1X" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.1f << YAML::Key << "y" << YAML::Value << 2.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.3f << YAML::Key << "y" << YAML::Value << 0.6f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData1Y" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 1.0f << YAML::Key << "y" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData2Z" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "CustomData2W" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "x" << YAML::Value << 0.0f << YAML::Key << "y" << YAML::Value << 0.2f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::Key << "ColorOverLifetime" << YAML::Value << true;


	emitter << YAML::Key << "ColorOverLifetimeGradientGraph" << YAML::BeginSeq <<
		YAML::BeginMap <<
		YAML::Key << "Color" << YAML::BeginSeq <<
		YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f <<
		YAML::EndSeq <<
		YAML::Key << "position" << YAML::Value << 0.0f <<
		YAML::EndMap <<
		YAML::BeginMap <<
		YAML::Key << "Color" << YAML::BeginSeq <<
		YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 1.0f << YAML::Value << 0.0f <<
		YAML::EndSeq <<
		YAML::Key << "position" << YAML::Value << 1.0f <<
		YAML::EndMap <<
		YAML::EndSeq;

	emitter << YAML::EndMap;

	fs::path p = "..\\Resources\\Particles";

	p /= "EmptyParticle.yaml";

	std::ofstream fout(p);
	fout << emitter.c_str();
}

void Truth::ParticleManager::LoadParticle(fs::path _path)
{
	if (m_particleMap.find(_path) != m_particleMap.end())
	{
		return;
	}
	USES_CONVERSION;

	std::ifstream fin(_path);
	auto node = YAML::Load(fin);


	int maxCount = node["MaxCount"].as<int>();
	m_particleMap[_path] = std::make_shared<ParticePool>(m_grapics);
	// m_particleMap[_path]->m_pool.resize(maxCount);
	m_particleMap[_path]->m_maxCount = maxCount;

	for (int i = 0; i < maxCount; i++)
	{
		std::shared_ptr<Ideal::IParticleMaterial> mat = m_grapics->CreateParticleMaterial();

		std::wstring sahder = A2W(node["Shader"].as<std::string>().c_str());
		mat->SetShader(m_grapics->CreateShader(sahder));

		std::shared_ptr<Ideal::IParticleSystem> particle;

		std::string matName = node["MatName"].as<std::string>();
		if (m_particleMatMap.find(matName) != m_particleMatMap.end())
		{
			particle = m_grapics->CreateParticle(m_particleMatMap[matName]);
			particle->SetActive(false);
			particle->SetTransformMatrix(Matrix::Identity);
		}
		else
		{
			/// Create Texture 0 1 2
			if (node["Texture0"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture0"].as<std::string>().c_str());
				std::shared_ptr<Texture> t0 = m_grapics->CreateTexture(tPath);
				mat->SetTexture0(t0->m_texture);
			}
			if (node["Texture1"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture1"].as<std::string>().c_str());
				std::shared_ptr<Texture> t1 = m_grapics->CreateTexture(tPath);
				mat->SetTexture1(t1->m_texture);
			}

			if (node["Texture2"].IsDefined())
			{
				std::wstring tPath = A2W(node["Texture2"].as<std::string>().c_str());
				std::shared_ptr<Texture> t2 = m_grapics->CreateTexture(tPath);
				mat->SetTexture2(t2->m_texture);
			}
			/// Blending Mode
			if (node["EBlendingMode"].IsDefined())
			{
				mat->SetBlendingMode(static_cast<Ideal::ParticleMaterialMenu::EBlendingMode>(node["EBlendingMode"].as<int>()));
			}
			/// Back Face Culling
			if (node["BackFaceCulling"].IsDefined())
			{
				mat->SetBackFaceCulling(node["BackFaceCulling"].as<bool>());
			}
			/// Write DepthBuffer
			if (node["WriteDepthBuffer"].IsDefined())
			{
				mat->SetWriteDepthBuffer(node["WriteDepthBuffer"].as<bool>());
			}
			/// Transparency
			if (node["Transparency"].IsDefined())
			{
				mat->SetTransparency(node["Transparency"].as<bool>());
			}
			m_particleMatMap[matName] = mat;
			particle = m_grapics->CreateParticle(mat);
			particle->SetActive(false);
			particle->SetTransformMatrix(Matrix::Identity);
		}
		/// mesh
		if (node["Mesh"].IsDefined())
		{
			particle->SetRenderMesh(m_grapics->CreateParticleMesh(A2W(node["Mesh"].as<std::string>().c_str())));
		}
		/// Stop When Finished
		if (node["StopWhenFinished"].IsDefined())
		{
			particle->SetStopWhenFinished(node["StopWhenFinished"].as<bool>());
		}
		/// Play OnWake
		if (node["PlayOnWake"].IsDefined())
		{
			particle->SetPlayOnWake(node["PlayOnWake"].as<bool>());
		}
		/// Max Particle
		if (node["MaxParticles"].IsDefined())
		{
			particle->SetMaxParticles((node["MaxParticles"].as<uint32>()));
		}


		/// Start Color
		if (node["StartColor"].IsDefined())
		{
			const YAML::Node& cNode = node["StartColor"];
			Color c(cNode[0].as<float>(), cNode[1].as<float>(), cNode[2].as<float>(), cNode[3].as<float>());
			particle->SetStartColor(c);
		}
		/// Start Size
		if (node["StartSize"].IsDefined())
		{
			const YAML::Node& cNode = node["StartSize"];
			particle->SetStartSize(cNode.as<float>());
		}
		/// Start Life Time
		if (node["StartLifetime"].IsDefined())
		{
			particle->SetStartLifetime(node["StartLifetime"].as<float>());
		}
		/// Simulation Speed
		if (node["SimulationSpeed"].IsDefined())
		{
			particle->SetSimulationSpeed(node["SimulationSpeed"].as<float>());
		}
		/// Duration
		if (node["Duration"].IsDefined())
		{
			particle->SetDuration(node["Duration"].as<float>());
		}
		/// Loop
		if (node["Loop"].IsDefined())
		{
			particle->SetLoop(node["Loop"].as<bool>());
		}
		/// Rate Over Time
		if (node["RateOverTime"].IsDefined())
		{
			particle->SetRateOverTime(node["RateOverTime"].as<bool>());
		}
		/// Emission Rate Over Time
		if (node["EmissionRateOverTime"].IsDefined())
		{
			particle->SetEmissionRateOverTime(node["EmissionRateOverTime"].as<float>());
		}
		/// ShapeMode
		if (node["ShapeMode"].IsDefined())
		{
			particle->SetShapeMode((node["ShapeMode"].as<bool>()));
		}
		/// Shape
		if (node["Shape"].IsDefined())
		{
			particle->SetShape(static_cast<Ideal::ParticleMenu::EShape>(node["Shape"].as<int>()));
		}
		/// Radius
		if (node["Radius"].IsDefined())
		{
			particle->SetRadius(node["Radius"].as<float>());
		}
		/// Radius Thickness
		if (node["RadiusThickness"].IsDefined())
		{
			particle->SetRadiusThickness(node["RadiusThickness"].as<float>());
		}
		/// Velocity Over Lifetime
		if (node["VelocityOverLifetime"].IsDefined())
		{
			particle->SetVelocityOverLifetime(node["VelocityOverLifetime"].as<bool>());
		}
		/// Velocity Direction Mode
		if (node["VelocityDirectionMode"].IsDefined())
		{
			particle->SetVelocityDirectionMode(static_cast<Ideal::ParticleMenu::EMode>(node["VelocityDirectionMode"].as<int>()));
		}
		/// Velocity Direction Random
		if (node["VelocityDirectionRandomMIN"].IsDefined() && node["VelocityDirectionRandomMAX"].IsDefined())
		{
			particle->SetVelocityDirectionRandom(
				node["VelocityDirectionRandomMIN"].as<float>(),
				node["VelocityDirectionRandomMAX"].as<float>()
			);
		}
		/// Velocity Direction Const
		if (node["VelocityDirectionConst"].IsDefined())
		{
			YAML::Node child = node["VelocityDirectionConst"];
			Vector3 val = {child["x"].as<float>(), child["y"].as<float>() , child["z"].as<float>() };
			particle->SetVelocityDirectionConst(val);
		}
		/// Velocity Speed Modifier Mode
		if (node["VelocitySpeedModifierMode"].IsDefined())
		{
			particle->SetVelocitySpeedModifierMode(static_cast<Ideal::ParticleMenu::EMode>(node["VelocitySpeedModifierMode"].as<int>()));
		}
		/// VelocitySpeedModifierRandom
		if (node["VelocitySpeedModifierRandomMIN"].IsDefined() && node["VelocitySpeedModifierRandomMAX"].IsDefined())
		{
			particle->SetVelocitySpeedModifierRandom(
				node["VelocitySpeedModifierRandomMIN"].as<float>(),
				node["VelocitySpeedModifierRandomMAX"].as<float>()
			);
		}
		/// Velocity Speed Modifier Const
		if (node["VelocitySpeedModifierConst"].IsDefined())
		{
			particle->SetVelocitySpeedModifierConst(node["VelocitySpeedModifierConst"].as<float>());
		}
		/// Color Over Lifetime
		if (node["ColorOverLifetime"].IsDefined())
		{
			particle->SetColorOverLifetime(node["ColorOverLifetime"].as<bool>());
		}

		/// Color Over Lifetime Gradient Graph
		if (node["ColorOverLifetimeGradientGraph"].IsDefined())
		{
			const YAML::Node& child = node["ColorOverLifetimeGradientGraph"];
			auto& graph = particle->GetColorOverLifetimeGradientGraph();

			for (YAML::const_iterator it = child.begin(); it != child.end(); ++it)
			{
				const YAML::Node& point = (*it)["Color"];
				Color startColor(point[0].as<float>(), point[1].as<float>(), point[2].as<float>(), point[3].as<float>());
				float position = (*it)["position"].as<float>();
				graph.AddPoint(startColor, position);
			}
		}
		/// Rotation Over Life Time
		if (node["RotationOverLifetime"].IsDefined())
		{
			particle->SetRotationOverLifetime(node["RotationOverLifetime"].as<bool>());
		}

		/// Rotation Over Life Time Axis X ControlPoints
		if (node["RotationOverLifetimeAxisXControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisXControlPoints"];
			GetControlPoints(&child, particle, particle->GetRotationOverLifetimeAxisX());
		}
		/// Rotation Over Life Time Axis Y ControlPoints
		if (node["RotationOverLifetimeAxisYControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisYControlPoints"];
			GetControlPoints(&child, particle, particle->GetRotationOverLifetimeAxisY());
		}
		/// Rotation Over Lifetime Axis Z ControlPoints
		if (node["RotationOverLifetimeAxisZControlPoints"].IsDefined())
		{
			const YAML::Node& child = node["RotationOverLifetimeAxisZControlPoints"];
			GetControlPoints(&child, particle, particle->GetRotationOverLifetimeAxisZ());
		}
		/// Size Over Lifetime
		if (node["SizeOverLifetime"].IsDefined())
		{
			particle->SetSizeOverLifetime(node["SizeOverLifetime"].as<bool>());
		}
		/// Size Over Life Time Axis X ControlPoints
		if (node["SizeOverLifetimeAxisX"].IsDefined())
		{
			const YAML::Node& child = node["SizeOverLifetimeAxisX"];
			GetControlPoints(&child, particle, particle->GetSizeOverLifetimeAxisX());
		}
		/// Size Over Life Time Axis Y ControlPoints
		if (node["SizeOverLifetimeAxisY"].IsDefined())
		{
			const YAML::Node& child = node["SizeOverLifetimeAxisY"];
			GetControlPoints(&child, particle, particle->GetSizeOverLifetimeAxisY());
		}
		/// Size Over Lifetime Axis Z ControlPoints
		if (node["SizeOverLifetimeAxisZ"].IsDefined())
		{
			const YAML::Node& child = node["SizeOverLifetimeAxisZ"];
			GetControlPoints(&child, particle, particle->GetSizeOverLifetimeAxisZ());
		}
		/// Custom Data 1 X
		if (node["CustomData1X"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1X"];
			GetControlPoints(&child, particle, particle->GetCustomData1X());
		}
		/// Custom Data 1 Y
		if (node["CustomData1Y"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1Y"];
			GetControlPoints(&child, particle, particle->GetCustomData1Y());
		}
		/// Custom Data 1 Z
		if (node["CustomData1Z"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1Z"];
			GetControlPoints(&child, particle, particle->GetCustomData1Z());
		}
		/// Custom Data 1 W
		if (node["CustomData1W"].IsDefined())
		{
			const YAML::Node& child = node["CustomData1W"];
			GetControlPoints(&child, particle, particle->GetCustomData1W());
		}

		/// Custom Data 2 X
		if (node["CustomData2X"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2X"];
			GetControlPoints(&child, particle, particle->GetCustomData2X());
		}
		/// Custom Data 2 Y
		if (node["CustomData2Y"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2Y"];
			GetControlPoints(&child, particle, particle->GetCustomData2Y());
		}
		/// Custom Data 2 Z
		if (node["CustomData2Z"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2Z"];
			GetControlPoints(&child, particle, particle->GetCustomData2Z());
		}
		/// Custom Data 2 W
		if (node["CustomData2W"].IsDefined())
		{
			const YAML::Node& child = node["CustomData2W"];
			GetControlPoints(&child, particle, particle->GetCustomData2W());
		}
		/// Texture Sheet Animation
		if (node["TextureSheetAnimation"].IsDefined())
		{
			particle->SetTextureSheetAnimation(node["TextureSheetAnimation"].as<bool>());
		}
		/// Texture Sheet Animation Tiles
		if (node["TextureSheetAnimationTiles"].IsDefined())
		{
			YAML::Node child = node["TextureSheetAnimationTiles"];
			Vector2 val = { child["x"].as<float>(), child["y"].as<float>()};
			particle->SetTextureSheetAnimationTiles(val);
		}
		/// Render Mode
		if (node["RenderMode"].IsDefined())
		{
			particle->SetRenderMode(static_cast<Ideal::ParticleMenu::ERendererMode>(node["RenderMode"].as<int>()));
		}



		/// Create Particle 
		m_particleMap[_path]->m_pool.push_back(particle);
	}
}

void Truth::ParticleManager::ReloadParticle(fs::path _path)
{

}

void Truth::ParticleManager::ReloadAllParticle()
{
	fs::path dir = "..\\Resources\\Particles";

	fs::directory_iterator itr(dir);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			ReloadAllParticle(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			LoadParticle(childPath);
		}
		itr++;
	}
}

void Truth::ParticleManager::ReloadAllParticle(fs::path _path)
{
	fs::directory_iterator itr(_path);
	while (itr != fs::end(itr))
	{
		const fs::directory_entry& entry = *itr;

		fs::path childPath = entry.path();

		if (fs::is_directory(childPath))
		{
			ReloadAllParticle(childPath);
		}
		else if (fs::is_regular_file(childPath))
		{
			LoadParticle(childPath);
		}
		itr++;
	}
}

std::shared_ptr<Ideal::IParticleSystem> Truth::ParticleManager::GetParticle(fs::path _path)
{
	return m_particleMap[_path]->GetParticle();
}

void Truth::ParticleManager::Reset()
{
	for (auto& pool : m_particleMap)
	{
		pool.second->Reset();
	}
	m_particleMap.clear();

	for (auto& mat : m_particleMatMap)
	{
		m_grapics->DeleteParticleMaterial(mat.second);
	}
	m_particleMatMap.clear();
}

void Truth::ParticleManager::StopAllParticle()
{
	for (auto& pool : m_particleMap)
	{
		pool.second->StopAllParticle();
	}
}

void Truth::ParticleManager::GetControlPoints(const YAML::Node* _node, std::shared_ptr<Ideal::IParticleSystem> _particle, Ideal::IBezierCurve& _graph)
{
	for (YAML::const_iterator it = _node->begin(); it != _node->end(); ++it)
	{
		_graph.AddControlPoint(Ideal::Point((*it)["x"].as<float>(), (*it)["y"].as<float>()));
	}
}

Truth::ParticePool::ParticePool(std::shared_ptr<GraphicsManager> _graphic)
	: m_grapics(_graphic)
	, m_ind(0)
	, m_maxCount(10)
{

}

Truth::ParticePool::~ParticePool()
{
	Reset();
}

std::shared_ptr<Ideal::IParticleSystem> Truth::ParticePool::GetParticle()
{
	if (m_ind >= m_maxCount)
	{
		m_ind = 0;
	}
	return m_pool[m_ind++];
}

void Truth::ParticePool::Reset()
{
	for (auto& p : m_pool)
	{
		m_grapics->DeleteParticle(p);
	}
}

void Truth::ParticePool::StopAllParticle()
{
	for (auto& p : m_pool)
	{
		p->SetActive(false);
	}
}
