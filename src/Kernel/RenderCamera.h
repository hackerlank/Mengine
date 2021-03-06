#	pragma once

#	include "Interface/RenderSystemInterface.h"
#	include "Interface/NotificationServiceInterface.h"

#	include "Kernel/Node.h"

#	include "Core/Viewport.h"

namespace Menge
{
	class RenderCamera
		: public Node
        , public RenderCameraInterface
	{
	public:
		RenderCamera();
		
	protected:
		bool _activate() override;
		void _deactivate() override;
	
	public:
		const mt::mat4f & getCameraViewMatrix() const override;
		const mt::mat4f & getCameraViewMatrixInv() const override;

		const mt::mat4f & getCameraProjectionMatrix() const override;
		const mt::mat4f & getCameraProjectionMatrixInv() const override;

	public:
		const mt::mat4f & getCameraViewProjectionMatrix() const override;

	protected:
		void _invalidateWorldMatrix() override;
		
	protected:
		void invalidateViewMatrix_();
		void invalidateProjectionMatrix_();

	protected:
		virtual void _updateViewMatrix() const = 0;
		virtual void _updateProjectionMatrix() const = 0;

	protected:
		void updateViewProjectionMatrix_() const;

	protected:
		void notifyChangeWindowResolution( bool _fullscreen, const Resolution & _resolution );

	protected:
		ObserverInterfacePtr m_observerChangeWindowResolution;

		mutable mt::mat4f m_viewMatrix;
		mutable mt::mat4f m_viewMatrixInv;

		mutable mt::mat4f m_projectionMatrix;
		mutable mt::mat4f m_projectionMatrixInv;

		mutable mt::mat4f m_viewProjectionMatrix;

		mutable bool m_invalidateProjectionMatrix;
		mutable bool m_invalidateViewMatrix;
		mutable bool m_invalidateViewProjectionMatrix;
	};
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraProjectionMatrix() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			m_invalidateProjectionMatrix = false;

			this->_updateProjectionMatrix();
		}

		return m_projectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraProjectionMatrixInv() const
	{
		if( m_invalidateProjectionMatrix == true )
		{
			m_invalidateProjectionMatrix = false;

			this->_updateProjectionMatrix();
		}

		return m_projectionMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraViewMatrix() const
	{
		if( m_invalidateViewMatrix == true )
		{
			m_invalidateViewMatrix = false;

			this->_updateViewMatrix();
		}

		return m_viewMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraViewMatrixInv() const
	{
		if( m_invalidateViewMatrix == true )
		{
			m_invalidateViewMatrix = false;

			this->_updateViewMatrix();
		}

		return m_viewMatrixInv;
	}
	//////////////////////////////////////////////////////////////////////////
	inline const mt::mat4f & RenderCamera::getCameraViewProjectionMatrix() const
	{
		if( m_invalidateViewProjectionMatrix == true )
		{
			m_invalidateViewProjectionMatrix = false;

			this->updateViewProjectionMatrix_();
		}

		return m_viewProjectionMatrix;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void RenderCamera::invalidateViewMatrix_()
	{
		m_invalidateViewMatrix = true;
		m_invalidateViewProjectionMatrix = true;
	}
	//////////////////////////////////////////////////////////////////////////
	inline void RenderCamera::invalidateProjectionMatrix_()
	{
		m_invalidateProjectionMatrix = true;
		m_invalidateViewProjectionMatrix = true;
	}
}
