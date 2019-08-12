#pragma once

#include "SCBaseScene.hpp"
#include "GameTimer.hpp"
#include "SCTank.h"

namespace sc {

    class SCBonusManager;
    class SCBulletManager;
    class SCMeshInstance;
    class SCAudioListener;
    class SCCamera;
    class SCGround;
    class SCRandomMeshPoint;
    class SCParticleLayer;
    class SCBuilding;

	class SCRenderScene final : public SCBaseScene {

	public:

		SCRenderScene(const std::string &name,
					  const std::string rootPath,
					  const std::shared_ptr<SCRendererInterface> &renderer,
					  const std::shared_ptr<SCAudio> &audio);
		
		virtual ~SCRenderScene() noexcept;

		virtual SCSceneType Render() override;

		virtual void Init() override;
		virtual void Destroy() override;

		virtual void handleScroll(double s) override;
		virtual void handleMouse(int button, int action, double x, double y) override;
		virtual void handleMove(const xVec2 &direction, const float angle) override;

        virtual bool isRenderingGame() override { return true; }

    private:

        void dropMine();
        void Shoot();
        void changePlayer();

	private:

        GLuint _btnbacktex;
        struct nk_image _backbtn;

        std::shared_ptr<SCBonusManager> Bonuses;
        std::shared_ptr<SCBulletManager> Bullets;
        std::shared_ptr<SCMeshInstance> MeshManager;
        std::shared_ptr<SCAudioListener> Listener;
        std::shared_ptr<SCCamera> camera;
        std::shared_ptr<SCGround> ground;
        std::shared_ptr<SCRandomMeshPoint> mapRandomPoint;
        std::shared_ptr<SCParticleLayer> mapParticleLayer;
        std::shared_ptr<SCBuilding> rocks;

        //misc
        std::shared_ptr<GameTimer> timer;

        uint32_t select;
        std::vector<std::shared_ptr<SCVehicle>> ai;

        GLuint ico[6];
        GLuint looser;
        GLuint win;
        GLuint bomb;
        GLuint blood;
        GLuint aiAvatar[16];

        bool showLooserImage;
        bool showWinImage;
        bool playerWinGame;
        bool loseSound;

        json botsInfo;

        constexpr static const float MAX_FRAME_TIME = 0.01;
        float accumulator;
        float sec;

        float Timer;
        float mhit;
	};
}
