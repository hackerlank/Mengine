#	pragma once

#   include "Interface/VideoCodecInterface.h"

#	include "Kernel/ResourceReference.h"
#	include "Kernel/ResourceCacher.h"

#	include "Math/vec2.h"

namespace Menge
{
	class ResourceVideo
		: public ResourceReference
	{
		DECLARE_VISITABLE( ResourceReference );

	public:
		ResourceVideo();
		~ResourceVideo();

	public:
		bool _loader( const Metabuf::Metadata * _parser ) override;
        bool _convert() override;

    public:
		void setFrameRate( float _frameRate );
		float getFrameRate() const;

	public:
		void setDuration( float _duration );
		float getDuration() const;

	public:
		void setFilePath( const FilePath & _path );
		const FilePath & getFilePath() const;

		void setCodecType( const ConstString & _type );
		const ConstString & getCodecType() const;

    public:
        VideoDecoderInterfacePtr createVideoDecoder() const;
		void destroyVideoDecoder( const VideoDecoderInterfacePtr & _decoder ) const;
		
	protected:
		void _cache() override;
		void _uncache() override;

	public:
		void setAlpha( bool _alpha );
		bool isAlpha() const;

	public:
		const mt::vec2f& getFrameSize() const;
        bool isNoSkeep() const;

    protected:
        bool _isValid() const override;

	protected:
		bool checkValidVideoDecoder_( const VideoDecoderInterfacePtr & _decoder ) const;

	protected:
		bool _compile() override;
		void _release() override;
			
	protected:
		FilePath m_filePath;
        
        ConstString m_converterType;
		ConstString m_codecType;

        float m_frameRate;
		float m_duration;

		typedef ResourceCacher<VideoDecoderInterfacePtr> TCacherVideoDecoder;
		mutable TCacherVideoDecoder m_videoDecoderCacher;

        bool m_alpha;
        bool m_noSeek;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ResourceVideo> ResourceVideoPtr;
}
