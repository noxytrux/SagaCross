#pragma once

#include "Core-pch.hpp"
#include "SCOpenGLRenderer.h"

namespace sc {

	class SCObjectRenderer;
	class SCRenderObj {

	public:

		float Pos[3];
		float vDir[3];
		float Rot[3];

		static SCObjectRenderer *manager;

        SCRenderObj(const std::shared_ptr<SCOpenGLRenderable> &renderer);

		virtual ~SCRenderObj() noexcept;

		virtual void Render()
		{
		}

		virtual void Simulate(float dt)
		{
		}

	protected:

		std::shared_ptr<SCOpenGLRenderable> _renderer;
	};

	class SCObjectRenderer final {

	public:

		void Render()
		{
			for (const auto &obj : _objs) {
				
				obj->Render();
			}
		}

		void Simulate(float dt)
		{
			for (const auto &obj : _objs) {

				obj->Simulate(dt);
			}
		}

		void AddObj(SCRenderObj *o)
		{
			_objs.push_back(o);
		}

		void DelObj(SCRenderObj *o)
		{
			for (int i = 0; i < _objs.size(); ++i) {

				if (_objs[i] == o) {

					_objs.erase(_objs.begin() + i);
					return;
				}
			}

		}

		void Clear() 
		{
			_objs.clear();
		}

	private:
		
		std::vector<SCRenderObj*> _objs;
	};
}

