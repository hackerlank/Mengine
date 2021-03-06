#	pragma once

#	include "Core/ModuleBase.h"

#	include "MagnetWorld.h"
#	include "MagnetActor.h"
#	include "NodeMagnetActor.h"

namespace Menge
{
	class ModuleMagnet
		: public ModuleBase
	{
	public:
		ModuleMagnet();
		~ModuleMagnet();

	public:
		bool _initialize() override;
		void _finalize() override;

	public:
		MagnetWorldPtr createMagnetWorld();
		void removeMagnetWorld( const MagnetWorldPtr & _collision );

	public:
		void _tick( float _time, float _timing ) override;
		void _render( const RenderObjectState * _state, unsigned int _debugMask ) override;
		
	protected:
		typedef FactoryPoolStore<MagnetWorld, 4> TFactoryMagnetWorld;
		TFactoryMagnetWorld m_factoryMagnetWorld;

		typedef stdex::vector<MagnetWorldPtr> TVectorMagnetWorlds;
		TVectorMagnetWorlds m_magnetWorlds;
	};
}