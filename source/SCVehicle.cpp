#include "SCVehicle.hpp"
#include "SCBulletManager.h"

using namespace sc;

std::vector<std::shared_ptr<SCVehicleObj>> SCVehicle::all;
std::vector<SCCollisionModel> SCVehicle::col;

void SCVehicle::MakeCollide(const std::shared_ptr<SCBulletManager> &Bullets)
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

SCVehicle::SCVehicle(const std::shared_ptr<SCVehicleObj> &o, const std::shared_ptr<SCOpenGLRenderable> &renderer) : SCRenderObj(renderer)
{
    all.push_back(o);
    obj = o;
}

SCVehicle::~SCVehicle() noexcept
{
    for (int i = 0; i < all.size(); ++i) {

        if (all[i] == obj)
        {

            all.erase(all.begin() + i);
            return;
        }
    }
}

std::shared_ptr<SCVehicleObj> SCVehicle::GetRandom() {

    return all[rand() % all.size()];
}

void SCVehicle::Damage(xVec3 P) {

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

void SCVehicle::Render()
{
    obj->Render();
}

void SCVehicle::Simulate(float dt)
{
    obj->Steer(this, dt);
}

void SCVehicle::DrawStatus(float dt)
{
    obj->DrawStatus(dt);
}
