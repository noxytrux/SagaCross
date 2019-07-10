#pragma once

#include "Core-pch.hpp"
#include "SCVehicle.hpp"

namespace sc {

	typedef SC_ENUM(uint32_t, SCGameTankType) {

		TankTypeNormal,
		TankTypeHeavy,
		TankTypeLight
	};

    class SCMeshInstance;
    class SCAudio;
    class SCCamera;
    class SCGround;
    class SCParticleLayer;
    class SCBonusManager;
    class SCRandomMeshPoint;
    class SCFastMesh;
    class SCFastCutOffMesh;

	class SCTank final : public SCVehicleObj, public std::enable_shared_from_this<SCTank> {

	public:

		xVec2 Velocity;
		float currAngle;

		virtual ~SCTank() noexcept {

		}

        SCTank(const json &botsData,
               const std::string &p,
               const std::shared_ptr<SCGround> &g,
               const std::shared_ptr<SCOpenGLRenderable> &r,
               const std::shared_ptr<SCAudio> &a,
               const std::shared_ptr<SCCamera> &c,
               const std::shared_ptr<SCParticleLayer> &pl,
               const std::shared_ptr<SCBonusManager> &bm,
               const std::shared_ptr<SCRandomMeshPoint> &rmp,
               const std::shared_ptr<SCMeshInstance> &mi,
               const std::shared_ptr<SCBulletManager> bum,
               bool ai,
               float x, float y, float z,
               int peerID = 0, SCGameTankType TankModel = TankTypeNormal, bool isAIHard = true);

        void ResetTank();
        void emoticon(int i);

        virtual void DrawStatus(float dt) override;
        void Explode();

        virtual void Die() override;
        virtual bool Damage(int c) override;
        virtual void Score() override;
        virtual void Render() override;
        virtual void DrawTray();
        virtual void Steer(SCRenderObj * o, float dt) override;
        virtual void Simulate(float dt) override;
        void Input();
        void handleMineDrop();
        void Shoot();
        virtual void Collide(std::vector<std::shared_ptr<SCVehicleObj>> &rest, std::vector<SCCollisionModel> &col, std::vector<std::shared_ptr<SCBullet>> &bullets) override;
        void DropMine();

        xVec3& getListenerForward() { return listenerForward; }

	public:

		int mines;
        float hit;
        bool tankDie;
        bool ai;

	private:

		bool HARD_AI;
		std::string path;
		float emoTime;
		float acc, turn_acc;

		std::shared_ptr<SCGround> ground;
		std::shared_ptr<SCOpenGLRenderable> renderer;
		std::shared_ptr<SCAudio> audio;
		std::shared_ptr<SCCamera> camera;
		std::shared_ptr<SCParticleLayer> particleLayer;
		std::shared_ptr<SCBonusManager> bonusManager;
		std::shared_ptr<SCRandomMeshPoint> randomMeshPoint;
		std::shared_ptr<SCMeshInstance> meshInstance;
		std::shared_ptr<SCBulletManager> bulletManager;

		float rifle_rot;
		float rifle_angle;

		float reload_time;
		float bullet_speed;

		float turn_to;
		float acc_to;
		float timer;

		int goal_count;
		bool render_info;
		bool isSterable;
		int traceNum;
		float tracePoints[100 * 3];

		float explode;
		xMat33 internalRotation;

		SCVehicleObj *follow;

		GLuint skull = 0;
		GLuint star = 0;
		GLuint body = 0;
		GLuint aimTex = 0;
		GLuint body_destroyed = 0;
		GLuint muzzle_flash = 0;
		GLuint tankHitTex = 0;
		GLuint blank = 0;

		GLuint emots[6];
		int emotID;

		float aimPos[3];
		float aimAcc[3];

		std::shared_ptr<SCFastMesh> suspension;
		std::shared_ptr<SCFastMesh> chasis;
		std::shared_ptr<SCFastMesh> turret;
		std::shared_ptr<SCFastMesh> gun;
		std::shared_ptr<SCFastMesh> muzzle_flash_mesh;

		std::shared_ptr<SCFastCutOffMesh> tankHitMesh;
		std::shared_ptr<SCFastCutOffMesh> tankExplosionA;
		std::shared_ptr<SCFastCutOffMesh> tankExplosionB;

		GLuint textureExpGround;
		std::shared_ptr<SCFastMesh> explosionFirst;

		float deltaTime;
		float inTIME;
		float alpha;

		float muzzleSize;
		bool shooted;

		xVec3 hitPos;
		float hitSize;
		bool hited;

		bool destroyedAnim;
		float destoyedASize;
		float destoyedBSize;
		float destoyedPosA;
		float destoyedPosB;

		xVec3 listenerForward;

		char score;

		int overfallScore;

		float kMaxTurretSpeed;
		float kMaxChaseDistance;
		int kMaxShootChance;

		xVec3 lastExplosionPos;

		int pyr_sound;

		char state;
		float action_time;

		char rot_random;
		char rot_rifle;
		char rot_rifle_a;
	};
}
