#pragma once

#include "SCSparkles.h"
#include "SCAudio.h"
#include "SCSphere.h"
#include "SCMeshInstanceManager.h"
#include "SCGround.h"
#include "SCVegetation.h"
#include "SCBonus.h"

namespace sc {

	class SCBullet final {

	public:

		SCBullet(const std::shared_ptr<SCMeshInstance> &meshInstance,
				 const std::shared_ptr<SCAudio> &audio,
				 const std::shared_ptr<SCGround> &ground,
				 const std::string &path,
				 float x, float y, float z,
				 float ax, float ay, float az)
			: _audio(audio)
			, _ground(ground)
		{
			balisticRotation = 0;
			texture = textureLoader.loadFile(path + "textures/mesh_tank-bullet-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
			bulletMesh = meshInstance->GetMesh("models/tank_bullet_01", path, SCMeshTypeNormal);
			bulletMesh->setScale(0.05);

			Pos[0] = x;
			Pos[1] = y;
			Pos[2] = z;

			Acc[0] = ax;
			Acc[1] = ay;
			Acc[2] = az;

			lPos = xVec3(Pos);

			parent = nullptr;

			memcpy(Sp, Pos, 12);
			memcpy(Sa, Acc, 12);

			NumLast = 0;
			C = 0;

			T = 0.0f;
			
			soundID = _audio->loadSound("sounds/whooo.wav");
		}

		~SCBullet() noexcept {
				
		}

		bool Simulate(float dt)
		{

			Pos[0] += Acc[0] * dt;
			Pos[1] += Acc[1] * dt;
			Pos[2] += Acc[2] * dt;

			Acc[0] *= 0.98f;
			Acc[1] -= 12.0f * dt;
			Acc[2] *= 0.98f;

			if (Acc[1] < 0.0f) {

				if (sound.NoSound()) {

					sound = _audio->playSound(soundID, Pos, 1.0f); 
				}

				sound.Update(Pos);
			}

			T += dt;

			balisticRotation += dt * 8.0;

			if (T > 60 || Pos[1] < _ground->GetHeight(Pos[0], Pos[2]))
			{

				if (!sound.NoSound()) sound.Stop();
				return true;
			}

			return false;
		}

		void Destroy()
		{
			Pos[1] = -2.0;
		}

		void Render()
		{
			auto renderer = bulletMesh->getRenderer();
			auto current = renderer->SimpleShader;

			glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);
			glBindTexture(GL_TEXTURE_2D, texture);
			
			xVec3 rPos = xVec3(Pos);
			xVec3 dir = rPos - lPos;
			lPos = rPos;
			dir.normalize();

			OY = std::atan2f(-dir.z, dir.x);
			OX = std::atan2f(dir.y, sqrt(dir.x * dir.x + dir.z * dir.z));

			quaternion.fromEulerAngles(0, OY, 0);
			quaternion2.fromEulerAngles(0, 0, OX);
			quaternion3.fromEulerAngles(balisticRotation, 0, 0);

			xMat34 mat;
			mat.t = rPos;
			mat.M.fromQuat(quaternion * quaternion2 * quaternion3);

			bulletMesh->setUserMatrix(mat);
			bulletMesh->Draw();
		}

		void *parent;

		float Pos[3];
		float Acc[3];

	private:

		float Sp[3];
		float Sa[3];

		int NumLast;
		int C;

		float T;
		float balisticRotation;

		SCAudioInfo sound;
		SCAudio::SoundID soundID;

		GLuint texture;
		std::shared_ptr<SCFastMesh> bulletMesh;

		xVec3 lPos;
		xQuat quaternion;
		xQuat quaternion2;
		xQuat quaternion3;

		float OY;
		float OX;

		std::shared_ptr<SCAudio> _audio;
		std::shared_ptr<SCGround> _ground;
	};

	class SCBulletManager final {

    private:

		std::shared_ptr<SCGround> ground;
		std::shared_ptr<SCBuilding> mapObjects;
		std::shared_ptr<SCBonusManager> Bonuses;
		std::shared_ptr<SCMeshInstance> meshInstance;
		std::shared_ptr<SCAudio> audio;
		std::string path;

    public:

        std::vector<std::shared_ptr<SCBullet>> bullets;

        SCBulletManager(const std::shared_ptr<SCGround> &g,
                        const std::shared_ptr<SCBuilding> &mo,
                        const std::shared_ptr<SCBonusManager> &b,
                        const std::shared_ptr<SCMeshInstance> &mi,
                        const std::shared_ptr<SCAudio> &a,
                        const std::string &p)
            : ground(g)
            , mapObjects(mo)
            , Bonuses(b)
            , meshInstance(mi)
            , audio(a)
            , path(p)
        {


        }

        ~SCBulletManager() noexcept = default;

		class SCExplode final {

		public:
			GLuint  texture;
			GLuint  textureExp;
			GLuint  textureExpGround;

			std::shared_ptr<SCFastMesh> explosionFirst;
			std::shared_ptr<SCFastMesh> explosionSecond;

			float Pos[3];
			float Acc[3];
			float Time;
			float alpha;

			SCExplode(const std::string &path, const std::shared_ptr<SCMeshInstance> &meshInstance) {

				Time = 0.0f;
				alpha = 1.0f;

				textureExpGround = textureLoader.loadFile(path + "textures/mesh_fx-ground-explosion-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
				textureExp = textureLoader.loadFile(path + "textures/mesh_fx-explosion01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 
				texture = textureLoader.loadFile(path + "textures/Kaboom.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 

				explosionFirst = meshInstance->GetMesh("models/fx_ground-explosion-b", path, SCMeshTypeNormal); 
				explosionSecond = meshInstance->GetMesh("models/fx_explosion-spherical-c", path, SCMeshTypeNormal); 

			}

			~SCExplode() noexcept = default;

			bool Draw()
			{
				auto renderer = explosionFirst->getRenderer();
				auto current = renderer->GuiShader;

				glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, renderer->Projection.getMatrix());
				glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, renderer->ModelView.getMatrix());

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture);
				glUniform1i(current->uniforms[UNI_TEX0], 0);

				float alpha = (float)std::min((int)(400 - Time * 4), 255) / 255.0;
				float color[4] = { 1.0f, 1.0f, 1.0f, alpha };
				glUniform4fv(current->uniforms[UNI_TEX1], 1, color);

				const int S = 6;

				float squareVertices[] = {
					-S + Pos[0], Pos[1] + 0.5f,  S + Pos[2],
					 S + Pos[0], Pos[1] + 0.5f,  S + Pos[2],
					-S + Pos[0], Pos[1] + 0.5f, -S + Pos[2],
					 S + Pos[0], Pos[1] + 0.5f, -S + Pos[2]
				};

				static float textureVertices[] = {
					1, 1,
					0, 1,
					1, 0,
					0, 0
				};

				glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, squareVertices);
				glEnableVertexAttribArray(ATTRIB_VERTEX);
				glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 0, textureVertices);
				glEnableVertexAttribArray(ATTRIB_COORDS);
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

				return Time > 100;
			}

			void Kaboom(float dt)
			{
				auto renderer = explosionFirst->getRenderer();
				auto current = renderer->SimpleShader;
				
				Time += dt * 5.0;

				if (Time < 3.0)
				{
					alpha = (3.0f - Time) / 3.0f;
					glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, alpha);
					glBindTexture(GL_TEXTURE_2D, textureExpGround);
					
					xMat34 &UserMatrix = explosionFirst->getUserMatrix();

					UserMatrix.t = xVec3(Pos);
					UserMatrix.t.y += 1.5;
					UserMatrix.M.rotY(-(Time*0.5));

					explosionFirst->setScale(xVec3(0.1 * 2.0, (0.1 * (1.1 - std::min(Time, 1.0f))) * 2.0, 0.1 * 2.0));
					explosionFirst->Draw();

					glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);

					if (Time < 1.0f)
					{
				
						glBindTexture(GL_TEXTURE_2D, textureExp);
				
						UserMatrix = explosionSecond->getUserMatrix();

						UserMatrix.t = xVec3(Pos);
						UserMatrix.M.id();

						explosionSecond->setScale(xVec3(0.005, (0.05 + 0.1 * (Time / 1.0)), 0.005));
						explosionSecond->Draw();
					}
				}

				if (Time < 1.0)
				{
					Sparcles.Add(Pos[0], Pos[1], Pos[2], Acc[0] * 0.2, -0.75*Acc[1], Acc[2] * 0.2, 4, 77, 56, 43);
					LightSparcles.Add(Pos[0], Pos[1], Pos[2], Acc[0] * 0.3, -0.5*Acc[1], Acc[2] * 0.3, 2, 244, 142, 74, 1 + rand() % 2);
					ParticleSmoke.Add(Pos[0], Pos[1] + 2, Pos[2], rand() % 3 - 1, 4, rand() % 3 - 1, 8, 12, 12, 12, 1);
				}

			}

		};

		std::vector<std::shared_ptr<SCExplode>> exp;

		void DrawExplodes()
		{
			glEnable(GL_BLEND);

			for (int i = 0; i < exp.size(); ) {

				if (exp[i]->Draw())
				{
					exp.erase(exp.begin() + i);
				}
				else {

					++i;
				}
			}

			glDisable(GL_BLEND);
		}

		void DestroyMines(const std::shared_ptr<SCBullet> &current) {

			for (int a = 0; a < Bonuses->B.size(); )
			{
				auto other = Bonuses->B[a];
				xVec3 G(current->Pos);
				xVec3 P(other->Pos);

				float L = (P - G).magnitude();

				if ((rand() % 100) > (L * 9.5f)) {

					Bonuses->B.erase(Bonuses->B.begin() + a);
				}
				else {

					++a;
				}
			}
		}

		void RemoveAllObjects() {

			exp.clear();
			bullets.clear();
		}

		void DrawClouds(float dt)
		{
			glDepthMask(0);
			glEnable(GL_BLEND);
			glDisable(GL_CULL_FACE);

			for (int i = 0; i < exp.size(); ++i) {

				exp[i]->Kaboom(dt);
			}

			glEnable(GL_CULL_FACE);
			glDepthMask(1);
			glDisable(GL_BLEND);
		}

		void Simulate(float dt)
		{

			for (int i = 0; i < bullets.size(); ) {

				if (bullets[i]->Simulate(dt))
				{

					auto e = std::make_shared<SCExplode>(path, meshInstance); 
					e->Time = 0.0f;

					memcpy(e->Pos, bullets[i]->Pos, 12);
					memcpy(e->Acc, bullets[i]->Acc, 12);

					SCAudio::SoundID kaboomID = audio->loadSound("sounds/kaboom.wav");
					audio->playSound(kaboomID, e->Pos);

					exp.push_back(e);

					DestroyMines(bullets[i]);

					if (this->mapObjects) {

						std::vector<std::shared_ptr<SCModel>> & objectsQueue = mapObjects->mainModelQueue;

						float len = 0;
						xVec3 dir;
						std::shared_ptr<SCModel> model = nullptr;

						for (int a = 0; a < objectsQueue.size(); a++) {

							model = objectsQueue[a];

							dir = xVec3(e->Pos) - model->matrix.t;
							
							len = dir.magnitude();

							if (len < 20.0 && model->destructible && !model->destroyed) {
							
								model->destroyed = true;
							}
						}

					}

					bullets.erase(bullets.begin() + i);
				}
				else {

					++i;
				}
			}
		}

		void Render()
		{
			glEnable(GL_BLEND);
			
			for (int i = 0; i < bullets.size(); ++i) {
			
				bullets[i]->Render();
			}

			glDisable(GL_BLEND);
		}

		void AddBullet(const std::shared_ptr<SCBullet> &b)
		{
			bullets.push_back(b);
		}
	};
}
