#pragma once
#include "Systems/System.h"

namespace drft::system
{
	class RealityBubble : public System
	{
	public:
		using System::System;

		void onUpdateBegin() override;
		void onUpdateEnd() override;
	};
}



