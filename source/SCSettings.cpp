#include "SCSettings.h"

using namespace sc;

SCSettings::SCSettings(const std::string &path) : _path(path), _settings({})
{
	//some defaults
	_settings.width = 1024;
	_settings.height = 768;
	_settings.volume = 0.1f;
	_settings.fullscreen = false;
	_settings.vsync = true;
	_settings.mute = false;
	_settings.hardAI = true;
	_settings.secondCam = false;

	userDefaults = json::object();
}

SCSettings::~SCSettings()
{
}

bool SCSettings::load() 
{
	std::fstream m_file;

	m_file.open(_path.c_str(), std::fstream::in | std::fstream::binary);

	if (!m_file) {

		std::cerr << "Error Durning loading file: " << _path << " File not found or data corrupted" << std::endl;
		m_file.close();

		return false;
	}

	m_file.read((char*)&_settings, sizeof(_settings));

	m_file.close();

	return true;
}

bool SCSettings::save()
{
	std::fstream m_file;

	m_file.open(_path.c_str(), std::fstream::out | std::fstream::binary);

	if (!m_file) {
		std::cerr << "Error Durning opening file: " << _path << " propably no disk space" << std::endl;
		m_file.close();
		return false;
	}

	m_file.write((const char*)&_settings, sizeof(_settings));

	m_file.close();

	return true;
}