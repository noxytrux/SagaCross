#include "SCRenderScene.hpp"
#include "glTextureLoader.hpp"

#include "SCTank.h"
#include "SCBulletManager.h"
#include "SCParticleLayer.h"
#include "SCMeshInstanceManager.h"

using namespace sc;

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

SCRenderScene::~SCRenderScene() {

}

SCSceneType SCRenderScene::Render() {

	//auto frustum = getFrustum();

	//float mv[16];
	//float p[16];

	//ModelView.getColumnMajor44(mv);
	//Projection.getColumnMajor44(p);

	//frustum.modelview = mv;
	//frustum.projection = p;

	//frustum.calculateFrustum();

    return _type;
}

void SCRenderScene::Init() {

	//clean up before loading game
	textureLoader.releaseTextures();

	//get game info 
	auto settings = _renderer->getSettings();
	auto &defaults = settings->userDefaults;

	auto selectedmap = defaults.value<int>("map", 0);
	auto botscount = defaults.value<int>("botscount", 0);

	defaults.clear();


}

void SCRenderScene::Destroy() {

	//clean up after game
	textureLoader.releaseTextures();

    _type = SceneTypeNone;
}

void SCRenderScene::handleScroll(double s) {

}

void SCRenderScene::handleMouse(int button, int action, double x, double y) {
   
}

void SCRenderScene::handleMove(const xVec2 &direction, const float angle) {

}

/*
bool restart = false;
float globalDT = 0.001;
float mhit = 0.0;

BonusManager Bonuses;
BulletManager Bullets;
RendererClass RenderManager;
cMeshInstanceManager * MeshManager;

cSoundManager * SoundManager;
cListener * Listener;

SparclesManager

Sparcles,
LightSparcles,
ParticleSmoke( 64 , 0.0f , 14 , 6, 0 , 3 ),
TankSmoke(255, 0.0f, 14, 0, 0, 0),
TreeParticles,
HouseParticles,
TankSmokeWater(255, 0.0f, 14, 0, 0, 0),
TankSmokeGrass(255, 0.0f, 14, 0, 0, 0);


vector< VehicleObj * > Vehicle :: all;
vector< CollisonModel > Vehicle :: col;

//ACHIEVEMENTS SCORES ////////////////////

NSString * const kTicTacToeStringKey = @"kTicTacToeStringKey";
NSString * const kWinGamesValueStringKey = @"kWinGamesValueStringKey";
NSString * const kLoseGamesValueStringKey = @"kLoseGamesValueStringKey";

int modelStruct::housesDestroyed;
int modelStruct::treesDestroyed;

int kTicTacToeValue = 0;
int kWinGamesValue = 0;
int kLoseGamesValue = 0;

//////////////////////////////////////////

struct ScoreSortForLeaderBoards
{
	bool operator()(Vehicle const * a, Vehicle const * b) const
	{
		Tank *currA = ((Tank*)a->obj);
		Tank *currB = ((Tank*)b->obj);

		float aAcuracy = currA->hitsPerformed / MAX(currA->shootsPerformed, 50.0);
		float bAcuracy = currB->hitsPerformed / MAX(currB->shootsPerformed, 50.0);

		currA->overfallScore = (1000 * MAX(currA->live, 0)/60.0) + (currA->damageTaken) + (aAcuracy*100.0f) + currA->score * 50;
		currB->overfallScore = (1000 * MAX(currB->live, 0)/60.0) + (currB->damageTaken) + (bAcuracy*100.0f) + currB->score * 50;

		return currA->overfallScore > currB->overfallScore;
	}
};

struct DamageSortForAchievement
{
	bool operator()(Vehicle const * a, Vehicle const * b) const
	{
		return ((Tank*)a->obj)->damageTaken > ((Tank*)b->obj)->damageTaken;
	}
};

//=====================================================

RendererClass * RenderObj :: manager = & RenderManager;

RenderObj :: RenderObj ()
{

	memset( Pos, 0 , 12 );
	memset( Rot, 0 , 12 );

	manager -> AddObj( this );
	enable_shadows = false;

	vDir[ 0 ] = 0;
	vDir[ 1 ] = 1;
	vDir[ 2 ] = 0;

}

//=====================================================

RenderObj :: ~RenderObj ()
{

	manager -> DelObj( this );

}

//=====================================================

//LightSource Light[ 8 ];
Camera camera( 80, 60, 0 );
//Vehicle * tank_player;
Ground *ground;
//glSphereES *mainSphereES;
RandomMeshPoint *mapRandomPoint = NULL;
STParticleLayer *mapParticleLayer = NULL;

MatrixPool3D*   MainMatrix3D = nil;
LoadPNG*        TexMenager = nil;

Texture * blank = NULL;
CFrustum        Frustum;

VelocityPointer *directionPointer;
VelocityPointer *turretPointer;

inline float GetHeight( float x , float z )
{

	return ground -> GetHeight( x , z );

}

inline float * GetNormal( float x , float z )
{

	static float n[ 3 ];
	ground -> GetNormal( x , z , n );

	return n;

}

float RETINA_SCALE = 1.0;

@interface ViewController () <STInGameChatViewDelegate,STSummaryScreenDelegate>{
	float Timer;
	int select;
	Vehicle * ai[ 16 ];
	BOOL playerWinGame;
	BOOL loseSound;

	Building * rocks;

	Texture * ico[6];
	Texture * looser;
	Texture * win;
	Texture * bomb;
	Texture * blood;
	Texture * aiAvatar[16];

	UIButton *_emoticons[6];
	float gfxOffset;
	UIButton *mineButton;
	UIImageView *mineImage;
	UIButton *againButton;

	BOOL showLooserImage;
	BOOL showWinImage;

	UILabel *nicks[16];

	int playersAvatarsIDS[4];

	UIButton *chatButton;
	UIButton *boardButton;
}

@property (nonatomic, strong) NSMutableDictionary *botsInfo;
@property (nonatomic, strong) NSMutableArray *botsSortedKeys;

@property (strong, nonatomic) EAGLContext *context;
@property (nonatomic, strong) STSummaryScreen *summary;

@property (nonatomic, strong) STNotificationView *notificationView;

@property (nonatomic) BOOL isGameHard;

@property (nonatomic, strong) NSArray *gameAvatars;

@property (nonatomic, strong) STInGameChatView *inChatView;

@property (nonatomic, strong) NSDate *startMatchDate;
@property (nonatomic, strong) NSDate *endMatchDate;

@property (nonatomic, strong) UIImageView *exclamationMark;

@property (nonatomic) BOOL waterEnabled;

@property (nonatomic, strong) UITapGestureRecognizer *changePlayerTap;

- (void) setupGL;
- (void) tearDownGL;
- (void) fireTank;
- (void) delayRestart;
- (void) emoticonButtonPress:(UIButton*)sender;

@end

@implementation ViewController
@synthesize gameMode = _gameMode;
@synthesize currentMapName = _currentMapName;

- (void)dealloc
{
	self.changePlayerTap = nil;
	self.exclamationMark = nil;
	self.startMatchDate = nil;
	self.endMatchDate = nil;

	NSUserDefaults *defs = [NSUserDefaults standardUserDefaults];
	[defs setInteger:kTicTacToeValue forKey:kTicTacToeStringKey];
	[defs setInteger:kWinGamesValue forKey:kWinGamesValueStringKey];
	[defs setInteger:kLoseGamesValue forKey:kLoseGamesValueStringKey];
	[defs synchronize];

	self.inChatView = nil;
	self.gameAvatars = nil;

	[[NSNotificationCenter defaultCenter] removeObserver:self];

	self.currentMapName = nil;
	Bullets.RemoveAllObjects();

	for (int i=0; i<6; i++) {
		[_emoticons[i] release];
	}

	self.summary = nil;

	self.botsInfo = nil;
	self.botsSortedKeys = nil;

	self.notificationView = nil;

	for (int i=0; i<6; i++){
		delete ico[i];
	}

	delete looser;
	delete win;
	delete bomb;
	delete blood;
	delete mapRandomPoint;
	delete mapParticleLayer;

	for (int i=0; i<_maxBotsCount; i++) {
		[nicks[i] release];
	}

	for(int i=0; i<_maxBotsCount; i++){
		delete aiAvatar[i];
	}

	mapRandomPoint = NULL;
	mapParticleLayer = NULL;

	//RenderManager.Clear();

	[directionPointer release];
	[turretPointer release];

	[self tearDownGL];
	[MainMatrix3D release];

	delete blank;

	[TexMenager ReleaseTex];
	[TexMenager release];

	//delete tank_player;
	delete rocks;
	delete ground;

	for( int i = 0; i < _maxBotsCount; ++i )
		delete ai[ i ];

	Timer = 5;

	Bonuses . Clear();
	Sparcles . Clear();
	ParticleSmoke . Clear();
	LightSparcles . Clear();
	TankSmoke . Clear();
	TankSmokeWater . Clear();
	TankSmokeGrass . Clear();
	TreeParticles . Clear();
	HouseParticles . Clear();

	delete Sparcles.texture;
	delete ParticleSmoke.texture;
	delete LightSparcles.texture;
	delete TankSmoke.texture;
	delete TankSmokeGrass.texture;
	delete TankSmokeWater.texture;
	delete TreeParticles.texture;
	delete HouseParticles.texture;

	Vehicle::all.clear();
	Vehicle::col.clear();

	Bullets.RemoveAllObjects();

	delete MeshManager;
	//delete mainSphereES;
	delete Listener;
	delete SoundManager;

	if ([EAGLContext currentContext] == self.context) {
		[EAGLContext setCurrentContext:nil];
	}

	NSLog(@"DELETE MAIN GAME VIEW");

	[againButton removeFromSuperview];
	[againButton release];
	againButton = nil;

	[super dealloc];
}

- (BOOL)prefersStatusBarHidden
{
	return YES;
}

-(BOOL) shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)toInterfaceOrientation
{
	return UIInterfaceOrientationIsLandscape(toInterfaceOrientation);
}

- (BOOL) shouldAutorotate
{
	return YES;
}

- (NSUInteger) supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskLandscapeLeft | UIInterfaceOrientationMaskLandscapeRight;
}

- (UIInterfaceOrientation) preferredInterfaceOrientationForPresentation
{
	return UIInterfaceOrientationLandscapeLeft;
}

- (void) viewDidAppear:(BOOL)animated
{
	NSLog(@"DISBALE IDLE TIMER");
	[[UIApplication sharedApplication] setIdleTimerDisabled:YES];
//#if !(TARGET_IPHONE_SIMULATOR)
//    [GAI sharedInstance].optOut = YES;
//#endif
}

-(void) viewDidDisappear:(BOOL)animated
{
	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"Tank_saga_menu" ofType:@"mp3"];
	BOOL paused = [APPDELEGATE.musicTrack paused];
	[APPDELEGATE.musicTrack playFile:filePath loops:-1];
	[APPDELEGATE.musicTrack setPaused:paused];

	NSLog(@"ENABLE IDLE TIMER");
	[[UIApplication sharedApplication] setIdleTimerDisabled:NO];

	[SVProgressHUD dismiss];
//#if !(TARGET_IPHONE_SIMULATOR)
//    [GAI sharedInstance].optOut = NO;
//#endif
}

- (void)viewDidLoad
{
	[super viewDidLoad];

	NSUserDefaults *defs = [NSUserDefaults standardUserDefaults];

	kTicTacToeValue = [defs integerForKey:kTicTacToeStringKey];
	kWinGamesValue  = [defs integerForKey:kWinGamesValueStringKey];
	kLoseGamesValue = [defs integerForKey:kLoseGamesValueStringKey];

	NSString *path = [[NSBundle mainBundle]pathForResource:@"avatars" ofType:@"json"];
	NSData *data = [NSData dataWithContentsOfFile:path];

	NSDictionary *avatarDict = [NSJSONSerialization JSONObjectWithData:data
															   options:NSJSONReadingMutableContainers
																 error:nil];

	self.gameAvatars = [avatarDict objectForKey:@"avatars"];

	_gameLoaded = NO;
	self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
	self.summary = nil;

	if (!self.context) {
		NSLog(@"Failed to create ES context");
	}

	GLKView *view = (GLKView *)self.view;
	view.context = self.context;
	view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
	self.preferredFramesPerSecond = 30;

	UIScreen* mainscr = [UIScreen mainScreen];
	float w = mainscr.currentMode.size.width;
	float h = mainscr.currentMode.size.height;

	//if([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPad){
		if ((w == 1536 && h == 2048) || (w == 640 && (h == 960 || h == 1136)))
		{
			NSLog(@"DETECT: [RETINA]");
			RETINA_SCALE = 2.0;
		}
		else {
			NSLog(@"DETECT: [NORMAL]");
			RETINA_SCALE = 1.0;
		}
	//}

	self.view.contentScaleFactor = RETINA_SCALE;
	self.view.layer.contentsScale= RETINA_SCALE;

	ScreenX = h;
	ScreenY = w;

	NSLog(@"WHAT: %d %d",ScreenX,ScreenY);

	playerWinGame = NO;
	loseSound = NO;

	[self setupGL];

	blank = new Texture( "blank" );

	ico[0] = new Texture( "tank-ico_blue" );
	ico[1] = new Texture( "tank-ico_yellow" );
	ico[2] = new Texture( "tank-ico_green" );
	ico[3] = new Texture( "tank-ico_white" );
	ico[4] = new Texture( "tank-ico_red" );
	ico[5] = new Texture( "tank-ico_dead" );

	looser = new Texture( "Looser" );
	win = new Texture( "win" );
	bomb = new Texture( "bomb" );
	blood = new Texture( "hurt" );

	SoundManager = new cSoundManager();
	Listener = new cListener();
	MeshManager = new cMeshInstanceManager();

	// - - - - - - - - - -

	SoundManager -> GetSound( "pyr" , "pyr" );
	SoundManager -> GetSound( "shoot" , "shoot" );
	SoundManager -> GetSound( "whooo" , "whooo" );
	SoundManager -> GetSound( "kaboom" , "kaboom" );
	SoundManager -> GetSound( "explode" , "explode" );
	SoundManager -> GetSound( "metal" , "metal" );
	SoundManager -> GetSound( "bonus" , "bonus" );
	//SoundManager -> GetSound( "hearth" , "hearth" );
	SoundManager -> GetSound( "fire" , "fire" );
	SoundManager -> GetSound( "tick" , "tick" );

	SoundManager -> GetSound( "nice0" , "nice0" );
	SoundManager -> GetSound( "nice1" , "nice1" );
	SoundManager -> GetSound( "nice2" , "nice2" );
	SoundManager -> GetSound( "nice3" , "nice3" );
	SoundManager -> GetSound( "nice4" , "nice4" );

	SoundManager -> GetSound( "hit0" , "hit0" );
	SoundManager -> GetSound( "hit1" , "hit1" );
	SoundManager -> GetSound( "hit2" , "hit2" );
	SoundManager -> GetSound( "hit3" , "hit3" );
	SoundManager -> GetSound( "ready" , "ready0" );

	SoundManager -> GetSound( "missile0" , "missile0" );
	SoundManager -> GetSound( "missile1" , "missile1" );
	SoundManager -> GetSound( "missile2" , "missile2" );

	SoundManager -> GetSound( "last_mine" , "last_mine" );
	SoundManager -> GetSound( "no_mine" , "no_mine" );

	// - - - - - - - - - -

	Vehicle::all.clear();
	Vehicle::col.clear();

	rocks = NULL;

	Sparcles . texture = new Texture( "particle0",true);
	Sparcles . p_size_min = 20;
	Sparcles . p_size_max = 20;

	ParticleSmoke . texture = new Texture( "particle1",true);
	ParticleSmoke . p_size_min = 40;
	ParticleSmoke . p_size_max = 60;

	LightSparcles . texture = new Texture( "particle2",true);
	LightSparcles . p_size_min = 12;
	LightSparcles . p_size_max = 14;

	TankSmoke . texture = new Texture( "fx_dust-cloud-01",true);
	TankSmoke . p_size_min = 15;
	TankSmoke . p_size_max = 150;
	TankSmoke . p_size_decrase = 50;

	TankSmokeWater . texture = new Texture( "fx_water-splash-01",true);
	TankSmokeWater . p_size_min = 15;
	TankSmokeWater . p_size_max = 150;
	TankSmokeWater . p_size_decrase = 50;
	TankSmokeWater . rotated = false;

	TankSmokeGrass . texture = new Texture( "fx_grass-particles-01",true);
	TankSmokeGrass . p_size_min = 60;
	TankSmokeGrass . p_size_max = 60;
	TankSmokeGrass . p_size_decrase = 0;
	TankSmokeGrass . rotated = false;

	TreeParticles . texture = new Texture( "fx_tree-debris-01",true);
	TreeParticles . p_size_min = 30;
	TreeParticles . p_size_max = 40;

	HouseParticles . texture = new Texture( "fx_debris-brics-01",true);
	HouseParticles . p_size_min = 110;
	HouseParticles . p_size_max = 130;

	//mainSphereES = new glSphereES();

	showLooserImage = YES;
	showWinImage = YES;

//again:
	_maxBotsCount = 0;
	_multiPlayerOffset = 0;
	self.gameMode = STGameModeSinglePlayer;

	Bonuses . Clear();
	Sparcles . Clear();
	ParticleSmoke . Clear();
	LightSparcles . Clear();
	TankSmoke . Clear();
	TankSmokeGrass . Clear();
	TankSmokeWater . Clear();
	TreeParticles . Clear();
	HouseParticles . Clear();

	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		gfxOffset = 300;
	}
	else{
		gfxOffset = 400;
	}

	self.view.multipleTouchEnabled = YES;

	//365 × 197
	mineImage = [[[UIImageView alloc] initWithImage:[UIImage imageNamed:@"gui_secondary-fire-top.png"]] autorelease];
	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		mineImage.frame = CGRectMake(ScreenX-380 - 64, 768-128-10 - 64, 365, 197);
	}
	else{
		mineImage.frame = CGRectMake((IS_IPHONE_5 ? 568 : 480)-230-16, 320-64-10-16, 182, 98);
	}
	[self.view addSubview:mineImage];

	againButton = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
	[againButton addTarget:self action:@selector(fireTank) forControlEvents:UIControlEventTouchUpInside];
	[againButton setFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake((1024-300) * 0.5, 768-80, 300, 60) : CGRectMake(((IS_IPHONE_5 ? 568 : 480) - 200) * 0.5, 320-50, 200,40))];
	againButton.titleLabel.font = [UIFont fontWithName:@"Orbitron-Light" size:20];
	againButton.hidden = YES;
	[againButton setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
	[againButton setTitle: NSLocalizedString(@"AGAIN!", nil) forState:UIControlStateNormal];
	[againButton setDefault];
	[self.view addSubview:againButton];

	mineButton = [UIButton buttonWithType:UIButtonTypeCustom];
	[mineButton setImage:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? [UIImage imageNamed:@"bomb_btn@2x.png"] : [UIImage imageNamed:@"bomb_btn.png"]) forState:UIControlStateNormal];
	[mineButton setImage:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? [UIImage imageNamed:@"bomb_btn_down@2x.png"] : [UIImage imageNamed:@"bomb_btn_down.png"]) forState:UIControlStateHighlighted];

	[mineButton addTarget:self action:@selector(dropMine) forControlEvents:UIControlEventTouchUpInside];
	[mineButton setFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake(ScreenX-380, 768-128-10, 64, 64) : CGRectMake((IS_IPHONE_5 ? 568 : 480)-230, 320-64-10, 64, 64))];

	if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad) {
		mineButton.transform = CGAffineTransformMakeScale(2.0, 2.0);
	}

	directionPointer = [[VelocityPointer alloc] initWithFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ?
															   CGRectMake(0, 100, 512, 768 - 100) :
															   CGRectMake(0, 60, (IS_IPHONE_5 ? 284 : 240), 320-60)) isBlue:YES];

	[self.view addSubview:directionPointer];

	turretPointer = [[VelocityPointer alloc] initWithFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ?
															CGRectMake(512, 100, 512, 768 - 100) :
															CGRectMake((IS_IPHONE_5 ? 284 : 240), 60, (IS_IPHONE_5 ? 284 : 240), 320-60)) isBlue:NO];

	[self.view addSubview:turretPointer];

	[self.view addSubview:mineButton];

	backButton = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
	[backButton addTarget:self action:@selector(dismiss) forControlEvents:UIControlEventTouchUpInside];
	[backButton setImage:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? [UIImage imageNamed:@"menubtn@2x.png"] : [UIImage imageNamed:@"menubtn.png"]) forState:UIControlStateNormal];
	[backButton setFrame: UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake(0, 0, 64, 64) : CGRectMake(0, 0, 32, 32) ];

	[self.view addSubview:backButton];

	id names[6] = {@"smile.png", @"sad.png", @"evil.png", @"tongue.png", @"dead.png", @"werid.png"};

	for (int i=0; i<6; i++) {
		_emoticons[i] = [[UIButton buttonWithType:UIButtonTypeCustom] retain];
		_emoticons[i].alpha = 0.0f;
		[_emoticons[i] setImage:[UIImage imageNamed:names[i]] forState:UIControlStateNormal];
		_emoticons[i].tag = i;
		[_emoticons[i] addTarget:self action:@selector(emoticonButtonPress:) forControlEvents:UIControlEventTouchUpInside];
		[self.view addSubview:_emoticons[i]];

		if(i<3){
			_emoticons[i].frame = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake(20, 180 + (i%3)*80, 32, 32) : CGRectMake(0, 50 + (i%3)*50, 32, 32));
		}else{
			_emoticons[i].frame = (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake(1024-32 - 20, 180 + (i%3)*80, 32, 32) : CGRectMake((IS_IPHONE_5 ? 568 : 480)-32, 50 + (i%3)*50, 32, 32));
		}

		if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad){
			_emoticons[i].transform = CGAffineTransformMakeScale(2.0, 2.0);
		}
	}

	self.notificationView = [[[STNotificationView alloc] initWithFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ? CGRectMake(1024-96 - 5, 5, 96, 32) : CGRectMake((IS_IPHONE_5 ? 568 : 480) - 48 - 5, 5, 48, 16))] autorelease];

	[self.view addSubview:self.notificationView];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(handlePlayerKillInfoEvent:)
												 name:PLAYERKILLEDOTHERPLAYER_NOTIFICATION
											   object:nil];

	[[NSNotificationCenter defaultCenter] addObserver:self
											 selector:@selector(handlePlayerMineInfoEvent:)
												 name:PLAYERKILLEDBYOTHERPLAYERMINE_NOTIFICATION
											   object:nil];


	self.isGameHard = YES;

	NSString *avatarString = [defs objectForKey:@"AVATAR"];

	if(avatarString) {
		playersAvatarsIDS[0] = [avatarString integerValue];
	}
	else{
		playersAvatarsIDS[0] = 0;
	}

	playersAvatarsIDS[1] = 0;
	playersAvatarsIDS[2] = 0;
	playersAvatarsIDS[3] = 0;

	self.exclamationMark = [[[UIImageView alloc] initWithImage:[UIImage imageNamed:@"exlamation.png"]] autorelease];
	[self.exclamationMark setFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ?
									CGRectMake(40, 40, 32, 32) : CGRectMake(20, 22, 16, 16))];

	chatButton = [UIButton buttonWithType:UIButtonTypeCustom];
	[chatButton addTarget:self action:@selector(openChatView) forControlEvents:UIControlEventTouchUpInside];
	[chatButton setFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ?
						  CGRectMake(70, 0, 64, 64) : CGRectMake(40, 0, 32, 32))];
	[chatButton setImage:[UIImage imageNamed:@"chat_icon.png"]
				forState:UIControlStateNormal];

	[chatButton addSubview:self.exclamationMark];
	[self.view addSubview:chatButton];

	self.exclamationMark.alpha = 0.0f;

	boardButton = [UIButton buttonWithType:UIButtonTypeCustom];
	[boardButton addTarget:self action:@selector(boardsOpen) forControlEvents:UIControlEventTouchUpInside];
	[boardButton setFrame:(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad ?
						  CGRectMake(((1024)-128) * 0.5, 32, 128, 64) : CGRectMake(((IS_IPHONE_5 ? 568 : 480)-64) * 0.5, 16, 64, 32))];

	[boardButton setImage:[UIImage imageNamed:@"leaderboardingame.png"]
				forState:UIControlStateNormal];

	[self.view addSubview:boardButton];

	chatButton.hidden = YES;
	boardButton.alpha = 0.0f;

	Timer = 5;

	NSString *isCameraStatic = [defs objectForKey:@"STATICCAMERA"];

	if(isCameraStatic){
		if([isCameraStatic isEqualToString:@"YES"]){
			staticCamera = true;
		}
		else {
			staticCamera = false;
		}
	}
	else{
		staticCamera = false;
	}

	self.changePlayerTap = [[[UITapGestureRecognizer alloc] initWithTarget:self action:@selector(changePlayer)] autorelease];
	self.changePlayerTap.delegate = self;

	[self.view addGestureRecognizer:self.changePlayerTap];

	self.changePlayerTap.enabled = NO;
}

- (void) handlePlayerKillInfoEvent:(NSNotification*)notification {

	NSDictionary *object = [notification object];

	NSNumber *fromNumber = object[FROM_KEY];
	NSNumber *toNumber = object[TO_KEY];

	[self.notificationView populateKillFromPlayer:fromNumber
										 toPlayer:toNumber
										 botsInfo:self.botsInfo
										 withKeys:self.botsSortedKeys
											event:STEventKill
									isMultiplayer:(self.gameMode == STGameModeMultiPlayer)
								 playerAvatarList:self.gameAvatars
									   playersIDS:playersAvatarsIDS];

}

- (void) handlePlayerMineInfoEvent:(NSNotification*)notification {

	NSDictionary *object = [notification object];

	NSNumber *fromNumber = object[FROM_KEY];
	NSNumber *toNumber = object[TO_KEY];

	[self.notificationView populateKillFromPlayer:fromNumber
										 toPlayer:toNumber
										 botsInfo:self.botsInfo
										 withKeys:self.botsSortedKeys
											event:STEventMine
									isMultiplayer:(self.gameMode == STGameModeMultiPlayer)
								 playerAvatarList:self.gameAvatars
									   playersIDS:playersAvatarsIDS];
}

- (void) emoticonButtonPress:(UIButton*)sender
{
	if(self.gameMode == STGameModeSinglePlayer){
		Tank * mainplayer = (Tank*)ai[0]->obj;
		mainplayer->emoticon(sender.tag);

	}
	else{
		Tank * mainplayer = (Tank*)ai[USER_MULTI_ID]->obj;
		mainplayer->emoticon(sender.tag);
	}
}

- (void) handleEmoticonForPlayer:(int)ID forEmoticon:(int)emID
{
	Tank * mainplayer = (Tank*)ai[ID]->obj;
	mainplayer->emoticon(emID);
}

- (void) openChatView {

	STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;

	self.inChatView = [[[STInGameChatView alloc] init] autorelease];
	[self.inChatView setMessages:ctr.messages];
	self.inChatView.delegate = self;
	[self.view addSubview:self.inChatView];

	[self.inChatView open];

	[UIView animateWithDuration:0.35
					 animations:^{

						 chatButton.alpha = 0.0f;
						 self.exclamationMark.alpha = 0.0f;
					 }];

}

- (void) updateChatView {

	[self.inChatView update];

	if (chatButton.alpha < 1.0) {
		return; //do not update icon if already opened.
	}

	[UIView animateWithDuration:0.25
					 animations:^{

						 self.exclamationMark.alpha = 1.0;
					 }];

}

- (void) chatViewWillClose:(STInGameChatView *)sender {

	__block ViewController *weakSelf = self;

	[sender close:^{
		weakSelf.inChatView = nil;

		[UIView animateWithDuration:0.35
						 animations:^{

							 chatButton.alpha = 1.0f;
						 }];

	}];

}

- (void) summaryScreendidHide:(STSummaryScreen *)sender {

	[UIView animateWithDuration:0.35
					 animations:^{

						 boardButton.alpha = 1.0f;
					 } completion:^(BOOL finished) {

					 }];

}

- (void) boardsOpen {

	if (self.summary) {
		[self.summary animateBack];
	}

	[UIView animateWithDuration:0.35
					 animations:^{

						 boardButton.alpha = 0.0f;
					 }];
}

- (void) startGame:(int)botCount mapName:(NSString*)mapName isGameHard:(BOOL)hardGame renderWater:(BOOL) haveWater
{
	self.waterEnabled = haveWater;

	chatButton.hidden = YES;
	boardButton.alpha = 0.0f;

	self.isGameHard = hardGame;
	self.currentMapName = mapName;

	self.gameMode = STGameModeSinglePlayer;
	[self setPaused:YES];
	_maxBotsCount = botCount+1;

	[self delayRestart];
	[self.view setNeedsDisplay];
}

- (void) startMultiGame:(int)users withBots:(int)bots MapName:(NSString*)mapName avatars:(int*)avToCpy renderWater:(BOOL) haveWater;
{
	self.waterEnabled = haveWater;

	chatButton.hidden = NO;
	boardButton.alpha = 0.0f;

	self.currentMapName = mapName;

	self.gameMode = STGameModeMultiPlayer;

	for (int i=0; i<6; i++) {
		_emoticons[i].alpha = 1.0f;
	}

	[self setPaused:YES];
	_maxBotsCount = users + bots;
	_multiPlayerOffset = users;

	memcpy(playersAvatarsIDS, avToCpy, MAX_PEERS_COUNT * sizeof(int));

	[self delayRestart];
	[self.view setNeedsDisplay];
}

- (void) generateAndSendStats {

	for( int i = 0; i < _maxBotsCount; ++i ){
		Tank *curr = ((Tank*)ai[i]->obj);

		MPTanksStats[i].damageTaken = curr->damageTaken;
		MPTanksStats[i].shootsPerformed = curr->shootsPerformed;
		MPTanksStats[i].hitsPerformed = curr->hitsPerformed;
	}

	STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;

	[ctr sendNetworkPacket:[STGCHelper sharedInstance].match
				   packetID:NETWORK_GAME_SYNC_STATS
				   withData:&MPTanksStats[0]
				   reserved:0
				   ofLength:sizeof(gameStats) * MAXMULTIPLAYERPLAYERCOUNT
				   reliable:YES
					 toUser:ctr.connectedPlayers];
}

- (void) updateMultiplauyerStats {

	for( int i = 0; i < _maxBotsCount; ++i ){
		Tank *curr = ((Tank*)ai[i]->obj);

		curr->damageTaken = MPTanksStats[i].damageTaken;
		curr->shootsPerformed = MPTanksStats[i].shootsPerformed;
		curr->hitsPerformed = MPTanksStats[i].hitsPerformed;
	}
}

- (void) dismiss
{
	turretPointer.releaseBlock = nil;

	if(self.gameMode == STGameModeMultiPlayer) {

		int empty = 0;

		STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;
		[ctr sendNetworkPacket:[STGCHelper sharedInstance].match
					  packetID:NETWORK_GAME_PLAYER_LEAVE
					  withData:&empty
					  reserved:0
					  ofLength:sizeof(int)
					  reliable:NO
						toUser:ctr.connectedPlayers];

		double delayInSeconds = 3.0;
		dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
		dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
			[ctr matchEnded];
		});

	}

	CATransition *animation = [CATransition animation];
	[animation setDuration:0.35];
	[animation setType:kCATransitionFade];
	[animation setSubtype:kCATransitionFromTop];
	[animation setTimingFunction:[CAMediaTimingFunction functionWithName:kCAMediaTimingFunctionEaseInEaseOut]];

	[[self.navigationController.view layer] addAnimation:animation forKey:kCATransitionFromLeft];

	[self.navigationController popViewControllerAnimated:NO];
}

- (BOOL) gestureRecognizer:(UIGestureRecognizer *)gestureRecognizer shouldReceiveTouch:(UITouch *)touch
{
	return  (![backButton pointInside:[touch locationInView:backButton] withEvent:nil]) ||
			(![chatButton pointInside:[touch locationInView:chatButton] withEvent:nil]) ||
			(![boardButton pointInside:[touch locationInView:boardButton] withEvent:nil]) ||
			(![againButton pointInside:[touch locationInView:againButton] withEvent:nil]);
}

- (void) fireTank
{
	if(self.gameMode == STGameModeSinglePlayer){
		if((!( (Tank*)ai[0] -> obj ) -> live) || playerWinGame){

#if !(TARGET_IPHONE_SIMULATOR)

			NSMutableDictionary *event =
			[[GAIDictionaryBuilder createEventWithCategory:@"GAME"
													action:@"RESTART"
													 label:@"SINGLEPLAYER"
													 value:nil] build];

			[[GAI sharedInstance].defaultTracker send:event];

#endif
			__block ViewController *weakSelf = self;

			[self.summary hide:^{
				weakSelf.summary = nil;
			}];

			[self setPaused:YES];
			//delete tank_player;
			delete rocks;
			delete ground;
			delete mapRandomPoint;
			mapRandomPoint = NULL;

			delete mapParticleLayer;
			mapParticleLayer = NULL;

			for( int i = 0; i < _maxBotsCount; ++i )
				delete ai[ i ];

			Timer = 5;

			Bonuses . Clear();
			Sparcles . Clear();
			ParticleSmoke . Clear();
			LightSparcles . Clear();
			TankSmoke . Clear();
			TankSmokeWater . Clear();
			TankSmokeGrass . Clear();
			TreeParticles . Clear();
			HouseParticles . Clear();

			Vehicle::all.clear();
			Vehicle::col.clear();

			Bullets.RemoveAllObjects();

			directionPointer.alpha = 1.0f;
			turretPointer.alpha = 1.0f;
			mineButton.alpha = 1.0f;
			mineImage.alpha = 1.0f;

			for (int i=0; i<_maxBotsCount; i++) {
				[nicks[i] release];
				[nicks[i] removeFromSuperview];
			}

			for(int i=0; i<_maxBotsCount; i++){
				delete aiAvatar[i];
			}

			[self delayRestart];
		}

	}
	else{
		if(!( !playerWinGame && !loseSound )) {

#if !(TARGET_IPHONE_SIMULATOR)

			NSMutableDictionary *event =
			[[GAIDictionaryBuilder createEventWithCategory:@"GAME"
													action:@"RESTART"
													 label:@"MULTIPLAYER"
													 value:nil] build];

			[[GAI sharedInstance].defaultTracker send:event];
#endif
			int empty = 0;

			STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;
			[ctr sendNetworkPacket:[STGCHelper sharedInstance].match
						  packetID:NETWORK_GAME_FINISH
						  withData:&empty
						  reserved:0
						  ofLength:sizeof(int)
						  reliable:YES
							toUser:ctr.connectedPlayers];

			[self restartMultiplayerGame];

			double delayInSeconds = 1.0;
			dispatch_time_t popTime = dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayInSeconds * NSEC_PER_SEC));
			dispatch_after(popTime, dispatch_get_main_queue(), ^(void){
				[ctr sendResetSignal];
			});
		}

	}
}

- (void) restartMultiplayerGame {

	[self setPaused:YES];

	boardButton.alpha = 0.0f;

	__block ViewController *weakSelf = self;

	[self.summary hide:^{
		weakSelf.summary = nil;
	}];

	[SVProgressHUD showWithStatus:NSLocalizedString(@"Restarting...", nil) maskType:SVProgressHUDMaskTypeGradient];

	_gameLoaded = NO;

	for (int i=0; i<6; i++) {
		_emoticons[i].alpha = 1.0f;
	}

	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"mapmusic_01" ofType:@"mp3"];
	BOOL paused = [APPDELEGATE.musicTrack paused];
	[APPDELEGATE.musicTrack playFile:filePath loops:-1];
	[APPDELEGATE.musicTrack setPaused:paused];

	playerWinGame = NO;
	loseSound = NO;
	showWinImage = YES;
	showLooserImage = YES;
	againButton.hidden = YES;

	Timer = 5;

	Bonuses . Clear();
	Sparcles . Clear();
	ParticleSmoke . Clear();
	LightSparcles . Clear();
	TankSmoke . Clear();
	TankSmokeGrass . Clear();
	TankSmokeWater . Clear();
	TreeParticles . Clear();
	HouseParticles . Clear();
	Bullets.RemoveAllObjects();

	rocks->ResetAll();

	directionPointer.alpha = 1.0f;
	turretPointer.alpha = 1.0f;
	mineButton.alpha = 1.0f;
	mineImage.alpha = 1.0f;
	backButton.alpha = 1.0f;

	camera . LookAt = ai[USER_MULTI_ID]->obj->Pos;
	select = 0;

	SoundManager -> GetSound( "ready" ) -> PlayAt( kZeroVectorf , 1.0f );

	for( int i = 0; i < _maxBotsCount; ++i ){
		Tank *curr = ((Tank*)ai[i]->obj);

		Vector3f rnd = mapRandomPoint->GetRandomPoint();

		curr->ResetTank();
		curr->Pos[0] = rnd._x;
		curr->Pos[2] = rnd._z;


	}

	Tank *wsk = ((Tank*)ai[USER_MULTI_ID]->obj);
	wsk->ai = false;

}

-(void) dropMine
{
	if(self.gameMode == STGameModeSinglePlayer){
		Tank * mainplayer = (Tank*)ai[0]->obj;
		mainplayer->handleMineDrop();
	}
	else{
		Tank * mainplayer = (Tank*)ai[USER_MULTI_ID]->obj;
		mainplayer->handleMineDrop();
	}
}

- (void) handleMultMineDrop:(int)ID
{
	Tank * mainplayer = (Tank*)ai[ID]->obj;
	mainplayer->handleMineDrop();
}

- (void) handleMultiShoot:(int)ID
{
	Tank * mainplayer = (Tank*) ai[ID]->obj;
	mainplayer->Shoot();
}

- (void) handleMultiTankDie:(int)ID
{
	Tank * mainplayer = (Tank*) ai[ID]->obj;
	mainplayer->Die();

	BOOL end = YES;

	for(int i=0; i<_multiPlayerOffset; i++) {
		if(!((Tank*)ai[i]->obj)->tankDie){
			end = NO;
			break;
		}
	}

	if(end && USER_MULTI_ID == 0){
		NSLog(@"POKAZUJE!!!");
		[UIView animateWithDuration:0.35
						 animations:^{
							 againButton.alpha = 1.0f;
						 }];
	}
}

-(void) delayRestart
{
	self.changePlayerTap.enabled = NO;

	if(self.gameMode == STGameModeSinglePlayer) {
		NSString *path = [[NSBundle mainBundle]pathForResource:@"Bots" ofType:@"json"];
		NSData *data = [NSData dataWithContentsOfFile:path];
		self.botsInfo = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];
		self.botsSortedKeys = [NSMutableArray arrayWithArray:[self.botsInfo allKeys]];
	}
	else{

		NSString *path = [[NSBundle mainBundle]pathForResource:@"Bots" ofType:@"json"];
		NSData *data = [NSData dataWithContentsOfFile:path];
		NSDictionary *currBots = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:nil];

		NSMutableDictionary *currentPlayers = [NSMutableDictionary dictionary];

		NSArray *allBotsNames = [currBots allKeys];

		int counter = 0;
		int botsCounter = 0;

		STMultiplayerViewController *ctr = [STMultiplayerBridge sharedInstance].multiController;

		self.botsSortedKeys = [NSMutableArray array];

		for (int i=0; i<_maxBotsCount; i++) {

			if(i == USER_MULTI_ID){

				NSString *youKey = NSLocalizedString(@"You", nil);

				[currentPlayers setObject: @{@"avatar": self.gameAvatars[playersAvatarsIDS[USER_MULTI_ID]]} forKey: youKey ];
				[self.botsSortedKeys addObject: youKey];
			}
			else{

				if(counter < (_multiPlayerOffset - 1)){

					NSString *playerID = ctr.pairedPlayerNames[i];

					[currentPlayers setObject: @{@"avatar": self.gameAvatars[playersAvatarsIDS[i]]} forKey: playerID];
					[self.botsSortedKeys addObject:playerID];
				}
				else{

					NSString *botID = allBotsNames[botsCounter];

					[currentPlayers setObject: currBots[botID] forKey: botID];
					[self.botsSortedKeys addObject:botID];

					botsCounter++;
				}

				counter++;
			}
		}

		self.botsInfo = currentPlayers;
	}

	boardButton.alpha = 0.0f;

	NSString *filePath = [[NSBundle mainBundle] pathForResource:@"mapmusic_01" ofType:@"mp3"];
	BOOL paused = [APPDELEGATE.musicTrack paused];
	[APPDELEGATE.musicTrack playFile:filePath loops:-1];
	[APPDELEGATE.musicTrack setPaused:paused];

	NSString *navigationMesh = [self.currentMapName stringByAppendingString:@"_spawn"];
	mapRandomPoint = new RandomMeshPoint([navigationMesh UTF8String]);

	NSString *layerFile = [self.currentMapName stringByAppendingString:@"_layers"];
	mapParticleLayer = new STParticleLayer([layerFile UTF8String]);

	ground = new Ground( 256, 256, 256, [self.currentMapName UTF8String], self.waterEnabled);

	rocks = new Building( ground , [self.currentMapName UTF8String] );

	Bullets.ground = ground;
	Bullets.mapObjects = rocks;

	playerWinGame = NO;
	loseSound = NO;
	showWinImage = YES;
	showLooserImage = YES;
	againButton.hidden = YES;

	__block ViewController *wekSlf = self;

	turretPointer.releaseBlock = ^{
		if(wekSlf.gameMode == STGameModeSinglePlayer){
			Tank * mainplayer = (Tank*)ai[0]->obj;
			mainplayer->Shoot();
		}
		else{
			Tank * mainplayer = (Tank*)ai[USER_MULTI_ID]->obj;
			mainplayer->Shoot();
		}
	};

	srand(time(NULL));

	NSArray * botsNames = self.botsSortedKeys;

	if(self.gameMode == STGameModeSinglePlayer){

		for( int i = 0; i < _maxBotsCount; ++i ) {
			Vector3f rnd = mapRandomPoint->GetRandomPoint();
			ai[ i ] = new Vehicle( new Tank((i==0 ? nil : _botsInfo[ botsNames[ i - 1 ] ] ), (i==0 ? false : true) , rnd._x , 0 , rnd._z , ground , 0, TankTypeNormal, self.isGameHard) );

			((Tank*)ai[ i ]->obj)->TANK_MULTI_ID = i;
		}

		for (int i=0; i<_maxBotsCount; i++) {
			nicks[i] = [[[UILabel alloc] initWithFrame:CGRectZero] retain];
			nicks[i].font = [UIFont fontWithName:@"Orbitron-Light" size:12/RETINA_SCALE];
			nicks[i].textAlignment = NSTextAlignmentLeft;
			nicks[i].text = (i == 0 ? NSLocalizedString(@"You", nil) : botsNames[ i - 1 ]);
			nicks[i].backgroundColor = [UIColor clearColor];
			nicks[i].textColor = [UIColor whiteColor];
			nicks[i].shadowColor = [UIColor blackColor];
			nicks[i].shadowOffset = CGSizeMake(1, 1);

			aiAvatar[i] = new Texture( (i == 0 ? [[self.gameAvatars[playersAvatarsIDS[0]] stringByAppendingString:@"_tiny"] UTF8String] : [[[[_botsInfo objectForKey:botsNames[ i - 1 ]] objectForKey:@"avatar"] stringByAppendingString:@"_tiny"] UTF8String] ) );

			[self.view addSubview:nicks[i]];
		}

	}
	else{

		for (int i=0; i<_maxBotsCount; i++) {
			nicks[i] = [[[UILabel alloc] initWithFrame:CGRectZero] retain];
			nicks[i].font = [UIFont fontWithName:@"Orbitron-Light" size:12/RETINA_SCALE];
			nicks[i].textAlignment = NSTextAlignmentLeft;
			nicks[i].text = botsNames[ i ];
			nicks[i].backgroundColor = [UIColor clearColor];
			nicks[i].textColor = [UIColor whiteColor];
			nicks[i].shadowColor = [UIColor blackColor];
			nicks[i].shadowOffset = CGSizeMake(1, 1);

			aiAvatar[i] = new Texture( [[[[_botsInfo objectForKey:botsNames[ i ]] objectForKey:@"avatar"] stringByAppendingString:@"_tiny"] UTF8String] );

			[self.view addSubview:nicks[i]];
		}

		for(int p=0; p<_multiPlayerOffset; p++){
			Vector3f rnd = mapRandomPoint->GetRandomPoint();

			ai[ p ] = new Vehicle( new Tank(nil, false , rnd._x , 0 , rnd._z, ground , p) );

			((Tank*)ai[p]->obj)->isMultiPlayer = YES;
			((Tank*)ai[p]->obj)->TANK_MULTI_ID = p;
			((Tank*)ai[p]->obj)->encodePacket();

			if(p == USER_MULTI_ID){
				((Tank*)ai[p]->obj)->isSterable = YES;
			}
			else{
				((Tank*)ai[p]->obj)->isSterable = NO;
			}
		}

		for( int i = _multiPlayerOffset; i < _maxBotsCount; ++i ){

			Vector3f rnd = mapRandomPoint->GetRandomPoint();

			ai[ i ] = new Vehicle( new Tank(_botsInfo[ botsNames[ i - 1 ] ], true , rnd._x , 0 , rnd._z , ground , i) );
			((Tank*)ai[i]->obj)->isMultiPlayer = YES;
			((Tank*)ai[i]->obj)->TANK_MULTI_ID = i;
			((Tank*)ai[i]->obj)->encodePacket();

			if(USER_MULTI_ID == kServer){
				((Tank*)ai[i]->obj)->isSterable = YES;
			}
			else{
				((Tank*)ai[i]->obj)->isSterable = NO;
			}
		}
	}

	camera = Camera( 80, 60, 0 );

	if(self.gameMode == STGameModeMultiPlayer){

		camera . LookAt = ai[USER_MULTI_ID]->obj->Pos;
	}
	else {

		for( int i = 0; i < 3; ++i )
			Bonuses . AddBonus( new Mine( NULL ) );

		camera . LookAt = ai[0] -> obj -> Pos;
	}

	select = 0;

	SoundManager -> GetSound( "ready" ) -> PlayAt( kZeroVectorf , 1.0f );

	if(self.gameMode == STGameModeSinglePlayer){
		_gameLoaded = YES;
		[self setPaused:NO];

		self.startMatchDate = [NSDate date];
	}
}

- (void) multiUnpauseAndStart
{
	NSLog(@"UNLOCKING!!!");

	_gameLoaded = YES;
	[self setPaused:NO];
	[SVProgressHUD dismiss];

	self.startMatchDate = [NSDate date];
}

- (void)didReceiveMemoryWarning
{
	[super didReceiveMemoryWarning];

	if ([self isViewLoaded] && ([[self view] window] == nil)) {
		self.view = nil;

		[self tearDownGL];

		if ([EAGLContext currentContext] == self.context) {
			[EAGLContext setCurrentContext:nil];
		}
		self.context = nil;
	}

	if ([self __getMemoryUsedPer1] > 0.45) {
		NSLog(@"GAME STARTS TO USE TO MUCH RESOURCES...");
	}

	// Dispose of any resources that can be recreated.
}

- (float)__getMemoryUsedPer1
{

#ifndef ANDROID
	struct mach_task_basic_info info;
	mach_msg_type_number_t size = sizeof(info);
	kern_return_t kerr = task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &size);
	if (kerr == KERN_SUCCESS)
	{
		float used_bytes = info.resident_size;
		float total_bytes = [NSProcessInfo processInfo].physicalMemory;
		//NSLog(@"Used: %f MB out of %f MB (%f%%)", used_bytes / 1024.0f / 1024.0f, total_bytes / 1024.0f / 1024.0f, used_bytes * 100.0f / total_bytes);
		return used_bytes / total_bytes;
	}
	return 1;
#else
	return 0;
#endif

}

- (void)setupGL
{
	[EAGLContext setCurrentContext:self.context];


	MainMatrix3D = [[MatrixPool3D alloc] init]; //tworzymy matrix pool

	MainMatrix3D->Projection = GLKMatrix4MakePerspective(( 45 * M_PI  ) / 180.0,
														 ScreenX/ScreenY,
														 0.01,
														 6000.0f);
	MainMatrix3D->ModelView = GLKMatrix4Identity;

	TexMenager = [[LoadPNG alloc] init]; //tworzymy manager textur bez tego nie da sie ladowac textur
}

- (void)tearDownGL
{
	[EAGLContext setCurrentContext:self.context];

	//delete buffers and programs and everything !
}

- (void) decodeTankPacket:(int)ID;
{
	if(_gameLoaded){
		Tank *wsk = (Tank*)ai[ID]->obj;
		tankInfo *pkg = &MPTanksTable[ID];

		wsk->Pos[0] = pkg->position[0];
		wsk->Pos[2] = pkg->position[1];
		wsk->Rot    = pkg->rotation;
		wsk->rifle_rot = pkg->rifle_rot;
		wsk->rifle_angle = pkg->rifle_angle;
		wsk->turn_to = pkg->turn_to;
		wsk->acc_to = pkg->acc_to;
		wsk->acc = pkg->acc;
		wsk->score = pkg->score;
		wsk->live = pkg->live;
		wsk->turn_acc = pkg->turn_acc;
		wsk->tankDie  = pkg->tankDie;
		wsk->active = pkg->active;
	}
}

- (void) handleMultiHealtPack:(float*)Pos
{
	Bonus *bns = new HeathPack();
	memcpy(bns->Pos, Pos, sizeof(float)*3);
	Bonuses . AddBonus( bns );
}

- (void) handleMultiMinePack:(float*)Pos
{
	Bonus *bns = new MineBonus();
	memcpy(bns->Pos, Pos, sizeof(float)*3);
	Bonuses . AddBonus( bns );
}

#pragma mark - change player in observer mode

- (void) changePlayer
{
	select = ( select + 1 ) % _maxBotsCount;

	Tank * wsk = (Tank*)ai[ select ]->obj;

	if( !wsk -> live )
	{
		wsk = (Tank*)ai[ select ]->obj;

		for (int i = 0; i < _maxBotsCount; i++) {
			if(wsk->live){
				camera . LookAt = wsk -> Pos;
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