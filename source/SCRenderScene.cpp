#include "SCRenderScene.hpp"
#include "glTextureLoader.hpp"

#include "SCBulletManager.h"
#include "SCParticleLayer.h"
#include "SCMeshInstanceManager.h"
#include "SCVegetation.h"
#include "SCCamera.h"
#include "SCGround.h"
#include "glMatrix4x4.hpp"

using namespace sc;

SCObjectRenderer RenderManager;
SCObjectRenderer* SCRenderObj::manager = &RenderManager;

SCSparclesManager

sc::Sparcles,
sc::LightSparcles,
sc::ParticleSmoke(64 , 0.0f , 14 , 6, 0 , 3 ),
sc::TankSmoke(255, 0.0f, 14, 0, 0, 0),
sc::TreeParticles,
sc::HouseParticles,
sc::TankSmokeWater(255, 0.0f, 14, 0, 0, 0),
sc::TankSmokeGrass(255, 0.0f, 14, 0, 0, 0);

std::vector<std::string> split(const std::string& s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		tokens.push_back(token);
	}
	return tokens;
}

void BuildCollision(float x, float y, float z, float Radius, float sx, float sz) {

	if (Radius > 0) {

		SCCollisionModel Col;

		Col.Pos[0] = x;
		Col.Pos[1] = y;
		Col.Pos[2] = z;
		Col.R = Radius * std::max(sx, sz);

		SCVehicle::col.push_back(Col);
	}
}

SCRenderScene::SCRenderScene(const std::string &name,
	const std::string rootPath,
	const std::shared_ptr<SCRendererInterface> &renderer,
	const std::shared_ptr<SCAudio> &audio)
: SCBaseScene(name, rootPath, renderer, audio)
{

}

SCRenderScene::~SCRenderScene() noexcept {

}

SCSceneType SCRenderScene::Render() {

    timer->update();
    sec = timer->getElapsedSeconds();
	sec = clamp(sec, 0.0f, 0.33f);

    RenderManager.Simulate(sec);
    Bullets->Simulate(sec);
    Bonuses->Simulate(sec);
    SCVehicle::MakeCollide(Bullets);

    auto wsk = std::dynamic_pointer_cast<SCTank>(ai[0]->obj);

    if (wsk->tankDie) Timer -= sec;

    float & hit = wsk->hit;
    hit -= sec;
    mhit = hit;

    if (!wsk->live && Timer < 0)
    {
        wsk = std::dynamic_pointer_cast<SCTank>(ai[select]->obj);

        for (int i = 0; i < ai.size(); i++) {

            if (wsk->live) {

                camera->LookAt = wsk -> Pos;
                Timer = 5;
                break;
            }
            else {

                select = ( select + 1 ) % ai.size();
                wsk = std::dynamic_pointer_cast<SCTank>(ai[select]->obj);
            }
        }
    }

    if (wsk) {

        xVec3 cPos = xVec3(wsk->Pos);
        Listener->Update( wsk->getListenerForward(), cPos );
    }
    else{

        Listener->Update( camera->gDir , camera->gEye );
    }

    //render all
    auto ctx = _renderer->getUIContext();
    auto display = _renderer->getDisplay();
    auto screenSize = display->getScreenSize();

    //game
    glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(1);

    auto renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);
    _renderer->Projection.setPerspective(45.0, (float)screenSize.width / (float)screenSize.height, 0.1f, 6000.0f);
	renderer->SimpleShader->begin();

    camera->Apply();
    //camera->FreeCam(renderer->getDisplay());

    auto frustum = _renderer->getFrustum();

	frustum->modelview = _renderer->ModelView.getMatrix();
	frustum->projection = _renderer->Projection.getMatrix();

    frustum->calculateFrustum();

    RenderManager.Render();
    Bonuses->Draw(SCVehicle::all);
    Bullets->Render();

	wsk->DrawTray();

    renderer->ParticleShader->begin();

    glDepthMask(0);

    Sparcles.Render(sec);
    ParticleSmoke.Render(sec);
    TankSmoke.Render(sec);
    TankSmokeWater.Render(sec); 
    TankSmokeGrass.Render(sec);
    TreeParticles.Render(sec);
    HouseParticles.Render(sec);

    glBlendFunc(GL_ONE, GL_ONE);
    LightSparcles.Render(sec);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDepthMask(1);

    renderer->GuiShader->begin();

    Bullets->DrawExplodes();

    renderer->SimpleShader->begin();

    Bullets->DrawClouds(sec);

    renderer->GuiShader->begin();

    auto current = renderer->GuiShader;

    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(0);

    for (const auto &v : SCVehicle::all) {

        v->DrawStatus(sec);
    }

    int live = wsk -> live;

    static float mTimer = 0.0f;
    mTimer += (( 20 - live ) / 60.0f) * sec * 30.0f;

    if ( live == 0 ) {

        renderer->RectFill( 0, 0, screenSize.width , screenSize.height, 128, 128, 128 , 64);
    }
    else if( live < 10 )
    {
        renderer->RectFill( 0, 0, screenSize.width , screenSize.height, 128, 32, 32 , ( 64 + 32 * sin( mTimer ) ) * ( 10 - live ) * 0.1f );
    }

    if ( mhit > 0.0 )
    {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blood);
        glUniform1i(current->uniforms[UNI_TEX0],0);

        renderer->RectFillGradientH( 0, 0, screenSize.width, screenSize.height, 255, 255, 255 , mhit * 3.0 , 0, 0, 0, 0 );
    }

    {
        glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

        int active_count = 0;

        for (int i = 0; i < SCVehicle::all.size(); ++i) {

            auto veh = std::dynamic_pointer_cast<SCTank>(SCVehicle::all[i]);

            if (veh->active) {

                if (veh->ai) {

                    glBindTexture(GL_TEXTURE_2D, ico[4]);
                }
                else {

                    glBindTexture(GL_TEXTURE_2D, ico[i]);
                }

                active_count++;

            } else {

                glBindTexture(GL_TEXTURE_2D, ico[5]);
            }

            renderer->DebugBlit( screenSize.width / 2.0 - SCVehicle::all.size() * 16 + i * 32 , 0 , 32, 32 );
        }

        float ax, ay;

        for ( int i = 0; i < SCVehicle::all.size(); ++i ) {

            glBindTexture(GL_TEXTURE_2D, aiAvatar[i]);

            auto veh = std::dynamic_pointer_cast<SCTank>(SCVehicle::all[i]);

            if ( veh -> active ) {

                renderer->Get2DOGLPos( veh->Pos[ 0 ] , veh->Pos[ 1 ] + 12.0f, veh->Pos[ 2 ] , ax , ay );

                if (ax > screenSize.width) ax = screenSize.width - 32;
                if (ax < 64) ax = 64;
                if (ay > screenSize.height) ay = screenSize.height - 32;
                if (ay < 0) ay = 0;

                renderer->DebugBlit( ax - 64, ay - 4, 32, 32 );
            }
        }

        if ( live == 0 ) {

            if (!playerWinGame) {

                if (showLooserImage) {

                    glBindTexture(GL_TEXTURE_2D, looser);
                    renderer->DebugBlit( screenSize.width / 2.0 - 128 , 50 , 256, 256 );
                }

                if (!loseSound) {

                    loseSound = true;
					showLooserImage = true;

                    _audio->pauseMusic();
                    _audio->loadMusic("music/lose.mp3");
                    _audio->playMusic();
                }
            }

        } else if ( active_count == 1 ) {

            if (!loseSound) {

                if (showWinImage) {

                    glBindTexture(GL_TEXTURE_2D, win);
                    renderer->DebugBlit( screenSize.width / 2.0 - 256 , 0 , 512, 512 );
                }

                wsk -> ai = true;

                if (!playerWinGame) {

                    playerWinGame = true;
					showWinImage = true;

                    _audio->pauseMusic();
                    _audio->loadMusic("music/win.mp3");
                    _audio->playMusic();
                }

            }
        }

        glBindTexture(GL_TEXTURE_2D, bomb);

        for (int i = 0; i < wsk -> mines; ++i) {

            renderer->DebugBlit((screenSize.width + (i * 32)) - 230 - 96, screenSize.height - 34, 32, 32 );
        }
    }

	glDepthMask(1);

    int bonus = rand() % 1500;

    if ( bonus < 4 ) {

        auto healthBonus = std::make_shared<SCHeathPack>(_rootPath, mapRandomPoint, ground, MeshManager);

        Bonuses->AddBonus(healthBonus);

    }
    else if ( bonus == 4 ) {

        auto mineBonus = std::make_shared<SCMineBonus>(_rootPath, mapRandomPoint, ground, MeshManager);

        Bonuses->AddBonus(mineBonus);
    }

    //ui
    ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));

    if (nk_begin(ctx, "Render", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {

        auto fonts = _renderer->getFontList();

        //buttons
        nk_style_set_font(ctx, &fonts[1]->handle);

        //backbutton
        ctx->style.button.normal = nk_style_item_image(_backbtn);
        ctx->style.button.hover = nk_style_item_image(_backbtn);
        ctx->style.button.active = nk_style_item_image(_backbtn);

        nk_layout_row_static(ctx, _backsize, _backsize, 1);
        if (nk_button_label(ctx, "")) {

            _type = SceneTypeMenu;
        }

        //TODO: add here pads for mobile UI
    }
    nk_end(ctx);

    return _type;
}

void SCRenderScene::Init() {

    //clean up before loading game
    textureLoader.releaseTextures();

	_audio->pauseMusic();

    auto btnbackpath = _rootPath + "textures/menubtn.png";
    _btnbacktex = textureLoader.loadFile(btnbackpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _backbtn = nk_subimage_id(_btnbacktex, _backsize, _backsize, nk_rect(0, 0, _backsize, _backsize));

    std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer)->blank = textureLoader.loadFile(_rootPath + "textures/blank.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

	//get game info 
	auto settings = _renderer->getSettings();
	auto &defaults = settings->userDefaults;

	auto selectedmap = defaults.value<int>("map", 0);
	auto botscount = defaults.value<int>("botscount", 0);

	defaults.clear();

    std::string map = "maps/map" + std::to_string(selectedmap) + "/";
    std::string level = "level00" + std::to_string(selectedmap);

    SCVehicle::all.clear();
    SCVehicle::col.clear();

    Bonuses = std::make_shared<SCBonusManager>(_audio);
    Listener = std::make_shared<SCAudioListener>(_audio);
    camera = std::make_shared<SCCamera>(_renderer, 80, 60, 0 );
    MeshManager = std::make_shared<SCMeshInstance>(_renderer);
    ground = std::make_shared<SCGround>(std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer), MeshManager, _rootPath, map + level, selectedmap == 1);
    mapRandomPoint = std::make_shared<SCRandomMeshPoint>(_rootPath + map + level + "_spawn.gmf");
    mapParticleLayer = std::make_shared<SCParticleLayer>(_rootPath + map + level + "_layers.bin", ground);
    rocks = std::make_shared<SCBuilding>(std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer), MeshManager, _rootPath, map + level + "_objects.json");
    Bullets = std::make_shared<SCBulletManager>(ground, rocks, Bonuses, MeshManager, _audio, _rootPath);

    timer = getPlatformTimerInstance();

    select = 0;
    ai.clear();

    ico[0] = textureLoader.loadFile(_rootPath + "textures/tank-ico_blue.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ico[1] = textureLoader.loadFile(_rootPath + "textures/tank-ico_yellow.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ico[2] = textureLoader.loadFile(_rootPath + "textures/tank-ico_green.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ico[3] = textureLoader.loadFile(_rootPath + "textures/tank-ico_white.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ico[4] = textureLoader.loadFile(_rootPath + "textures/tank-ico_red.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ico[5] = textureLoader.loadFile(_rootPath + "textures/tank-ico_dead.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    looser = textureLoader.loadFile(_rootPath + "textures/Looser.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    win = textureLoader.loadFile(_rootPath + "textures/win.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    bomb = textureLoader.loadFile(_rootPath + "textures/bomb.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    blood = textureLoader.loadFile(_rootPath + "textures/hurt.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

    _audio->loadSound("sounds/pyr.wav");
    _audio->loadSound("sounds/shoot.wav");
    _audio->loadSound("sounds/whooo.wav");
    _audio->loadSound("sounds/kaboom.wav");
    _audio->loadSound("sounds/explode.wav");
    _audio->loadSound("sounds/metal.wav");
    _audio->loadSound("sounds/bonus.wav");
    _audio->loadSound("sounds/fire.wav");
    _audio->loadSound("sounds/tick.wav");
    _audio->loadSound("sounds/nice0.wav");
    _audio->loadSound("sounds/nice1.wav");
    _audio->loadSound("sounds/nice2.wav");
    _audio->loadSound("sounds/nice3.wav");
    _audio->loadSound("sounds/nice4.wav");
    _audio->loadSound("sounds/hit0.wav");
    _audio->loadSound("sounds/hit1.wav");
    _audio->loadSound("sounds/hit2.wav");
    _audio->loadSound("sounds/hit3.wav");
    _audio->loadSound("sounds/missile0.wav");
    _audio->loadSound("sounds/missile1.wav");
    _audio->loadSound("sounds/missile2.wav");
    _audio->loadSound("sounds/last_mine.wav");
    _audio->loadSound("sounds/no_mine.wav");
    SCAudio::SoundID ready = _audio->loadSound("sounds/ready0.wav");

    Sparcles.Init();
    Sparcles.texture = textureLoader.loadFile(_rootPath + "textures/particle0.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    Sparcles.p_size_min = 20;
    Sparcles.p_size_max = 20;
    Sparcles.ground = ground;
    Sparcles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    ParticleSmoke.Init();
    ParticleSmoke.texture = textureLoader.loadFile(_rootPath + "textures/particle1.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ParticleSmoke.p_size_min = 40;
    ParticleSmoke.p_size_max = 60;
    ParticleSmoke.ground = ground;
    ParticleSmoke.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    LightSparcles.Init();
    LightSparcles.texture = textureLoader.loadFile(_rootPath + "textures/particle2.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    LightSparcles.p_size_min = 12;
    LightSparcles.p_size_max = 14;
    LightSparcles.ground = ground;
    LightSparcles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmoke.Init();
    TankSmoke.texture = textureLoader.loadFile(_rootPath + "textures/fx_dust-cloud-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmoke.p_size_min = 15;
    TankSmoke.p_size_max = 150;
    TankSmoke.p_size_decrase = 50;
    TankSmoke.ground = ground;
    TankSmoke.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmokeWater.Init();
    TankSmokeWater.texture = textureLoader.loadFile(_rootPath + "textures/fx_water-splash-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmokeWater.p_size_min = 15;
    TankSmokeWater.p_size_max = 150;
    TankSmokeWater.p_size_decrase = 50;
    TankSmokeWater.rotated = false;
    TankSmokeWater.ground = ground;
    TankSmokeWater.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmokeGrass.Init();
    TankSmokeGrass.texture = textureLoader.loadFile(_rootPath + "textures/fx_grass-particles-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmokeGrass.p_size_min = 60;
    TankSmokeGrass.p_size_max = 60;
    TankSmokeGrass.p_size_decrase = 0;
    TankSmokeGrass.rotated = false;
    TankSmokeGrass.ground = ground;
    TankSmokeGrass.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TreeParticles.Init();
    TreeParticles.texture = textureLoader.loadFile(_rootPath + "textures/fx_tree-debris-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TreeParticles.p_size_min = 30;
    TreeParticles.p_size_max = 40;
    TreeParticles.ground = ground;
    TreeParticles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    HouseParticles.Init();
    HouseParticles.texture = textureLoader.loadFile(_rootPath + "textures/fx_debris-brics-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    HouseParticles.p_size_min = 110;
    HouseParticles.p_size_max = 130;
    HouseParticles.ground = ground;
    HouseParticles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    std::ifstream bf((_rootPath + "models/Bots.json").c_str());
    bf >> botsInfo;
    bf.close();

    srand(static_cast<uint32_t>(time(nullptr)));

    auto maxBotsCount = botscount + 1; //add player to the list
    std::vector<std::string> botsNames;

    for (const auto &item : botsInfo.items()) {

        botsNames.push_back(item.key());
    }

    auto renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    for (int i = 0; i < maxBotsCount; ++i) {

        auto rnd = mapRandomPoint->GetRandomPoint();

        auto info = i == 0 ? json::object() : botsInfo[botsNames[i-1]];

        auto tank = std::make_shared<SCTank>(info,
                                             _rootPath,
                                             ground,
                                             renderer,
                                             _audio,
                                             camera,
                                             mapParticleLayer,
                                             Bonuses,
                                             mapRandomPoint,
                                             MeshManager,
                                             Bullets,
                                             (i == 0 ? false : true),
                                             rnd.x, 0, rnd.z,
                                             0,
                                             TankTypeNormal,
                                             settings->hardAIEnabled());

        auto vehicle = std::make_shared<SCVehicle>(tank, renderer);

        ai.push_back(vehicle);

        auto avatar = i == 0 ? "textures/Statistics/gui_portrait-003_tiny.png" : "textures/Statistics/" + info["avatar"].get<std::string>() + "_tiny.png";

        aiAvatar[i] = textureLoader.loadFile(_rootPath + avatar, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    }

    showLooserImage = false;
    showWinImage = false;
    playerWinGame = false;
    loseSound = false;

    _audio->loadMusic("music/mapmusic_01.mp3");
    _audio->playMusic();


    for (int i = 0; i < 3; ++i) {

        auto mine = std::make_shared<SCMine>(nullptr,
                                             _rootPath,
                                             mapRandomPoint,
                                             ground,
                                             MeshManager);

        Bonuses->AddBonus(mine);
    }

    camera->LookAt = ai[0]->obj->Pos;

    select = 0;

    _audio->playSound(ready, xVec3(0.0), 1.0f);

    mhit = 0.0f;
    Timer = 5.0f;
}

void SCRenderScene::Destroy() {

    if (Bullets) Bullets->RemoveAllObjects();
    botsInfo.clear();

    if (Bonuses) Bonuses->Clear();

    Sparcles.Clear();
    ParticleSmoke.Clear();
    LightSparcles.Clear();
    TankSmoke.Clear();
    TankSmokeWater.Clear();
    TankSmokeGrass.Clear();
    TreeParticles.Clear();
    HouseParticles.Clear();

    SCVehicle::all.clear();
    SCVehicle::col.clear();

    _audio->pauseMusic();
    _audio->unloadSounds();

    _btnbacktex = 0;
    
	//clean up after game
	textureLoader.releaseTextures();

    _type = SceneTypeNone;
}

void SCRenderScene::handleScroll(double s) {
    // ...
}

void SCRenderScene::handleMouse(int button, int action, double x, double y) {

    if (ai.empty()) {
        return;
    }

    auto tank = std::dynamic_pointer_cast<SCTank>(ai[0]->obj);

    if (!tank) {
        return;
    }

    if (!tank->live) {

        if (button == 0 && action == 1) {

            changePlayer();
        }

        return;
    }

    if (button == 0 && action == 1) {

        Shoot();
    }

    if (button == 1 && action == 1) {

        dropMine();
    }
}

void SCRenderScene::handleMove(const xVec2 &direction, const float angle) {

    auto tank = std::dynamic_pointer_cast<SCTank>(ai[0]->obj);

    if (!tank) {
        return;
    }

    if (!tank->live) {
        return;
    }

    tank->Velocity = direction;
    tank->currAngle = angle;
}

void SCRenderScene::dropMine() {

    auto tank = std::dynamic_pointer_cast<SCTank>(ai[0]->obj);
    tank->handleMineDrop();
}

void SCRenderScene::Shoot() {

    auto tank = std::dynamic_pointer_cast<SCTank>(ai[0]->obj);
    tank->Shoot();
}

void SCRenderScene::changePlayer() {

    select = ( select + 1 ) % ai.size();

    auto wsk = std::dynamic_pointer_cast<SCTank>(ai[ select ]->obj);

    if ( !wsk -> live ) {

        for (int i = 0; i < ai.size(); i++) {

            if (wsk->live) {
                camera->LookAt = wsk -> Pos;
                break;
            }
            else{

                select = ( select + 1 ) % ai.size();
                wsk = std::dynamic_pointer_cast<SCTank>(ai[ select ]->obj);
            }
        }
    }

    if (wsk) {

        xVec3 cPos = xVec3(wsk->Pos);
        Listener->Update( wsk->getListenerForward(), cPos );
    }
    else{

        Listener->Update( camera->gDir , camera->gEye );
    }
}


//TODO: enable this later on (multiplayer)
/*
void emoticon(uint32_t ID, uint32_t emID);
{
	auto tank = std::dynamic_pointer_cast<SCTank>(ai[ ID ]->obj);
	tank->emoticon(emID);
}
*/
