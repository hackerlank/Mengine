#   pragma once

#	include "Interface/SoundSystemInterface.h"
#	include <AL/al.h>

namespace Menge
{
	class MarmaladeSoundSystem;
	class MarmaladeSoundBufferBase;

	class MarmaladeSoundSource
		: public SoundSourceInterface
	{
	public:
		MarmaladeSoundSource();
		~MarmaladeSoundSource();
        
    public:
        void initialize( ServiceProviderInterface * _serviceProvider, MarmaladeSoundSystem* _soundSystem );

	public:
		void play() override;
		void pause() override;
		void stop() override;
		bool isPlaying() const override;

		void setVolume( float _volume ) override;
		float getVolume() const override;

		void setPosition( float _x, float _y, float _z ) override;
		const float * getPosition() const override;

		void setLoop( bool _loop ) override;
		bool getLoop() const override;

		float getLengthMs() const override;
		float getPosMs() const override;
		void setPosMs( float _posMs ) override;

		void loadBuffer( SoundBufferInterface* _soundBuffer ) override;
		SoundBufferInterface* getSoundBuffer() const override;

	public:
		void setHeadMode( bool _headMode );
		bool getHeadMode() const;

    protected:
        void unloadBuffer_();

    private:
        ServiceProviderInterface * m_serviceProvider;
        
        MarmaladeSoundSystem * m_soundSystem;

		float m_position[3];
		float m_volume;
		void apply_( ALuint _source );

		ALuint m_sourceId;
		float m_timing;

		MarmaladeSoundBufferBase* m_soundBuffer;

        bool m_headMode;
        bool m_playing;
        bool m_loop;
	};
}	// namespace Menge