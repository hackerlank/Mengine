#	pragma once

#	include "Kernel/Node.h"
#	include "Kernel/Animatable.h"

#	include "Kernel/RenderCameraProjection.h"
#	include "Kernel/RenderViewport.h"

#   include "ResourceMovie2.h"

#   include "stdex/stl_map.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Movie2
		: public Node
		, public Animatable
	{
	public:
		Movie2();
		~Movie2();

    public:
		void setResourceMovie2( const ResourceMovie2Ptr & _resourceMovie );
		const ResourceMovie2Ptr & getResourceMovie2() const;

	public:
		void setCompositionName( const ConstString & _name );
		const ConstString & getCompositionName() const;

	public:
		float getDuration() const;
		void setWorkAreaFromEvent( const ConstString & _eventName );
		void removeWorkArea();

	protected:
		bool _play( float _time ) override;
		bool _restart( uint32_t _enumerator, float _time ) override;
		void _pause( uint32_t _enumerator ) override;
		void _resume( uint32_t _enumerator, float _time ) override;
		void _stop( uint32_t _enumerator ) override;
		void _end( uint32_t _enumerator ) override;
		bool _interrupt( uint32_t _enumerator ) override;

	protected:
		bool _compile() override;
		void _release() override;
		
	protected:
		bool _activate() override;
		void _deactivate() override;

		void _afterActivate() override;

	protected:
		void _setLoop( bool _value ) override;
		void _setTiming( float _timing ) override;
		float _getTiming() const override;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void _render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state ) override;

	protected:
		void _changeParent( Node * _oldParent, Node * _newParent ) override;
		void _addChild( Node * _node ) override;
		void _removeChild( Node * _node ) override;

	public:
		struct Camera
		{
			RenderCameraProjection * projection;
			RenderViewport * viewport;
		};

		Camera * addCamera( const ConstString & _name, RenderCameraProjection * _projection, RenderViewport * _viewport );
		bool hasCamera( const ConstString & _name ) const;

		bool getCamera( const ConstString & _name, Camera ** _camera );

	public:
		ResourceHolder<ResourceMovie2> m_resourceMovie2;

		ConstString m_compositionName;

		aeMovieComposition * m_composition;

		struct Mesh
		{
			TVectorRenderVertex2D vertices;
			TVectorRenderIndices indices;

			RenderMaterialInterfacePtr material;
		};

		typedef stdex::vector<Mesh> TVectorMesh;
		TVectorMesh m_meshes;

		typedef stdex::map<ConstString, Camera> TMapCamera;
		TMapCamera m_cameras;
	};
}