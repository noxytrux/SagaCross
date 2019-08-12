#include "SCAudio.h"
#include "fmod_errors.h"

using namespace sc;

SCAudio::SCAudio(const std::string &path) : _path(path) {
	_muted = false;

    FMOD::Debug_Initialize(FMOD_DEBUG_LEVEL_NONE);

	_result = FMOD::System_Create(&_system);
	this->checkResult(_result);

	_result = _system->getVersion(&_version);
	this->checkResult(_result);

	if (_version < FMOD_VERSION) {

		std::cerr << "FMOD lib version " << _version << " doesn't match header version " << FMOD_VERSION << std::endl;
	}

#if defined(__linux__) && !defined(__ANDROID__)

	_result = _system->setOutput(FMOD_OUTPUTTYPE_ALSA);
	this->checkResult(_result);

	_result = _system->setStreamBufferSize(32768, FMOD_TIMEUNIT_RAWBYTES);
	this->checkResult(_result);

	_result = _system->setDSPBufferSize(4096, 4);
	this->checkResult(_result);

#endif

#ifdef _RPI_

	//this could be different depending on PI double check this in future
	_system->setDriver(1);

#endif // _RPI_

	_paths.clear();

	_result = _system->init(32, FMOD_INIT_NORMAL, nullptr);
	this->checkResult(_result);

	_result = _system->getMasterChannelGroup(&_masterGroup);
	this->checkResult(_result);

	_channel = 0;
	_schannel = 0;

	_volume = 1.0;

	_stream = nullptr;
	_stream_to_play = nullptr;

	_result = _system->set3DSettings(1.0f, 1.0f, 1.0f);
	this->checkResult(_result);

	_result = _system->setGeometrySettings(250.0f);
	this->checkResult(_result);
}

SCAudio::~SCAudio() noexcept {

	this->unload();
}

void SCAudio::update() {

	_system->update();
}

void SCAudio::checkResult(FMOD_RESULT result) const {

	if (result == FMOD_OK) {
		return;
	}

	std::cout << "FMOD error: " << result << "-" << FMOD_ErrorString(result) << std::endl;
}

SCAudio::SoundID SCAudio::loadSound(const std::string & path) const {

	FMOD::Sound *sound;

	auto it = std::find(_paths.begin(), _paths.end(), path);

	if (it != _paths.end()) {

		return (SoundID)std::distance(_paths.begin(), it);
	}

	_result = _system->createSound((_path + path).c_str(), FMOD_DEFAULT, 0, &sound);
	this->checkResult(_result);

	_result = sound->setMode(FMOD_LOOP_OFF);
	this->checkResult(_result);

	_sounds.push_back(sound);
	_paths.push_back(path);

	return (SoundID)_sounds.size() - 1;
}

void SCAudio::playSound(const SoundID & sid) const {

	_result = _system->playSound(_sounds[sid], 0, false, &_channel);
	this->checkResult(_result);
}

SCAudioInfo SCAudio::playSound(const SoundID & sid, const xVec3 & Pos, float volume, float pitch_rand) {

	SCAudioInfo info;

	FMOD::Channel * & channel = info.channel;

	_system->playSound(_sounds[sid], 0, true, &channel);
	channel->set3DAttributes((FMOD_VECTOR*)&Pos, nullptr);
	channel->set3DMinMaxDistance(10, 40);
	channel->setVolume(volume);
	FMOD_Channel_SetPitchCustom(channel, 1.0f + (rand() % 200 - 100) * 0.01f * pitch_rand);
	channel->setPaused(false);

	return info;
}

void SCAudio::stopSound(const SoundID & sid) const {

	bool paused;
	_result = _channel->getPaused(&paused);
	this->checkResult(_result);

	if (paused == true) {
		return;
	}

	_result = _channel->setPaused(true);
	this->checkResult(_result);
}

void SCAudio::loadMusic(const std::string & path) const {

	if (_stream) {

		//force pause
		_schannel->setPaused(true);
		_stream->release();
	}

	_result = _system->createStream((_path + path).c_str(), FMOD_LOOP_NORMAL | FMOD_2D, 0, &_stream);
	this->checkResult(_result);

	_result = _stream->getNumSubSounds(&_numsubsounds);
	this->checkResult(_result);

	if (_numsubsounds) {

		_stream->getSubSound(0, &_stream_to_play);
		this->checkResult(_result);
	}
	else {

		_stream_to_play = _stream;
	}
}

void SCAudio::playMusic() const {

	_result = _system->playSound(_stream_to_play, 0, false, &_schannel);
	this->checkResult(_result);
}

void SCAudio::pauseMusic() const {

	bool paused;
	_result = _schannel->getPaused(&paused);
	this->checkResult(_result);

	if (paused == true) {
		return;
	}

	_result = _schannel->setPaused(true);
	this->checkResult(_result);
}

void SCAudio::mute() const {

	if (_muted) {
		return;
	}

	_muted = true;

	_masterGroup->setMute(true);
}

void SCAudio::unmute() const {

	if (!_muted) {
		return;
	}

	_muted = false;

	_masterGroup->setMute(false);
}

void SCAudio::setVolume(float volume) {

	if (!this->isMuted()) {

		_masterGroup->setVolume(volume);
	}

	_volume = volume;
}

float SCAudio::getVolume() {

	_masterGroup->getVolume(&_volume);

	return _volume;
}

void SCAudio::unload() {

	this->pauseMusic();

	if (_stream) {
		_result = _stream->release();
		this->checkResult(_result);
	}

	for (auto sound : _sounds) {

		_result = sound->release();
		this->checkResult(_result);
	}

	_sounds.clear();
	_paths.clear();

	_result = _system->close();
	this->checkResult(_result);
	_result = _system->release();
	this->checkResult(_result);
}

void SCAudio::unloadSounds() {

    for (auto sound : _sounds) {

        _result = sound->release();
        this->checkResult(_result);
    }

    _sounds.clear();
    _paths.clear();
}
