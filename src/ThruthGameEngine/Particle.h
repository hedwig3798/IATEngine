#pragma once

#include "Headers.h"
#include "IParticleMesh.h"

namespace Truth
{
	class Texture;
}

namespace Truth
{
	class Particle
	{
	private:
		std::shared_ptr<Ideal::IParticleMaterial> m_t0;
		std::shared_ptr<Texture> m_t1;
		std::shared_ptr<Texture> m_t2;

	public:
	};
}

