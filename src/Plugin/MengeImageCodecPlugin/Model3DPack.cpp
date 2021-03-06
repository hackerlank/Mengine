#	include "Model3DPack.h"

namespace Menge
{
	///////////////////////////////////////////////////////////////////////
	Model3DPack::Model3DPack()
		: m_frameCount(0)
		, m_vertexCount(0)
		, m_indicesCount(0)
		, m_frameDelay(0.f)
		, m_cameraFOV(0.f)
		, m_cameraAspect(0.f)
		, m_cameraRightSign(1.f)
	{		
	}
    //////////////////////////////////////////////////////////////////////////
    void Model3DPack::initialize( uint32_t _frameCount, uint32_t _vertexCount, uint32_t _indicesCount, float _frameDelay )
    {
		m_frameCount = _frameCount;
		m_vertexCount = _vertexCount;
		m_indicesCount = _indicesCount;

		m_frameDelay = _frameDelay;

        m_model3DFrames.resize( _frameCount );
    }
	//////////////////////////////////////////////////////////////////////////
	void Model3DPack::setCamera( float _fov, float _aspect, float _rightSign )
	{
		m_cameraFOV = _fov;
		m_cameraAspect = _aspect;
		m_cameraRightSign = _rightSign;
	}
	//////////////////////////////////////////////////////////////////////////
	Model3DFrame & Model3DPack::mutableFrame( uint32_t _frameId )
	{
		 Model3DFrame & frame = m_model3DFrames[_frameId];

		 return frame;
	}
	//////////////////////////////////////////////////////////////////////////	
	uint32_t Model3DPack::getFrameCount() const
	{
		return m_frameCount;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Model3DPack::getVertexCount() const
	{
		return m_vertexCount;
	}
	//////////////////////////////////////////////////////////////////////////
	uint32_t Model3DPack::getIndicesCount() const
	{
		return m_indicesCount;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model3DPack::getCameraFOV() const
	{
		return m_cameraFOV;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model3DPack::getCameraAspect() const
	{
		return m_cameraAspect;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model3DPack::getCameraRightSign() const
	{
		return m_cameraRightSign;
	}
	//////////////////////////////////////////////////////////////////////////
	float Model3DPack::getFrameDelay() const
	{
		return m_frameDelay;
	}
	//////////////////////////////////////////////////////////////////////////
	const Model3DFrame * Model3DPack::getFrame( uint32_t _frameId ) const
	{
		const Model3DFrame & frame = m_model3DFrames[_frameId];

		return &frame;
	}
	//////////////////////////////////////////////////////////////////////////
	Pointer Model3DPack::allocateMemory( size_t _size ) const
	{
		return stdex_malloc( _size );
	}
}