#   pragma once

#   include "Interface/RenderSystemInterface.h"

#	include <d3d9.h>

namespace Menge
{
	class DX9RenderVertexShader
		: public RenderVertexShaderInterface
	{
	public:
		DX9RenderVertexShader();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	public:
		const ConstString & getName() const override;

	public:
		bool initialize( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _isCompile );

	public:
		bool compile( IDirect3DDevice9 * _pD3DDevice );

	public:
		bool enable( IDirect3DDevice9 * _pD3DDevice );

	protected:
		ServiceProviderInterface * m_serviceProvider;

		ConstString m_name;

		MemoryInterfacePtr m_memory;

		IDirect3DVertexShader9 * m_shader;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<DX9RenderVertexShader> DX9RenderVertexShaderPtr;
}