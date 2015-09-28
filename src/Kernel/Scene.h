#	pragma once

#	include "Kernel/Entity.h"
//#	include "Reference.h"

#	include "Kernel/Scriptable.h"

#	include "Kernel/Layer.h"

#	include "math/vec3.h"
#	include "math/vec4.h"

namespace Menge
{
	class Scene
		: public Entity
	{
	public:
		Scene();
		~Scene();

	public:
		void setMainLayer( Layer * _layer );
		Layer * getMainLayer() const;
				
	public:
		Scene * getParentScene() const;
		bool isSubScene() const;

	public:
		Scene * getScene() override;

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

    public:
		void onFocus( bool _focus );

	protected:
		void _addChild( Node * _layer ) override;
        void _removeChild( Node * _node ) override;

		void _changeParent( Node * _oldParent, Node * _newParent ) override;        

	protected:
		void _setEventListener( const pybind::dict & _embed ) override;

	protected:
		Scene * m_parentScene;
		Layer * m_mainLayer;

		mt::vec2f m_offsetPosition;	
	};
}
