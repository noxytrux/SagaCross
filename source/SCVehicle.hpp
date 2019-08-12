#pragma once

#include "SCRenderer.h"

namespace sc {

	struct SCCollisionModel {

		float Pos[3];
		float R;
	};

    class SCBullet;
    class SCBulletManager;

	class SCVehicleObj {

	public:

		float Pos[3];
		float Rot;
		bool active;
		int live;

		GLubyte color[3];

		float Goal[3];

		float max_speed;

		bool isGoal;

		SCVehicleObj()
		{
			memset(Pos, 0, 12);
			Rot = 0;
			active = true;
			isGoal = false;
			max_speed = 1.2f;
		}

		virtual void Render() {}

		virtual void Steer(SCRenderObj *, float dt) {}

		virtual void Die() = 0;
		virtual bool Damage(int c) = 0;
		virtual void Score() = 0;

		virtual void Simulate(float dt) {}
		virtual void DrawStatus(float dt) {}
		virtual void Collide(std::vector<std::shared_ptr<SCVehicleObj>> &, std::vector<SCCollisionModel> &, std::vector<std::shared_ptr<SCBullet>> &) {}

	};

	class SCVehicle : public SCRenderObj {

	public:

		std::shared_ptr<SCVehicleObj> obj;

		static std::vector<std::shared_ptr<SCVehicleObj>> all;
		static std::vector<SCCollisionModel> col;

        static void MakeCollide(const std::shared_ptr<SCBulletManager> &Bullets);

        SCVehicle(const std::shared_ptr<SCVehicleObj> &o, const std::shared_ptr<SCOpenGLRenderable> &renderer);
        ~SCVehicle() noexcept;

        static std::shared_ptr<SCVehicleObj> GetRandom();
        static void Damage(xVec3 P);

        virtual void Render() override;
        virtual void Simulate(float dt) override;
        virtual void DrawStatus(float dt);

    };
}
