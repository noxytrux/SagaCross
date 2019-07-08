#pragma once

#include "SCRenderer.h"
#include "SCHeightMap.h"
#include "SCMeshInstanceManager.h"

namespace sc {

	class SCGround final : public SCRenderObj
	{

	public:

		~SCGround() noexcept {
		
		}

		SCGround(const std::shared_ptr<SCOpenGLRenderable> &renderer,
				 const std::shared_ptr<SCMeshInstance> &meshInstance, 
				 const std::string &path, const std::string &base, bool shouldRenderWater = true)
			: SCRenderObj(renderer)
			, _renderingWaterEnabled(shouldRenderWater)
		{
			_texture = textureLoader.loadFile(path + base + "_color.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 

			_heightmap = std::make_unique<SCHeigthMap>(path + base + "_heightmap.png");
			_terrain = meshInstance->GetMesh(base + "_terrain", path, SCMeshTypeNormal);
			_terrain->setScale(1.0f);

			_water = std::make_unique<SCFastWaterMesh>(path + "models/object_level001-water-stream-fx.gmf");
			_water->setScale(1.0f);
		}

		inline float GetHeight(float x, float y)
		{

			if (x < -GetGroundSize() + 3 || y < -GetGroundSize() + 3 || x > GetGroundSize() - 3 || y > GetGroundSize() - 3)
				return 0.0f;

			x += GetGroundSize();
			y += GetGroundSize();

			x /= 2;
			y /= 2;

			return _heightmap->GetHeight(x, y);
		}

		inline xVec3 GetNormal(float x, float y)
		{

			if (x < -GetGroundSize() || y < -GetGroundSize() || x > GetGroundSize() || y > GetGroundSize())
				return xVec3(0,1,0);

			x += GetGroundSize();
			y += GetGroundSize();

			x /= 2;
			y /= 2;

			return _heightmap->GetNormal(x, y);
		}

		inline int GetGroundSize()
		{
			return 256;
		}

		virtual void Render() override
		{
			glEnable(GL_BLEND);
			
			auto current = _renderer->SimpleShader;

			glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, _texture);
			glUniform1i(current->uniforms[UNI_TEX0], 0);

			_terrain->getUserMatrix().id();
			_terrain->Draw();

			if (_renderingWaterEnabled) {

				_water->getUserMatrix().id();
				_water->Draw();
			}

			glDisable(GL_BLEND);
		}

		virtual void Simulate(float dt) override
		{
			_water->time += dt;
			
			if (_water->time > 9000.0f) {
			
				_water->time = 0.0f;
			}
		}

	private:

		std::shared_ptr<SCFastMesh> _terrain;
		std::unique_ptr<SCFastWaterMesh> _water;
		std::unique_ptr<SCHeigthMap> _heightmap;

		GLuint _texture;
		bool _renderingWaterEnabled;
	};

}