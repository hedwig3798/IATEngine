#pragma once
#include "Core/Core.h"

namespace Ideal
{
	class IdealLayer
	{
	public:
		IdealLayer();
		~IdealLayer();

		void AddLayer(uint32 LayerNum);
		void DeleteLayer(uint32 LayerNum);

		// �ƿ� Layer��ü�� �ٲ۴�.
		void ChangeLayer(uint32 LayerNum);
		void ChangeBitMask(uint32 LayerMask);

		uint32 GetLayer() { return m_Layer; }
	private:
		uint32 m_Layer;
	};
}
