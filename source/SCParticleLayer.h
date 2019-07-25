#pragma once

#include "Core-pch.hpp"
#include "SCGround.h"

namespace sc {

	typedef SC_ENUM(uint32_t, SCGroundType) {
		
		SCGroundTypeSand,
		SCGroundTypeGrass,
		SCGroundTypeWater
	};

	class SCParticleLayer final {

	public:

		SCParticleLayer(const std::string &filename, const std::shared_ptr<SCGround> &ground) : _ground(ground)
		{
			std::fstream o(filename.c_str(), std::ios::in | std::ios::binary);

			uint32_t magic = 0xC5609A15;

			o.read((char*)&magic, 4);
			o.read((char*)&w, 4);
			o.read((char*)&h, 4);

			data = new unsigned char[w * h];

			o.read((char*)data, w*h);

			o.close();
		}

		~SCParticleLayer() noexcept
		{
			delete[] data;
			data = nullptr;
		}

		SCGroundType getCurrentGroundType(float x, float y)
		{

			if (x < -_ground->GetGroundSize() || y < -_ground->GetGroundSize() || x > _ground->GetGroundSize() || y > _ground->GetGroundSize()) {
			
				return SCGroundTypeGrass;
			}

			x += _ground->GetGroundSize();
			y += _ground->GetGroundSize();

			x /= 2.0;
			y /= 2.0;

			int cx = (int)x;
			int cy = (int)y;

			return (SCGroundType)data[cx + ((w - cy) * w)];
		}

	private:

		int w;
		int h;

		unsigned char *data;

		std::shared_ptr<SCGround> _ground;
	};
}
