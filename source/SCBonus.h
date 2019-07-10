#pragma once

#include "SCGround.h"
#include "SCAudio.h"
#include "SCRandomMeshPoint.h"
#include "SCVehicle.hpp"
#include "SCTank.h"

namespace sc {

	class SCBonus {

	public:

		virtual ~SCBonus() noexcept = default;

		SCBonus(const std::shared_ptr<SCRandomMeshPoint> &randomMeshPoint, 
				const std::shared_ptr<SCGround> &ground) 
			: _ground(ground)
		{
			xVec3 rnd = randomMeshPoint->GetRandomPoint(); 

			Pos[0] = rnd.x;
			Pos[2] = rnd.z;
			Pos[1] = 500;
			first_drop = true;
			drop = 0.0f;
			T = 0.0f;
			MaxTime = 200.0f;

			bonusMatrix.t = xVec3(0, 0, 0);
			bonusMatrix.M.id();
		}

		virtual bool Draw() = 0;

		bool Simulate(float dt)
		{
			T += dt;

			drop += 2.0f * dt;

			Pos[1] -= drop * dt;

			float h = _ground->GetHeight(Pos[0], Pos[2]);

			if (Pos[1] < h)
			{

				if (first_drop) {
					
					Sparcles.Add(Pos[0], Pos[1], Pos[2], 0, 15, 0, 40, 180, 180, 180);
				}

				first_drop = false;
				Pos[1] = h;
			}

			return T > MaxTime;
		}

		virtual bool Give(const std::shared_ptr<SCVehicleObj> &obj, const std::shared_ptr<SCAudio> &audio) = 0;

	public:

		float Pos[3];

	protected:

		float drop;
		float T, MaxTime;
		float first_drop;
		float alpha;
		float disable;

		xMat34 bonusMatrix;
		std::shared_ptr<SCGround> _ground;
	};

	class SCHeathPack final : public SCBonus {

	public:

		virtual ~SCHeathPack() noexcept {
	
		}

		SCHeathPack(const std::string &path,
					const std::shared_ptr<SCRandomMeshPoint> &randomMeshPoint,
				    const std::shared_ptr<SCGround> &ground,
					const std::shared_ptr<SCMeshInstance> &meshInstance)
			: SCBonus(randomMeshPoint, ground)
		{

			texture = textureLoader.loadFile(path + "textures/mesh_object_wooden-crate-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 
			healtMesh = meshInstance->GetMesh("models/object_wooden-crate-closed-A", path, SCMeshTypeNormal);
			healtMesh->setScale(0.04);

			parachuteMesh = meshInstance->GetMesh("models/object_parachute", path, SCMeshTypeNormal); 
			parachuteMesh->setScale(0.04);

			for (int i = 0; i < SCVehicle::all.size(); ++i) {
			
				if ((!SCVehicle::all[i]->isGoal && SCVehicle::all[i]->live < 50 && rand() % 2 != 0) || SCVehicle::all[i]->live < 30)
				{

					memcpy(SCVehicle::all[i]->Goal, Pos, 12);
					SCVehicle::all[i]->isGoal = true;
				}
			}
		}

		virtual bool Draw() override
		{
			auto renderer = healtMesh->getRenderer();
			auto current = renderer->SimpleShader;

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);

			glBindTexture(GL_TEXTURE_2D, texture);
			glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);

			bonusMatrix.t = xVec3(Pos[0], Pos[1], Pos[2]);
			healtMesh->setUserMatrix(bonusMatrix);
			healtMesh->Draw();

			if (first_drop) {

				parachuteMesh->setUserMatrix(bonusMatrix);
				parachuteMesh->Draw();
			}

			return false;
		}

		virtual bool Give(const std::shared_ptr<SCVehicleObj> &obj, const std::shared_ptr<SCAudio> &audio) override
		{

			char & L = std::dynamic_pointer_cast<SCTank>(obj)->live;

			L += 40;

			if (L > 60) {

				L = 60;
			}

			SCAudio::SoundID bonusID = audio->loadSound("sounds/bonus.wav");
			audio->playSound(bonusID, Pos, 1.0f, 0.2f);

			return true;
		}

	private:

		GLuint texture;
		std::shared_ptr<SCFastMesh> healtMesh;
		std::shared_ptr<SCFastMesh> parachuteMesh;
	};

	class SCMineBonus final : public SCBonus {

	public:

		virtual ~SCMineBonus() noexcept {
	
		}

		SCMineBonus(const std::string &path,
					const std::shared_ptr<SCRandomMeshPoint> &randomMeshPoint,
					const std::shared_ptr<SCGround> &ground,
					const std::shared_ptr<SCMeshInstance> &meshInstance)
			: SCBonus(randomMeshPoint, ground)
		{
			texture = textureLoader.loadFile(path + "textures/mesh_object_wooden-crate-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 
			textureMine = textureLoader.loadFile(path + "textures/mesh_tank_anti-tank-mine.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);  

			boxMesh = meshInstance->GetMesh("models/object_wooden-crate-A", path, SCMeshTypeNormal);
			mineMesh = meshInstance->GetMesh("models/object_wooden-crate-A_mine", path, SCMeshTypeNormal); 

			mineMesh->setScale(0.04);
			boxMesh->setScale(0.04);

			parachuteMesh = meshInstance->GetMesh("models/object_parachute", path, SCMeshTypeNormal);
			parachuteMesh->setScale(0.04);

			for (int i = 0; i < SCVehicle::all.size(); ++i) {
			
				if (!SCVehicle::all[i]->isGoal && rand() % 3 != 0)
				{

					memcpy(SCVehicle::all[i]->Goal, Pos, 12);
					SCVehicle::all[i]->isGoal = true;
				}
			}

		}

		virtual bool Draw() override
		{
			
			auto renderer = boxMesh->getRenderer();
			auto current = renderer->SimpleShader;

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);
			glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);

			glBindTexture(GL_TEXTURE_2D, texture);

			bonusMatrix.t = xVec3(Pos[0], Pos[1], Pos[2]);

			boxMesh->setUserMatrix(bonusMatrix);
			boxMesh->Draw();

			if (first_drop) {

				parachuteMesh->setUserMatrix(bonusMatrix);
				parachuteMesh->Draw();
			}

			glBindTexture(GL_TEXTURE_2D, textureMine);

			mineMesh->setUserMatrix(bonusMatrix);
			mineMesh->Draw();

			return false;
		}

		virtual bool Give(const std::shared_ptr<SCVehicleObj> &obj, const std::shared_ptr<SCAudio> &audio) override
		{

			std::dynamic_pointer_cast<SCTank>(obj)->mines++;

			SCAudio::SoundID bonusID = audio->loadSound("sounds/bonus.wav");
			audio->playSound(bonusID, Pos, 1.0f, 0.2f);

			return true;
		}

	private:

		GLuint texture;
		GLuint textureMine;

		std::shared_ptr<SCFastMesh> boxMesh;
		std::shared_ptr<SCFastMesh> mineMesh;
		std::shared_ptr<SCFastMesh> parachuteMesh;
	};

	class SCMine final : public SCBonus {

	public:

		virtual ~SCMine() noexcept {

		}

		SCMine(const std::shared_ptr<SCVehicleObj> w,
  	   		   const std::string &path,
			   const std::shared_ptr<SCRandomMeshPoint> &randomMeshPoint,
			   const std::shared_ptr<SCGround> &ground,
			   const std::shared_ptr<SCMeshInstance> &meshInstance)
			: SCBonus(randomMeshPoint, ground), ptr(w)
		{

			texture = textureLoader.loadFile(path + "textures/mesh_tank_anti-tank-mine.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
			mineMesh = meshInstance->GetMesh("models/tank_anti-tank-mine", path, SCMeshTypeNormal);
			mineMesh->setScale(0.025);

			if (ptr)
			{
				memcpy(Pos, ptr->Pos, 12);
			}
			else {
                
				Pos[1] = 5;
			}

			drop = -1;
			MaxTime = 1000;
			T = 0;
		}

		virtual bool Draw() override
		{
			auto renderer = mineMesh->getRenderer();
			auto current = renderer->SimpleShader;

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);
			glBindTexture(GL_TEXTURE_2D, texture);

			if (T < 1.0) {

				glUniform4f(current->uniforms[UNI_TEX1], 68.0 / 255.0, 1.0, 68.0 / 255.0, 1.0);
			}
			else
			{
				if ((int)T % 2) {
					
					glUniform4f(current->uniforms[UNI_TEX1], 1.0, 32.0 / 255.0, 32.0 / 255.0, 1.0);
				}
				else {
				
					glUniform4f(current->uniforms[UNI_TEX1], 1.0, 1.0, 1.0, 1.0);
				}
			}

			bonusMatrix.t = xVec3(Pos[0], Pos[1] + 0.1, Pos[2]);

			mineMesh->setUserMatrix(bonusMatrix);
			mineMesh->Draw();

			return false;

		}

		virtual bool Give(const std::shared_ptr<SCVehicleObj> &obj, const std::shared_ptr<SCAudio> &audio) override
		{

			if (T < 0.2 || obj == ptr) {

				return false;
			}

			auto tank = std::dynamic_pointer_cast<SCTank>(obj);

			if (tank->Damage(20))
			{
				if (ptr) {

					tank->Score();
				}

			}
			else {

				tank->Explode();
			}

			return true;
		}

	private:

		std::shared_ptr<SCVehicleObj> ptr;
		GLuint texture;
		std::shared_ptr<SCFastMesh> mineMesh;
	};

	class SCBonusManager final {

	public:

		SCBonusManager(const std::shared_ptr<SCAudio> audio) 
			: deltaTime(0)
			, _audio(audio)
		{
		}

		~SCBonusManager() noexcept = default;

		std::vector<std::shared_ptr<SCBonus>> B;

		void AddBonus(const std::shared_ptr<SCBonus> &o)
		{
			B.push_back(o);
		}

		void Clear() 
		{
			B.clear();
		}

		void Simulate(float dt) 
		{	
			deltaTime = dt;
		}

		void Draw(std::vector<std::shared_ptr<SCVehicleObj>> & V)
		{

			glEnable(GL_BLEND);

			for (int i = 0; i < B.size(); ++i)
			{
				xVec3 vP(B[i]->Pos);

				for (int j = 0; j < V.size(); ++j) {

					if (V[j]->active)
					{
						xVec3 vT(V[j]->Pos);

						float d = (vP - vT).magnitude();

						if (d < 7.0f)
						{
							if (B[i]->Give(V[j], _audio))
							{
								Sparcles.Add(vP.x, vP.y, vP.z, 0, 15, 0, 40, 180, 180, 180);
								B.erase(B.begin() + i);
								--i;
								break;
							}
						}
					}
				}
			}

			for (int i = 0; i < B.size(); ) {
				
				if (B[i]->Simulate(deltaTime))
				{
					B.erase(B.begin() + i);
				}
				else {
					++i;
				}
			}

			for (int i = 0; i < B.size(); ) {
			
				if (B[i]->Draw())
				{
					B.erase(B.begin() + i);
				}
				else {

					++i;
				}
			}

			glDisable(GL_BLEND);
		}

	private:

        float deltaTime;
		std::shared_ptr<SCAudio> _audio;
	};
}
