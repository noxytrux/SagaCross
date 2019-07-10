#include "SCRenderScene.hpp"
#include "glTextureLoader.hpp"

#include "SCBulletManager.h"
#include "SCParticleLayer.h"
#include "SCMeshInstanceManager.h"
#include "SCVegetation.h"
#include "SCCamera.h"
#include "SCGround.h"

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

    //set modelview and projection

    auto frustum = _renderer->getFrustum();

    frustum.modelview = _renderer->ModelView.m();
    frustum.projection = _renderer->Projection.m();

    frustum.calculateFrustum();

    //render all


    auto ctx = _renderer->getUIContext();
    auto display = _renderer->getDisplay();
    auto screenSize = display->getScreenSize();

    ctx->style.window.fixed_background = nk_style_item_color(nk_rgba(0, 0, 0, 0));

    if (nk_begin(ctx, "Render", nk_rect(0, 0, screenSize.width, screenSize.height), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {

        auto fonts = _renderer->getFontList();

        //buttons
        nk_style_set_font(ctx, &fonts[1]->handle);

        //backbutton
        ctx->style.button.normal = nk_style_item_image(_backbtn);
        ctx->style.button.hover = nk_style_item_image(_backbtn);
        ctx->style.button.active = nk_style_item_image(_backbtn);

        nk_layout_row_static(ctx, 32, 32, 1);
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

    auto btnbackpath = _rootPath + "textures/menubtn.png";
    _btnbacktex = textureLoader.loadFile(btnbackpath, GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    _backbtn = nk_subimage_id(_btnbacktex, 32, 32, nk_rect(0, 0, 32, 32));

    std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer)->blank = textureLoader.loadFile(_rootPath + "textures/blank.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);

	//get game info 
	auto settings = _renderer->getSettings();
	auto &defaults = settings->userDefaults;

	auto selectedmap = defaults.value<int>("map", 0);
	auto botscount = defaults.value<int>("botscount", 0);

	defaults.clear();

    std::string map = "maps/map" + std::to_string(selectedmap) + "/";
    std::string level = "level00" + std::to_string(selectedmap);

    Bonuses = std::make_shared<SCBonusManager>(_audio);
    Listener = std::make_shared<SCAudioListener>(_audio);
    camera = std::make_shared<SCCamera>(_renderer, 80, 60, 0 );
    MeshManager = std::make_shared<SCMeshInstance>(_renderer);
    ground = std::make_shared<SCGround>(std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer), MeshManager, _rootPath, map + level, selectedmap == 1);
    mapRandomPoint = std::make_shared<SCRandomMeshPoint>(_rootPath + map + level + "_spawn.gmf");
    mapParticleLayer = std::make_shared<SCParticleLayer>(_rootPath + map + level + "_layers.bin", ground);
    rocks = std::make_shared<SCBuilding>(std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer), MeshManager, _rootPath, map + level + "_objects.json");
    Bullets = std::make_shared<SCBulletManager>(ground, rocks, Bonuses, MeshManager, _audio, _rootPath);

    deltaTime = 0.0f;
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

    SCVehicle::all.clear();
    SCVehicle::col.clear();

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
    SCAudio::SoundID ready = _audio->loadSound("sounds/ready.wav");

    Sparcles.texture = textureLoader.loadFile(_rootPath + "textures/particle0.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    Sparcles.p_size_min = 20;
    Sparcles.p_size_max = 20;
    Sparcles.ground = ground;
    Sparcles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    ParticleSmoke.texture = textureLoader.loadFile(_rootPath + "textures/particle1.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    ParticleSmoke.p_size_min = 40;
    ParticleSmoke.p_size_max = 60;
    ParticleSmoke.ground = ground;
    ParticleSmoke.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    LightSparcles.texture = textureLoader.loadFile(_rootPath + "textures/particle2.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    LightSparcles.p_size_min = 12;
    LightSparcles.p_size_max = 14;
    LightSparcles.ground = ground;
    LightSparcles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmoke.texture = textureLoader.loadFile(_rootPath + "textures/fx_dust-cloud-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmoke.p_size_min = 15;
    TankSmoke.p_size_max = 150;
    TankSmoke.p_size_decrase = 50;
    TankSmoke.ground = ground;
    TankSmoke.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmokeWater.texture = textureLoader.loadFile(_rootPath + "textures/fx_water-splash-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmokeWater.p_size_min = 15;
    TankSmokeWater.p_size_max = 150;
    TankSmokeWater.p_size_decrase = 50;
    TankSmokeWater.rotated = false;
    TankSmokeWater.ground = ground;
    TankSmokeWater.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TankSmokeGrass.texture = textureLoader.loadFile(_rootPath + "textures/fx_grass-particles-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TankSmokeGrass.p_size_min = 60;
    TankSmokeGrass.p_size_max = 60;
    TankSmokeGrass.p_size_decrase = 0;
    TankSmokeGrass.rotated = false;
    TankSmokeGrass.ground = ground;
    TankSmokeGrass.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

    TreeParticles.texture = textureLoader.loadFile(_rootPath + "textures/fx_tree-debris-01.png", GL_LINEAR, 0, GL_CLAMP_TO_EDGE, false);
    TreeParticles.p_size_min = 30;
    TreeParticles.p_size_max = 40;
    TreeParticles.ground = ground;
    TreeParticles.renderer = std::dynamic_pointer_cast<SCOpenGLRenderable>(_renderer);

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
                                             MeshManager,
                                             renderer,
                                             _audio,
                                             camera,
                                             (i == 0 ? false : true),
                                             rnd.x, 0, rnd.z,
                                             ground,
                                             0,
                                             TankTypeNormal,
                                             settings->hardAIEnabled());

        auto vehicle = std::make_shared<SCVehicle>(tank, renderer);

        ai.push_back(vehicle);

        auto avatar = i == 0 ? "textures/Statistics/gui_portrait-003_tiny.png" : info["avatar"].get<std::string>() + "_tiny.png";

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
}

void SCRenderScene::Destroy() {

    Bullets->RemoveAllObjects();
    botsInfo.clear();

    Bonuses->Clear();

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
    tank->DropMine();
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


/*
- (void) handleEmoticonForPlayer:(int)ID forEmoticon:(int)emID
{
	Tank * mainplayer = (Tank*)ai[ID]->obj;
	mainplayer->emoticon(emID);
}



#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
	if (_gameLoaded){

		globalDT = MAX(self.timeSinceLastUpdate, 0.00001);

		[directionPointer Update:globalDT];
		[turretPointer Update:globalDT];

		RenderManager . Simulate();
		Bullets . Simulate();

		Vehicle :: MakeCollide();

		Tank *wsk = NULL;

		if(self.gameMode == STGameModeSinglePlayer){
			wsk = ((Tank*)ai[0]->obj );
		}
		else {
			wsk = ((Tank*)ai[USER_MULTI_ID]->obj);
		}

		if(wsk->tankDie) Timer -= globalDT;

		float & hit = wsk -> hit;
		hit -= globalDT;
		mhit = hit;

		if( !wsk -> live && Timer < 0 )
		{
			wsk = (Tank*)ai[ select ]->obj;

			for (int i = 0; i < _maxBotsCount; i++) {
				if(wsk->live){
					camera . LookAt = wsk -> Pos;
					Timer = 5;
					break;
				}
				else{
					select = ( select + 1 ) % _maxBotsCount;
					wsk = (Tank*)ai[ select ]->obj;
				}
			}
		}

		if(wsk){
			xVec3 cPos = xVec3(wsk->Pos);
			Listener -> Update( wsk->listenerForward , cPos );
		}
		else{
			Listener -> Update( camera.gDir , camera.gEye );
		}

	}
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{

	glViewport(0, 0, rect.size.width*RETINA_SCALE, rect.size.height*RETINA_SCALE);

	ScreenX = rect.size.width*RETINA_SCALE;
	ScreenY = rect.size.height*RETINA_SCALE;

	if(_gameLoaded) {

		if(self.summary){
			if (self.framesDisplayed % 30) {

				std::vector<Vehicle*> tanks;

				for(int i=0; i<_maxBotsCount; i++){
					tanks.push_back(ai[i]);
				}

				[self.summary updateStatistics:tanks
								  currentCount:_maxBotsCount
									  withInfo:self.botsInfo
									  withKeys:self.botsSortedKeys
							 isMultiplayerGame: (self.gameMode == STGameModeMultiPlayer)
							  playerAvatarList:self.gameAvatars
									playersIDS:playersAvatarsIDS];
			}
		}


		glClearColor( 107/255.0 , 130/255.0 , 146/255.0 , 1.0 );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer

		glEnable( GL_TEXTURE_2D );

		glDisable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );
		glDepthMask( 1 );
		MainMatrix3D->Projection = GLKMatrix4MakePerspective(( 45.0 * M_PI  ) / 180.0, rect.size.width/rect.size.height, 0.1f, 6000.0f);
		camera . Apply();

		viewport[0] = 0;
		viewport[1] = 0;
		viewport[2] = ScreenX;
		viewport[3] = ScreenY;

		Frustum.modelview = MainMatrix3D->ModelView.m; //podpiamy macierze do ucianania widoku
		Frustum.projection = MainMatrix3D->Projection.m;

		Frustum.calculateFrustum();

		glUseProgram(MainMatrix3D->SimpleShader->ShaderProgram);

		// - - - - - - - - -

		RenderManager . Render();

		Bonuses . Draw( Vehicle :: all );

		Bullets . Render();


		Tank *wsk = NULL;

		if(self.gameMode == STGameModeSinglePlayer){
			wsk = ((Tank*) ai[0]->obj);
		}
		else {
			wsk = ((Tank*)ai[USER_MULTI_ID]->obj);
		}


		wsk->DrawTray();

		glUseProgram(MainMatrix3D->ParticleShader->ShaderProgram);
		glDepthMask( 0 );

		//SparclesManager :: Start();

		Sparcles . Render();
		ParticleSmoke . Render();
		TankSmoke . Render();
		TankSmokeWater . Render();
		TankSmokeGrass . Render();
		TreeParticles . Render();
		HouseParticles . Render();

		glBlendFunc( GL_ONE, GL_ONE );
		LightSparcles . Render();
		glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );

		//SparclesManager :: End();

		glDepthMask( 1 );

		// - - - - - - --

		glUseProgram(MainMatrix3D->guiShader->ShaderProgram);

		Bullets . DrawExplodes();

		glUseProgram(MainMatrix3D->SimpleShader->ShaderProgram);

		Bullets . DrawClouds();

		glUseProgram(MainMatrix3D->guiShader->ShaderProgram);
		cShader *current = MainMatrix3D->guiShader;

		glEnable( GL_BLEND );
		glDisable( GL_DEPTH_TEST );
		glDepthMask( 0 );
		glEnable(GL_TEXTURE_2D);


		for( int i = 0; i < Vehicle :: all . size(); ++i )
			Vehicle :: all[ i ] -> DrawStatus();


		int live = wsk -> live ;

		static float mTimer = 0.0f;
		mTimer += (( 20 - live ) / 60.0f) * globalDT * 30.0f;

		if( live == 0 )
			RectFill( 0, 0, ScreenX , ScreenY , 128, 128, 128 , 64 );
		else if( live < 10 )
		{
			RectFill( 0, 0, ScreenX , ScreenY , 128, 32, 32 , ( 64 + 32 * sin( mTimer ) ) * ( 10 - live ) * 0.1f );
		}

		if( mhit > 0.0 )
		{

			glActiveTexture(GL_TEXTURE0);
			blood->Bind();
			glUniform1i(current->uniforms[UNI_TEX0],0);

			RectFillGradientH( 0 , 0 , ScreenX , ScreenY , 255, 255, 255 , mhit*3.0 , 0, 0, 0 , 0 );
		}

		{

			//draw Vehicles poisition
			glUniform4f(current->uniforms[UNI_TEX1], 1.0f, 1.0f, 1.0f, 1.0f);

			int active_count = 0;

			for( int i = 0; i < Vehicle :: all . size(); ++i ){

				Tank *veh = (Tank*)Vehicle :: all[ i ];

				if( veh -> active ){

					if (veh->ai) {
						ico[4] -> Bind();
					}
					else{
						ico[i] -> Bind();
					}

					active_count ++;
				} else {
					ico[5] -> Bind();
				}

				DebugBlit( ScreenX/2 - Vehicle :: all . size() * 16 + i * 32 , 0 , 32, 32 );
			}

			float ax, ay;

			for( int i = 0; i < Vehicle :: all . size(); ++i ){

				aiAvatar[i]->Bind();

				Tank *veh = (Tank*)Vehicle :: all[ i ];

				if( veh -> active ){

					Get2DOGLPos( veh->Pos[ 0 ] , veh->Pos[ 1 ] + 12.0f, veh->Pos[ 2 ] , ax , ay );

					if(ax > ScreenX) ax = ScreenX-32;
					if(ax < 64) ax = 64;
					if(ay > ScreenY) ay = ScreenY-32;
					if(ay < 0) ay = 0;

					DebugBlit( ax - 64, ay - 4, 32, 32 );
					nicks[i].hidden = NO;
					nicks[i].frame = CGRectMake((ax-32)/RETINA_SCALE + 5, (ay + 10)/RETINA_SCALE, 100/RETINA_SCALE, 16/RETINA_SCALE);
				}
				else{
					nicks[i].hidden = YES;
				}
			}


			if ((wsk->shootsPerformed == 0) && (wsk->score > 0) && (wsk->minesUsed > 0)) {
				[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber14
															percentage:100.0];
			}

			if( live == 0 ){

				if(!playerWinGame){
					directionPointer.alpha = 0.0f;
					turretPointer.alpha = 0.0f;
					mineButton.alpha = 0.0f;
					mineImage.alpha = 0.0f;

					againButton.hidden = NO;

					if(showLooserImage){
						looser -> Bind();
						DebugBlit( ScreenX/2 - 128 , 50 , 256, 256 );
					}

					if(!loseSound){
						loseSound = YES;

						self.changePlayerTap.enabled = YES;

						if (kTicTacToeValue < 3) {
							kTicTacToeValue = 0;
						}

						kLoseGamesValue++;

						float tierOneLose = ((float)kLoseGamesValue / 10.0) * 100.0;
						float tierTwoLose = ((float)kLoseGamesValue / 100.0 ) * 100.0;

						[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber4
																	percentage:tierOneLose];

						[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber12
																	percentage:tierTwoLose];

						/////////////LEADERBOARDS///////////////////

						std::vector<Vehicle*> tanks;

						for(int i=0; i<_maxBotsCount; i++){
							tanks.push_back(ai[i]);
						}

						std::sort(tanks.begin(), tanks.end(), ScoreSortForLeaderBoards());

						int64_t score = ((Tank*)ai[0]->obj)->overfallScore;

						if(self.gameMode == STGameModeMultiPlayer){
							[[STGCHelper sharedInstance] reportCurrentScore:score];
						}

						/////////////LEADERBOARDS///////////////////



						NSString *filePath = [[NSBundle mainBundle] pathForResource:@"lose" ofType:@"mp3"];
						BOOL paused = [APPDELEGATE.musicTrack paused];
						[APPDELEGATE.musicTrack playFile:filePath loops:-1];
						[APPDELEGATE.musicTrack setPaused:paused];

						if(self.gameMode==STGameModeMultiPlayer){

							for (int i=0; i<6; i++) {
								_emoticons[i].alpha = 0.0f;
							}

							againButton.alpha = 0.0f;
							backButton.alpha = 0.0f;

							//jezeli jestes serwerem to ci sie to nie pokaze!!!

							if(USER_MULTI_ID == 0){
								double delayInSeconds = 1.0;
								dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
								dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
									 [self handleMultiTankDie:USER_MULTI_ID];
								});
							}
						}

						__block UIButton *weakButton = againButton;
						__block ViewController *weakSelf = self;

						double delayInSeconds = 3.0;
						dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
						dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
							if(weakButton.hidden == NO){
								showLooserImage = NO;

								weakSelf.summary = [[[STSummaryScreen alloc] initWithFrame:self.view.frame] autorelease];
								weakSelf.summary.delegate = self;
								[weakSelf.view addSubview:weakSelf.summary];
								[weakSelf.summary show];
							}
						});

					}

				}

			} else if( active_count == 1 ){
				if(!loseSound) {
					directionPointer.alpha = 0.0f;
					turretPointer.alpha = 0.0f;
					mineButton.alpha = 0.0f;
					mineImage.alpha = 0.0f;
					againButton.alpha = 1.0f;

					againButton.hidden = NO;

					if(showWinImage){
						win -> Bind();
						DebugBlit( ScreenX/2 - 256 , 0 , 512, 512 );
					}

					wsk -> ai = true;

					if (!playerWinGame) {
						playerWinGame = YES;

						self.changePlayerTap.enabled = YES;

						/////////////LEADERBOARDS///////////////////

						std::vector<Vehicle*> tanks;

						for(int i=0; i<_maxBotsCount; i++){
							tanks.push_back(ai[i]);
						}

						std::sort(tanks.begin(), tanks.end(), ScoreSortForLeaderBoards());

						int64_t score = ((Tank*)ai[0]->obj)->overfallScore;

						if(self.gameMode == STGameModeMultiPlayer){
							[[STGCHelper sharedInstance] reportCurrentScore:score];
						}

						/////////////LEADERBOARDS///////////////////


						kWinGamesValue++;
						kTicTacToeValue++;

						float tierOneWins = kWinGamesValue;
						float tierTwoWins = ((float)kWinGamesValue / 1000.0 ) * 100.0;

						[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber2
																	percentage:tierOneWins];

						[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber5
																	percentage:tierTwoWins];

						if(kTicTacToeValue >= 3) {
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber11
																		percentage:100.0];
						}

						self.endMatchDate = [NSDate date];

						NSTimeInterval gameTime = [self.endMatchDate timeIntervalSinceDate:self.startMatchDate];
						if (gameTime <= 180.0) {

							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber7
																		percentage:100.0];
						}


						if(self.isGameHard && (wsk->score >= 6) ){

							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber3
																		percentage:100.0];
						}

						if(wsk->live <= 5){
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber6
																		percentage:100.0];
						}

						if (wsk->shootsPerformed == 0) {
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber13
																		percentage:100.0];
						}

						if (wsk->medkidsTaken == 0) {
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber17
																		percentage:100.0];
						}

						if (wsk->hitsTakenFromOtherTanks == 0) {
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber8
																		percentage:100.0];
						}

						std::sort(tanks.begin(), tanks.end(), DamageSortForAchievement());

						//simply if we are the last overalldamage tank
						if (wsk == ((Tank*)ai[ tanks.size() - 1 ]->obj)) {
							[[STAchievementManager sharedInstance] passAchievement:STAchievementNumber9
																		percentage:100.0];
						}

						NSString *filePath = [[NSBundle mainBundle] pathForResource:@"win" ofType:@"mp3"];
						BOOL paused = [APPDELEGATE.musicTrack paused];
						[APPDELEGATE.musicTrack playFile:filePath loops:-1];
						[APPDELEGATE.musicTrack setPaused:paused];

						if(self.gameMode==STGameModeMultiPlayer){
							for (int i=0; i<6; i++) {
								_emoticons[i].alpha = 0.0f;
							}

							backButton.alpha = 0.0f;

						}

						__block UIButton *weakButton = againButton;
						__block ViewController *weakSelf = self;

						double delayInSeconds = 3.0;
						dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
						dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
							if(weakButton.hidden == NO){
								showWinImage = NO;

								weakSelf.summary = [[[STSummaryScreen alloc] initWithFrame:self.view.frame] autorelease];
								weakSelf.summary.delegate = self;
								[weakSelf.view addSubview:weakSelf.summary];
								[weakSelf.summary show];
							}
						});
					}

				}
			}

			bomb -> Bind();

			for( int i = 0; i < wsk -> mines; ++i ){

				if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
					DebugBlit((ScreenX+(i * 32))-190-96, ScreenY - 34, 32, 32 );
				}
				else{
					DebugBlit((ScreenX+(i * 32))-230-96, ScreenY - 34, 32, 32 );
				}
			}

		}


		glDisable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );
		glDepthMask( 1 );
		glDisable(GL_TEXTURE_2D);

		// - - - - - - --

		if (self.gameMode == STGameModeSinglePlayer) {
			int bonus = rand()%1500;

			if( bonus < 4 )
			{
				Bonuses . AddBonus( new HeathPack() );
			} else if( bonus == 4 ){
				Bonuses . AddBonus( new MineBonus() );
			}
		}
		else if((self.gameMode == STGameModeMultiPlayer) && (USER_MULTI_ID == 0)) {

			int bonus = rand()%1500;

			if( bonus < 4 )
			{
				Bonus *bns = new HeathPack();

				STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;
				[ctr sendNetworkPacket:[STGCHelper sharedInstance].match
							  packetID:NETWORK_BONUS_DROP_HEALT
							  withData:&bns->Pos[0]
							  reserved:0
							  ofLength:sizeof(float) * 3
							  reliable:YES
								toUser:ctr.connectedPlayers];


				Bonuses . AddBonus( bns );
			} else if( bonus == 4 ){

				Bonus *bns = new MineBonus();

				STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;
				[ctr sendNetworkPacket:[STGCHelper sharedInstance].match
							  packetID:NETWORK_BONUS_DROP_MINE
							  withData:&bns->Pos[0]
							  reserved:0
							  ofLength:sizeof(float) * 3
							  reliable:YES
								toUser:ctr.connectedPlayers];

				Bonuses . AddBonus( bns );
			}

		}

		// - - - - - - --

	}
	else{
		glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear The Screen And The Depth Buffer
	}
}
*/
