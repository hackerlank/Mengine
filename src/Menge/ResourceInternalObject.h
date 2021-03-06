#	pragma once

#	include "Kernel/ResourceReference.h"

namespace Menge
{
	class ResourceInternalObject
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceInternalObject();
		~ResourceInternalObject();

	public:
		const ConstString & getInternalName() const;
		const ConstString & getInternalGroup() const;

	protected:
		bool _loader( const Metabuf::Metadata * _parser ) override;

	protected:
		bool _compile() override;
		void _release() override;
	
	protected:
		ConstString m_internalName;
		ConstString m_internalGroup;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceInternalObject> ResourceInternalObjectPtr;
}