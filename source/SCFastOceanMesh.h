//
//  glFastOceanMesh.h
//  SagaTanks
//
//  Created by Marcin Pędzimąż on 18.02.2014.
//  Copyright (c) 2014 Marcin Pędzimąż. All rights reserved.
//

#pragma once 

#include "SCFastMesh.h"

namespace sc {

	class SCFastOceanMesh : public SCFastMesh {

	public:

		GLuint caustic; 
		GLuint causticSecond; 

		float time;

		SCFastOceanMesh(const std::string &filename, const std::shared_ptr<SCRendererInterface> &renderer)
			: SCFastMesh(filename, renderer) 
			, caustic(0)
			, causticSecond(0)
			, time(0)
		{
		}

		virtual ~SCFastOceanMesh() noexcept = default;

		virtual void Draw() override {

			auto current = _renderer->WaterShader;

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, caustic);
			glUniform1i(current->uniforms[UNI_TEX1], 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, causticSecond);
			glUniform1i(current->uniforms[UNI_TEX2], 2);

			xMat33 scale = xMat33(xVec3(_scaleX, 0, 0), xVec3(0, _scaleY, 0), xVec3(0, 0, _scaleZ));

			_world = _userMatrix;
			_finalWorld = _world * xMat34(scale, xVec3());

			_worldmodelview = _renderer->ModelView * _finalWorld;

			glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.m());
			glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _worldmodelview.m());
			glUniform1f(current->uniforms[UNI_TEX3], time);

			for (int i = 0; i < _buffers.size(); i++) {

				glBindVertexArray(_buffers[i].VAO);
				glDrawElements(GL_TRIANGLES, _buffers[i].faceSize * 3, GL_UNSIGNED_SHORT, (void*)0);
			}

			glBindVertexArray(0);

		}
	};
}

