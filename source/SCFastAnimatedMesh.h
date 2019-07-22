#pragma once

#include "SCFastMesh.h"

namespace sc {

	class SCFastAnimatedMesh final : public SCFastMesh {

	public:
		
		float time;
		float speed;
		float amplitude;

		SCFastAnimatedMesh(const std::string &filename, const std::shared_ptr<SCRendererInterface> &renderer) : SCFastMesh(filename, renderer) {
			time = 0;
			speed = 2.0f;
			amplitude = 0.02f;
		}

		virtual ~SCFastAnimatedMesh() noexcept = default;

		virtual void Draw() override {

            xMat33 scale = xMat33(xVec3(_scaleX, 0, 0), xVec3(0, _scaleY, 0), xVec3(0, 0, _scaleZ));

            float userMat[16];
            _userMatrix.getColumnMajor44(userMat);
            glMatrix4x4 scaleMat; scaleMat.scale(_scaleX, _scaleY, _scaleZ);

            _finalWorld.multiply(_finalWorld.getMatrix(), userMat, scaleMat.getMatrix());

            auto pos = _finalWorld.t();

            if (!_renderer->getFrustum().SphereInFrustum(pos.x, pos.y, pos.z, _AABBRay * std::max(_scaleX, std::max(_scaleY, _scaleZ)))) return;

            _worldmodelview.multiply(_worldmodelview.getMatrix(), _renderer->ModelView.getMatrix(), _finalWorld.getMatrix());

			auto current = _renderer->FoliageShader;

			glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.getMatrix());
			glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _worldmodelview.getMatrix());

			glUniform1f(current->uniforms[UNI_TEX1], time);
			glUniform1f(current->uniforms[UNI_TEX2], speed);
			glUniform1f(current->uniforms[UNI_TEX3], amplitude);

			for (int i = 0; i < _buffers.size(); i++) { //draw all mesh

				glBindVertexArray(_buffers[i].VAO);
				glDrawElements(GL_TRIANGLES, _buffers[i].faceSize * 3, GL_UNSIGNED_SHORT, (void*)0);
			}

			glBindVertexArray(0);

		}
	};
}

