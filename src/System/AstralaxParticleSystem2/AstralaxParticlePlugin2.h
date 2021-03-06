#	pragma once

#	include "Core/PluginBase.h"

namespace Menge
{
	class AstralaxParticlePlugin2
		: public PluginBase
	{
		PLUGIN_DECLARE("AstralaxParticle")

	public:
		AstralaxParticlePlugin2();

	protected:
		bool _initialize() override;
		void _finalize() override;

	protected:
		void _destroy() override;
	};
}