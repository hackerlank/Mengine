#	pragma once

#   include "Utils/Factory/Factorable.h"

#   include <stddef.h>

namespace Menge
{
	class FactorablePtr
        : public Factorable
	{
	public:
		FactorablePtr();
		~FactorablePtr();

    public:
        size_t getReference() const;

    protected:
        friend inline void intrusive_ptr_add_ref( FactorablePtr * _ptr )
        {
            ++_ptr->m_reference;
        }

        friend inline void intrusive_ptr_release( FactorablePtr * _ptr )
        {
            if( --_ptr->m_reference == 0 )
            {
                _ptr->destroy();
            }
        }

    protected:
        size_t m_reference;
	};
}