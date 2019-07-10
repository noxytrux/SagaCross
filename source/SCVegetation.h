#pragma once

#include "SCMeshInstanceManager.h"
#include "SCVehicle.hpp"

extern void BuildCollision(float x, float y, float z, float Radius, float sx, float sz);

namespace sc {

	class SCModel {
	public:

		xMat34 matrix;

		std::shared_ptr<SCFastMesh> mesh;
		std::shared_ptr<SCFastMesh> destroyedMesh;

		GLuint tex;
		xVec3 Scale;

		float time;
		float speed;
		float random;
		float amplitude;

		bool destructible;
		bool destroyed;
		bool animated;
		bool waterRendering;

		SCModel() 
			: mesh(nullptr) 
			, destroyedMesh(nullptr)
			, tex(0)
			, Scale(1.0f)
			, time(0)
			, speed(2.0)
			, random((rand() % 64) / 10.0)
			, amplitude(0.02)
			, destructible(false)
			, destroyed(false)
			, animated(false)
			, waterRendering(false)
		{			
		}

		~SCModel() noexcept = default;

		virtual void Reset() {

			destroyed = false;
			time = 0;
		}

		virtual void Draw() {

			glBindTexture(GL_TEXTURE_2D, tex);

			if (destructible && destroyed) {

				if (animated) {

					auto fa = std::dynamic_pointer_cast<SCFastAnimatedMesh>(destroyedMesh);

					fa->time = 0;
					fa->speed = 0;
					fa->amplitude = 0;
					fa->Draw();
				}
				else {
				
					destroyedMesh->Draw();
				}
			}
			else {

				if (animated) {

					auto fa = std::dynamic_pointer_cast<SCFastAnimatedMesh>(mesh);

					fa->time = time;
					fa->speed = speed;
					fa->amplitude = amplitude;
					fa->Draw();
				}
				else {
			
					mesh->Draw();
				}
			}
		}

		virtual void Update(float & dt) {

			time = dt + random;
			
			if (time > 9000.0f) { 
				
				time = 0.0f; 
			}

			mesh->setUserMatrix(matrix);
			mesh->setScale(Scale);

			if (destroyedMesh) {

				destroyedMesh->setUserMatrix(matrix);
				destroyedMesh->setScale(Scale);
			}
		}
	};

	class SCTreeModel : public SCModel {
	public:

		SCTreeModel()
			: SCModel() 
			, finishedDestruction(false)
			, startDestruction(false)
			, fallingPosY(0.0f)
			, fallingAngle(0.0f)
		{
				treeFall.id();
				treeRotation.id();
		}

		virtual void Reset() override {
			
			SCModel::Reset();

			fallingAngle = 0;
			treeFall.id();
			treeRotation.id();
			finishedDestruction = false;
			fallingPosY = 0.0f;
			startDestruction = false;
		}

		virtual void Update(float & dt) override {
			
			SCModel::Update(dt);

			if (destroyed && !startDestruction) {

				startDestruction = true;

				for (int i = 0; i < 20; ++i)
				{

					float dx = (rand() % 10 - 5) * 0.5;
					float dz = (rand() % 10 - 5) * 0.5;

					float dy = rand() % 3 * 10;

					TreeParticles.Add(matrix.t.x, matrix.t.y + 4, matrix.t.z, dx * 2, dy, dz * 2, 6, 255, 255, 255, 2 + rand() % 2);
				}
			}

			if (startDestruction && !finishedDestruction) {
				
				fallingPosY += (dt * 7.0);

				treeFall.t -= xVec3(0, 0, dt * 7.0);

				if (fallingPosY > (3.0 * Scale.z)) {

					fallingAngle += dt * 3.0;

					treeRotation.M.rotY(fallingAngle);

					if (fallingAngle > M_PI_2) {

						finishedDestruction = true;
					}
				}

				mesh->setUserMatrix(matrix * treeFall * treeRotation);
			}
		}

	private:

		bool finishedDestruction;
		bool startDestruction;

		float fallingPosY;
		float fallingAngle;

		xMat34 treeFall;
		xMat34 treeRotation;
	};

	class SCHouseModel : public SCModel {
	public:

		SCHouseModel() : SCModel(), startDestruction(false)
		{

		}

		virtual void Reset() override {
			SCModel::Reset();
			startDestruction = false;
		}

		virtual void Update(float & dt) override {
			SCModel::Update(dt);

			if (destroyed && !startDestruction) {

				startDestruction = true;

				for (int i = 0; i < 10; ++i)
				{

					float dx = (rand() % 10 - 5) * 0.5;
					float dz = (rand() % 10 - 5) * 0.5;

					float dy = rand() % 3 * 10;

					float px = (rand() % 6 - 3);
					float pz = (rand() % 6 - 3);

					HouseParticles.Add(matrix.t.x + px, matrix.t.y + 4, matrix.t.z + pz, dx * 2, dy, dz * 2, 4, 255, 255, 255, 2 + rand() % 2);
				}
			}
		}

	private:

		bool startDestruction;
	};

	class SCWaterModel : public SCModel {
	public:

		virtual void Draw() override {

			auto renderer = mesh->getRenderer();
			auto current = renderer->WaterShader;

			current->begin();
			
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glUniform1i(current->uniforms[UNI_TEX0], 0);
	
			auto ocean = std::dynamic_pointer_cast<SCFastOceanMesh>(mesh);

			ocean->time = time;
			ocean->Draw();

			renderer->SimpleShader->begin();
		}
	};

	struct pred
	{
		bool operator()(const std::shared_ptr<SCModel> &a, const std::shared_ptr<SCModel> &b) const
		{
			return a->animated > b->animated;
		}
	};

	class SCBuilding final : public SCRenderObj {

	private:

		xVec2 global_wind;
		float objectsDT;
		int animated_count;

	public:

		std::vector<std::shared_ptr<SCModel>> mainModelQueue;
		std::vector<std::shared_ptr<SCModel>> customModelQueue;

		~SCBuilding() noexcept {

			mainModelQueue.clear();
			customModelQueue.clear();
		}

		void ResetAll() {

			for (const auto &model : mainModelQueue) {

				model->Reset();
			}

			for (const auto &model : customModelQueue) {

				model->Reset();
			}
		}

		SCBuilding(const std::shared_ptr<SCOpenGLRenderable> &renderer, 
				   const std::shared_ptr<SCMeshInstance> &meshInstance,
				   const std::string &path,
				   const std::string &mapname) 
			: SCRenderObj(renderer)
		{
			global_wind = xVec2(3.0, 3.0);
			objectsDT = 0.0f;
			animated_count = 0;

			std::ifstream i((path + mapname).c_str());
			json dictObjects;
			i >> dictObjects;
            i.close();

			std::vector<json> objects = dictObjects.value<std::vector<json>>("objects", {});

			std::ifstream ii((path + "models/Prefabs.json").c_str());
			json prefabDict;
			ii >> prefabDict;
            ii.close();
	
			for (const auto &objDict : objects)
			{
				std::string modelPosition = objDict["position"];
				std::string modelRotation = objDict["rotation"];
				std::string modelScale = objDict["scale"];
				std::string modelType = objDict["type"];

                json objectInfo = prefabDict[modelType];

				std::string meshName = objectInfo["mesh"].get<std::string>();
				std::string textureName = objectInfo["texture"].get<std::string>();
				
				float Radius = objectInfo.value<float>("radius", 0.0f);

				std::vector<std::string> posArray = split(modelPosition, ','); 
				std::vector<std::string> quatArray = split(modelRotation, ','); 
				std::vector<std::string> scaleArray = split(modelScale, ','); 

				float x = std::stof(posArray[0]); 
				float y = std::stof(posArray[1]);
				float z = std::stof(posArray[2]); 

				float qx = std::stof(quatArray[0]); 
				float qy = std::stof(quatArray[1]); 
				float qz = std::stof(quatArray[2]);
				float qw = std::stof(quatArray[3]); 

				double sx = std::stod(scaleArray[0]);
				double sy = std::stod(scaleArray[1]);
				double sz = std::stod(scaleArray[2]);

				std::shared_ptr<SCModel> currModel = nullptr;

				bool updateIntoCustomQueue = false;

				if (meshName == "nature_tree-sprucesummer - A") {

					currModel = std::make_shared<SCTreeModel>();
				}
				else if (meshName == "object_building-rural - A") {

					currModel = std::make_shared<SCHouseModel>();
				}
				else if (meshName == "level002_mesh_sea-surface") {

					updateIntoCustomQueue = true;
					currModel = std::make_shared<SCWaterModel>();
					currModel->waterRendering = true;
				}
				else {

					currModel = std::make_shared<SCModel>();
				}

				currModel->Scale = xVec3(sx, sz, sy);

				float rx = x;
				float rz = (float)(512.0 - (z + 256.0)) - 256.0;
				float ry = y;

				xMat34 mainMat;
				mainMat.t = xVec3(rx, ry, rz);

				xQuat quat(xVec3(qx, -qy, qz), qw);
				xMat33 rotMat(quat);

				mainMat.M = rotMat;

				xMat34 rotation90Degree;
				rotation90Degree.t.zero();
				rotation90Degree.M.id();
				rotation90Degree.M.rotX(-M_PI_2);

				currModel->matrix = mainMat * rotation90Degree;

				auto texturePath = path + "textures/" + textureName + ".png";

				currModel->tex = textureLoader.loadFile(texturePath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false); 

				BuildCollision(rx, ry, rz, Radius, sx, sy);

				if (objectInfo.find("speed") != objectInfo.end()) {

					currModel->speed = objectInfo["speed"].get<float>();
				}

				if (objectInfo.find("amplitude") != objectInfo.end()) {

					currModel->amplitude = objectInfo["amplitude"].get<float>();
				}

				if (objectInfo.find("foliage") != objectInfo.end()) {

					currModel->animated = true;
					animated_count++;
				}

				if (objectInfo.find("destructible") != objectInfo.end()) {

					currModel->destructible = true;
					auto dstring = "models/" + meshName + "_destroyed";
                    currModel->destroyedMesh = meshInstance->GetMesh(dstring, path, currModel->animated ? SCMeshTypeAnimated : SCMeshTypeNormal);
				}

				SCMeshType type = SCMeshTypeNormal;

				if (currModel->animated) {
				
					type = SCMeshTypeAnimated;
				}
				else if (currModel->waterRendering) {
				
					type = SCMeshTypeOcean;
				}

				auto modelPath = "models/" + meshName;
				currModel->mesh = meshInstance->GetMesh(modelPath, path, type);

				if (updateIntoCustomQueue) {

					customModelQueue.push_back(currModel);
				}
				else {

					mainModelQueue.push_back(currModel);
				}

			}

			std::sort(mainModelQueue.begin(), mainModelQueue.end(), pred());
		}

		virtual void Render() override
		{
			glEnable(GL_BLEND);

			auto current = _renderer->FoliageShader;
			current->begin();

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);

			for (int i = 0; i < animated_count; ++i)
			{
				auto currModel = mainModelQueue[i];
				currModel->Draw();
			}

			current = _renderer->SimpleShader;
			current->begin();

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);
			glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

			for (int i = animated_count; i < mainModelQueue.size(); ++i)
			{
				auto currModel = mainModelQueue[i];
				currModel->Draw();
			}

			for (int i = 0; i < customModelQueue.size(); ++i)
			{
				auto currModel = customModelQueue[i];
				currModel->Draw();
			}

			glDisable(GL_BLEND);
		}

		virtual void Simulate(float dt) override
		{
			objectsDT += dt;

			for (const auto &model : mainModelQueue) {

				model->Update(objectsDT);
			}

			for (const auto &model : customModelQueue) {

				model->Update(objectsDT);
			}
		}
	};
}

