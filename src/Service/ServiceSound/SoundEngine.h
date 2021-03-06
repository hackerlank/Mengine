#	pragma once

#   include "Interface/StreamInterface.h"
#	include "Interface/SoundSystemInterface.h"

#	include "MixerVolume.h"

#   include "Kernel/ThreadJob.h"
#   include "ThreadWorkerSoundBufferUpdate.h"

#	include "Core/ConstString.h"
#   include "Factory/FactoryStore.h"

#	include	"math/vec3.h"

#   include "stdex/stl_map.h"
#	include <stdex/stl_vector.h>

namespace Menge
{
	enum ESoundSourceState
	{
		ESS_STOP = 0,	    // currently stopped
		ESS_PLAY = 2,		// currently playing
		ESS_PAUSE = 4,			// currently paused
	};

	struct SoundSourceDesc
	{
		uint32_t soundId;

		SoundSourceInterfacePtr source;
		SoundListenerInterface * listener;

		ThreadWorkerSoundBufferUpdatePtr worker;
        uint32_t bufferId;

		float timing;
		MixerVolume volume;

		ESoundSourceState state;
		ESoundSourceType type;

        bool streamable;
		bool looped;
		bool turn;
	};

	class SoundEngine
		: public ServiceBase<SoundServiceInterface>
	{
	public:
		SoundEngine();
		~SoundEngine();

    public:
		bool _initialize() override;
		void _finalize() override;

	public:
		bool supportStreamSound() const override;

	public:
		void tick( float _time, float _timing ) override;

	public:
		void addSoundVolumeProvider( SoundVolumeProviderInterface * _soundVolumeProvider ) override;
		bool removeSoundVolumeProvider( SoundVolumeProviderInterface * _soundVolumeProvider ) override;

        
    public:
		void onTurnStream( bool _turn ) override;
        void onTurnSound( bool _turn ) override;

    public:
		uint32_t createSoundSource( bool _isHeadMode, const SoundBufferInterfacePtr & _sample, ESoundSourceType _type, bool _streamable ) override;

    public:
		SoundBufferInterfacePtr createSoundBufferFromFile( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecType, bool _streamable ) override;  

	protected:
		SoundDecoderInterfacePtr createSoundDecoder_( const ConstString& _pakName, const FilePath & _fileName, const ConstString & _codecType, bool _streamable );

    public:
		void setSoundVolume( const ConstString & _type, float _volume, float _default ) override;
		float getSoundVolume( const ConstString & _type ) const override;
		float mixSoundVolume() const override;

		void setCommonVolume( const ConstString & _type, float _volume, float _default ) override;
		float getCommonVolume( const ConstString & _type ) const override;
		float mixCommonVolume() const override;

		void setMusicVolume( const ConstString & _type, float _volume, float _default ) override;
		float getMusicVolume( const ConstString & _type ) const override;
		float mixMusicVolume() const override;

		void setVoiceVolume( const ConstString & _type, float _volume, float _default ) override;
        float getVoiceVolume( const ConstString & _type ) const override;
		float mixVoiceVolume() const override;

	public:
		bool setSourceVolume( uint32_t _emitterId, float _volume, float _default ) override;
		float getSourceVolume( uint32_t _emitterId ) const override;

	public:
		bool setSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer, float _volume, float _default ) override;
		float getSourceMixerVolume( uint32_t _emitter, const ConstString & _mixer ) const override;

	public:
		bool releaseSoundSource( uint32_t _sourceID ) override;

		bool validSoundSource( uint32_t _sourceID ) const override;
		
	public:
		bool play( uint32_t _emitterId ) override;
		bool pause( uint32_t _emitterId ) override;
		bool resume( uint32_t _emitterId ) override;
		bool stop( uint32_t _emitterId ) override;

	public:
		bool setLoop( uint32_t _emitterId, bool _looped ) override;
		bool getLoop( uint32_t _emitterId ) const override;

	public:
		void setSourceListener( uint32_t _emitterId, SoundListenerInterface* _listener ) override;
		
	public:
		float getDuration( uint32_t _emitterId ) const override;
		
		bool setPosMs( uint32_t _emitterId, float _pos ) override;
		float getPosMs( uint32_t _emitterId ) override;

		void mute( bool _mute ) override;
		bool isMute() const override;

	public:
		void updateVolume() override;

    protected:
		void updateSourceVolume_( SoundSourceDesc * _source );

    protected:
        bool getSoundSourceDesc_( uint32_t _emitterId, SoundSourceDesc ** _desc );
        bool getSoundSourceDesc_( uint32_t _emitterId, const SoundSourceDesc ** _desc ) const;

    protected:
        void playSounds_();
        void pauseSounds_();
        void stopSounds_();      
        
    protected:
        bool stopSoundBufferUpdate_( SoundSourceDesc * _source );
        bool playSoundBufferUpdate_( SoundSourceDesc * _source );

	protected:
        MixerVolume m_commonVolume;
		MixerVolume m_soundVolume;
		MixerVolume m_musicVolume;
        MixerVolume m_voiceVolume;

        uint32_t m_enumerator;

        typedef stdex::template_pool<SoundSourceDesc, 32> TPoolSoundSourceDesc;
        TPoolSoundSourceDesc m_poolSoundSourceDesc;

		typedef stdex::map<uint32_t, SoundSourceDesc *> TMapSoundSource;
		TMapSoundSource m_soundSourceMap;
		
        ThreadJobPtr m_threadSoundBufferUpdate;

        typedef FactoryPoolStore<ThreadWorkerSoundBufferUpdate, 32> TPoolWorkerTaskSoundBufferUpdate;
        TPoolWorkerTaskSoundBufferUpdate m_poolWorkerTaskSoundBufferUpdate;

		typedef stdex::vector<SoundVolumeProviderInterface *> TVectorSoundVolumeProviders;
		TVectorSoundVolumeProviders m_soundVolumeProviders;

		bool m_supportStream;
		bool m_muted;

		bool m_turnStream;
		bool m_turnSound;		
	};
};
