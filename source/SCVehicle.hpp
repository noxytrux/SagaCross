#pragma once

#include "SCRenderer.h"
#include "SCBulletManager.h"

namespace sc {

	struct SCCollisionModel {

		float Pos[3];
		float R;
	};

	class SCVehicleObj {

	public:

		float Pos[3];
		float Rot;
		bool active;
		char live;

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

		static void MakeCollide(const std::shared_ptr<SCBulletManager> &Bullets)
		{

			for (int i = 0; i < all.size(); ++i) {
	
				all[i]->Collide(all, col, Bullets->bullets);
			}

			for (int i = 0; i < Bullets->bullets.size(); ++i) {

				for (int a = 0; a < col.size(); ++a)
				{

					SCCollisionModel * other = &col[a];

					xVec3 G(Bullets->bullets[i]->Pos);
					xVec3 P(other->Pos);

					float L = (P - G).magnitude();

					if (L < (other->R + 4.0f))
					{
						Bullets->bullets[i]->Destroy();

						break;
					}
				}
			}
		}

		SCVehicle(const std::shared_ptr<SCVehicleObj> &o, const std::shared_ptr<SCOpenGLRenderable> &renderer) : SCRenderObj(renderer)
		{

			all.push_back(o);
			obj = o;
		}

		~SCVehicle() noexcept
		{
			for (int i = 0; i < all.size(); ++i) {
				
				if (all[i] == obj)
				{

					all.erase(all.begin() + i);
					return;
				}
			}
		}

		static std::shared_ptr<SCVehicleObj> GetRandom() {

			return all[rand() % all.size()];
		}

		static void Damage(xVec3 P) {

			for (int i = 0; i < all.size(); ++i) {

				if (all[i]->active && all[i]->live > 0)
				{

					xVec3 p(all[i]->Pos);

					float distance = (p - P).magnitude();

					if (distance < 10) {

						all[i]->Damage((int)(50 - distance * 5));

					}
				}
			}
		}

		virtual void Render() override
		{
			obj->Render();
		}

		virtual void Simulate(float dt) override
		{

			obj->Steer(this, dt);

		}

		virtual void DrawStatus(float dt)
		{
			obj->DrawStatus(dt);
		}

	};
}