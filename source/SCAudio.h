#pragma once

#include "Core-pch.hpp"
#include "fmod.hpp"

namespace sc {

	inline void FMOD_Channel_SetPitchCustom(FMOD::Channel *channel, float p)
	{
		float freq;
		channel->getFrequency(&freq);
		freq *= p;
		channel->setFrequency(freq);
	}

	class SCAudioInfo final {

	public:

		FMOD::Channel *channel;

		SCAudioInfo() : channel(nullptr) {}

		bool NoSound()
		{
			return channel == nullptr;
		}

		void Update(xVec3 & Pos, float pitch = 1.0f)
		{
			xVec3 vel(0, 0, 0);

			channel->set3DAttributes((FMOD_VECTOR*)&Pos, (FMOD_VECTOR*)&vel);
			
			if (pitch != 1.0f) {

				FMOD_Channel_SetPitchCustom(channel, pitch);
			}
		}

		void Stop()
		{
			channel->stop();
		}
	};

	class SCAudio final {

	public:

		typedef uint32_t SoundID;

		SCAudio();
		~SCAudio() noexcept;

		FMOD::System *getSystem() const { return _system; }

		SoundID loadSound(const std::string & path) const;

		void playSound(const SoundID & sid) const;
		SCAudioInfo playSound(const SoundID & sid, const xVec3 & Pos, float volume = 1.0f, float pitch_rand = 0.0f);
		void stopSound(const SoundID & sid) const;

		void loadMusic(const std::string & path) const;
		void playMusic() const;
		void pauseMusic() const;

		void mute() const;
		void unmute() const;

		bool isMuted() { return _muted; }
		void setVolume(float volume);
		float getVolume();

		//required by webGL
		void update();

	private:

		void unload();
		void checkResult(FMOD_RESULT result) const;

		mutable bool             _muted;
		mutable FMOD::System     *_system;
		mutable FMOD::Channel    *_channel;
		mutable FMOD_RESULT      _result;
		unsigned int      _version;
		mutable int       _numsubsounds;

		mutable FMOD::Channel    *_schannel;
		mutable FMOD::Sound      *_stream, *_stream_to_play;

		FMOD::ChannelGroup *_masterGroup;

		mutable std::vector<FMOD::Sound *> _sounds;
		mutable std::vector<std::string> _paths;
		float _volume;
	};

	class SCAudioListener final {

	public:

		SCAudioListener(const std::shared_ptr<SCAudio> &m) : _manager(m) {}
		~SCAudioListener() noexcept = default;

		void Update(xVec3 & center, xVec3 & eye)
		{
			const xVec3 up(0, 1, 0);
			xVec3 forward = eye - center;
			forward.normalize();

			xVec3 vel = (_lastPos - eye).operator*(0.25f) / (1.0f / 60.0f);

			_manager->getSystem()->set3DListenerAttributes(0, (FMOD_VECTOR*)&eye, (FMOD_VECTOR*)&vel, (FMOD_VECTOR*)&forward, (FMOD_VECTOR*)&up);

			_lastPos = eye;

			_manager->getSystem()->update();
		}

	private:

		std::shared_ptr<SCAudio> _manager;
		xVec3 _lastPos;
	};
}