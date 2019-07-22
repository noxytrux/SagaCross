//
//  glFastCutoffMesh.h
//  SagaTanks
//
//  Created by Marcin Pędzimąż on 30.09.2013.
//  Copyright (c) 2013 Marcin Pędzimąż. All rights reserved.
//

#pragma once 

#include "SCFastMesh.h"

namespace sc {

	class SCFastCutOffMesh : public SCFastMesh {

	public:

		float cutoff;

		SCFastCutOffMesh(const std::string &filename, const std::shared_ptr<SCRendererInterface> &renderer)
			: SCFastMesh(filename, renderer) 
			, cutoff(0.0f)
		{
		}

		virtual ~SCFastCutOffMesh() noexcept = default;
		
		virtual void Draw() override {

			auto current = _renderer->CutoffShader;

            xMat33 scale = xMat33(xVec3(_scaleX, 0, 0), xVec3(0, _scaleY, 0), xVec3(0, 0, _scaleZ));

            float userMat[16];
            _userMatrix.getColumnMajor44(userMat);
            glMatrix4x4 scaleMat; scaleMat.scale(_scaleX, _scaleY, _scaleZ);

            _finalWorld.multiply(_finalWorld.getMatrix(), userMat, scaleMat.getMatrix());

            auto pos = _finalWorld.t();

            if (!_renderer->getFrustum().SphereInFrustum(pos.x, pos.y, pos.z, _AABBRay * std::max(_scaleX, std::max(_scaleY, _scaleZ)))) return;

            _worldmodelview.multiply(_worldmodelview.getMatrix(), _renderer->ModelView.getMatrix(), _finalWorld.getMatrix());

			glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.getMatrix());
			glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _worldmodelview.getMatrix());
			glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);
			glUniform1f(current->uniforms[UNI_TEX2], cutoff);

			for (int i = 0; i < _buffers.size(); i++) { //draw all mesh

				glBindVertexArray(_buffers[i].VAO);
				glDrawElements(GL_TRIANGLES, _buffers[i].faceSize * 3, GL_UNSIGNED_SHORT, (void*)0);
			}

			glBindVertexArray(0);
		}
	};
}

