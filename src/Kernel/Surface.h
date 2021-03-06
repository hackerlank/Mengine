#	pragma once

#	include "Interface/COMInterface.h"

#	include "Factory/FactorablePtr.h"

#	include "Kernel/Identity.h"
#	include "Kernel/Servant.h"
#   include "Kernel/Visitable.h"
#	include "Kernel/Scriptable.h"
#   include "Kernel/Materialable.h"
#	include "Kernel/Resource.h"

#   include "Math\vec2.h"
#   include "Math\uv4.h"
#   include "Core\ColourValue.h"

namespace Menge
{
	class Surface
		: public FactorablePtr
        , public Servant
        , public Identity
        , public Visitable
        , public Scriptable
        , public Materialable
        , public Resource
	{
        DECLARE_VISITABLE_BASE();

	public:
		Surface();
		~Surface();

    public:
        virtual const mt::vec2f & getMaxSize() const = 0;
        virtual const mt::vec2f & getSize() const = 0;
        virtual const mt::vec2f & getOffset() const = 0;

        virtual uint32_t getUVCount() const = 0;
        virtual const mt::uv4f & getUV( uint32_t _index ) const = 0;

        virtual const ColourValue & getColour() const = 0;
        
    public:
        virtual bool update( float _current, float _timing ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<Surface> SurfacePtr;
}