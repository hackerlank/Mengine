#	pragma once

#	include "Kernel/Node.h"

#	include "Kernel/RenderCameraOrthogonal.h"

namespace Menge
{
	class RenderCameraOrthogonalTarget
		: public Node
	{
	public:
		RenderCameraOrthogonalTarget();
		~RenderCameraOrthogonalTarget();

	public:
		void setRenderCameraOrthogonal( RenderCameraOrthogonal * _camera );
		RenderCameraOrthogonal * getRenderCameraOrthogonal() const;

	public:
		void setSpeed( float _speed );
		float getSpeed() const;

	public:
		void setFixedHorizont( float _horizont );
		float getFixedHorizont() const;
		bool isFixedHorizont() const;

	public:
		void _update( float _current, float _timing ) override;

	protected:
		RenderCameraOrthogonal * m_camera;
		float m_speed;

		bool m_fixedHorizont;
		float m_horizont;
	};	
}
