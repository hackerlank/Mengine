#	pragma once

#	include "Interface/SoundSystemInterface.h"
#	include "Interface/SoundCodecInterface.h"

#	include "SDL.h"

namespace Menge
{
    class SDLSoundBuffer
        : public SoundBufferInterface
    {
    public:
        SDLSoundBuffer();
        virtual ~SDLSoundBuffer();

    public:
        void setServiceProvider( ServiceProviderInterface * m_serviceProvider );

    public:
        virtual bool load( const SoundDecoderInterfacePtr & _soundDecoder );
        virtual void play( int _channel, bool _loop );

    public:
        virtual bool update() override;

    public:
        virtual const SoundDecoderInterfacePtr & getDecoder() const override;
        
    protected:
        ServiceProviderInterface * m_serviceProvider;
        SoundDecoderInterfacePtr m_soundDecoder;
    };

    typedef stdex::intrusive_ptr<SDLSoundBuffer> SDLSoundBufferPtr;
}   // namespace Menge
