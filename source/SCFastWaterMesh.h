//
//  glFastWaterMesh.h
//  SagaTanks
//
//  Created by Marcin Pędzimąż on 14.09.2013.
//  Copyright (c) 2013 Marcin Pędzimąż. All rights reserved.
//

#pragma once 

#include "SCFastMesh.h"

namespace sc {

	class SCFastWaterMesh : public SCFastMesh {

	public:
		
		GLuint tex; 
		float time;

		SCFastWaterMesh(const std::string &filename, const std::shared_ptr<SCRendererInterface> &renderer)
			: SCFastMesh(filename, renderer) 
			, tex(0)
			, time(0) 
		{
		}

		virtual ~SCFastWaterMesh() noexcept = default;

		virtual void Draw() override {
			
			auto current = _renderer->TerrainShader;
			current->begin();

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(current->uniforms[UNI_TEX0], 0);

            xMat33 scale = xMat33(xVec3(_scaleX, 0, 0), xVec3(0, _scaleY, 0), xVec3(0, 0, _scaleZ));

            float userMat[16];
            _userMatrix.getColumnMajor44(userMat);
            glMatrix4x4 scaleMat; scaleMat.scale(_scaleX, _scaleY, _scaleZ);

            _finalWorld.multiply(_finalWorld.getMatrix(), userMat, scaleMat.getMatrix());

            auto pos = _finalWorld.t();

            if (!_renderer->getFrustum()->SphereInFrustum(pos.x, pos.y, pos.z, _AABBRay * std::max(_scaleX, std::max(_scaleY, _scaleZ)))) return;

            _worldmodelview.multiply(_worldmodelview.getMatrix(), _renderer->ModelView.getMatrix(), _finalWorld.getMatrix());

            glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.getMatrix());
            glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _worldmodelview.getMatrix());

			glUniform1f(current->uniforms[UNI_TEX1], time);

			for (int i = 0; i < _buffers.size(); i++) { 

				glBindVertexArray(_buffers[i].VAO);
				glDrawElements(GL_TRIANGLES, _buffers[i].faceSize * 3, GL_UNSIGNED_SHORT, (void*)0);
			}

			glBindVertexArray(0);

			_renderer->SimpleShader->begin();
		}
	
	};

}

