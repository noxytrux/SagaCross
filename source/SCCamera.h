#pragma once

#include "Core-pch.hpp"
#include "SCRendererInterface.h"
#include "SCSettings.h"

namespace sc {

	class SCCamera final {

	public:

		float *LookAt;

		void AddShake(float force)
		{
			shake = std::max(force, shake);
		}

		SCCamera(const std::shared_ptr<SCSettings> &settings, 
				 const std::shared_ptr<SCRendererInterface> &renderer, 
				 float cx, float cy, float cz) 
			: LookAt(nullptr)
			, _settings(settings)
			, _renderer(renderer)
		{
			cameraRoatation.id();

			C[0] = cx;
			C[1] = cy;
			C[2] = cz;

			angle_y = 0.0;
			angle_x = -0.1f;

			speed = 0.5f;
			shake = 0.0f;

			memset(Sr, 0, 12);
			memset(S, 0, 12);
			memset(E, 0, 12);

			k_v3fLookAtPos = xVec3(0.0, 1.3, 0.0);
			k_v3LookFromPos = xVec3(0.0, 80 /*THIS IS CAM HEIGHT!!!*/, 0.0);

			s_fFacing = 1.0;
			yPosAddition = 0;
		}

		void Apply()
		{
			shake *= 0.75;

			for (int i = 0; i < 3; ++i)
			{
				Sr[i] = (rand() % 5 - 3) * shake;
				S[i] += (Sr[i] - S[i]) * 0.3f;
			}

			if (LookAt)
			{
				static xVec3 v3Position = xVec3(0, 0, 0);
				static xVec3 v3LookDirection = xVec3(0, 0, 0);
				static xVec3 vLastPosDir;

				v3Position = xVec3(LookAt[0], LookAt[1], LookAt[2]);
				float CameraDistance = 60.0;

				xVec3 v3LookFromPos;

				vLastPosDir = v3Position + xVec3(2.0, 0.0, 0.0);
				v3LookDirection = v3Position - vLastPosDir;

				float fMag = v3LookDirection.magnitude();

				if (fMag > 0.0f) {
					
					v3LookDirection /= fMag;
				}

				if (!_settings->usesSecondCam()) {
		
					cameraRoatation.multiply(v3LookDirection, v3LookDirection);
				}

				if (v3LookDirection.y > 0.3f) v3LookDirection.y = 0.3f;
				if (v3LookDirection.y < -0.3f) v3LookDirection.y = -0.3f;

				v3LookFromPos = v3Position - v3LookDirection * CameraDistance;

				if (fabs(v3LookDirection.y) <= 0.99f)
				{
					k_v3LookFromPos.y = 80 + yPosAddition;
					v3LookFromPos += k_v3LookFromPos;

					xVec3 v3LookAtPos = (v3Position + k_v3fLookAtPos);

					xVec3 Ray = v3LookFromPos - v3LookAtPos;
					xVec3 d(Ray);
					d.normalize();

					gDir = -d;
					gDir.normalize();
				}

				E[0] = v3LookFromPos.x + S[0];
				E[1] = v3LookFromPos.y + S[1];
				E[2] = v3LookFromPos.z + S[2];

				gEye = xVec3(E);

				auto vDir = xVec3(E[0] + gDir.x, E[1] + gDir.y, E[2] + gDir.z);
				auto vUp = xVec3(0, 1, 0);

				_renderer->ModelView.mLookAt(gEye, vDir, vUp);
			}
			else
			{
				auto vUp = xVec3(0, 1, 0);
				auto vC = xVec3(C);

				_renderer->ModelView.mLookAt(vC, gEye, vUp);
			}
		}

		void GoForward(float dir = 1.0f)
		{

			for (int i = 0; i < 3; ++i) {

				C[i] += (E[i] - C[i]) * speed * dir;
			}
		}

		void GoPlane(float add)
		{

			C[0] -= cos(angle_y + add) * speed;
			C[2] -= sin(angle_y + add) * speed;
		}

	public:

		xMat33 cameraRoatation;
		float yPosAddition;

	private:

		std::shared_ptr<SCSettings> _settings;
		std::shared_ptr<SCRendererInterface> _renderer;

		float E[3];
		float C[3];

		float angle_y;
		float angle_x;

		float speed;
		float shake;

		float S[3], Sr[3];

		float  s_fFacing;
		xVec3 k_v3fLookAtPos;
		xVec3 k_v3LookFromPos;
	
		xVec3 gDir;
		xVec3 gEye;
	};

}

