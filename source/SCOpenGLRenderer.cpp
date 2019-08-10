#include "SCOpenGLRenderer.h"
#include "glMatrix4x4.hpp"

#ifndef MOBILE
#include "SCGLFWDisplay.h"
#endif

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_INCLUDE_FONT_BAKING
#define NK_IMPLEMENTATION
#include "nuklear_glfw_gl3.h"

using namespace sc;

#if defined(__EMSCRIPTEN__) || defined(MOBILE)
int glhProjectf(float objx, float objy, float objz, float *modelview, float *projection, int *viewport, GLdouble *wx, GLdouble *wy, GLdouble *wz)
{
	float fTempo[8];
	
	fTempo[0]=modelview[0]*objx+modelview[4]*objy+modelview[8]*objz+modelview[12]; // w is always 1
	fTempo[1]=modelview[1]*objx+modelview[5]*objy+modelview[9]*objz+modelview[13];
	fTempo[2]=modelview[2]*objx+modelview[6]*objy+modelview[10]*objz+modelview[14];
	fTempo[3]=modelview[3]*objx+modelview[7]*objy+modelview[11]*objz+modelview[15];
	
	// Projection transform, the final row of projection matrix is always [0 0 -1 0]
	// so we optimize for that.
	fTempo[4]=projection[0]*fTempo[0]+projection[4]*fTempo[1]+projection[8]*fTempo[2]+projection[12]*fTempo[3];
	fTempo[5]=projection[1]*fTempo[0]+projection[5]*fTempo[1]+projection[9]*fTempo[2]+projection[13]*fTempo[3];
	fTempo[6]=projection[2]*fTempo[0]+projection[6]*fTempo[1]+projection[10]*fTempo[2]+projection[14]*fTempo[3];
	fTempo[7]=-fTempo[2];
	
	// The result normalizes between -1 and 1
	if (fTempo[7]==0.0) // The w value
	 return 0;
	
	fTempo[7]=1.0/fTempo[7];
	
	// Perspective division
	fTempo[4]*=fTempo[7];
	fTempo[5]*=fTempo[7];
	fTempo[6]*=fTempo[7];
	
	*wx = (fTempo[4]*0.5+0.5)*viewport[2]+viewport[0];
	*wy = (fTempo[5]*0.5+0.5)*viewport[3]+viewport[1];
	*wz = (1.0+fTempo[6])*0.5;	// Between 0 and 1

	return 1;
}
#endif

SCOpenGLRenderable::SCOpenGLRenderable(const std::shared_ptr<SCDisplay> &display, 
									   const std::shared_ptr<SCSettings> &settings, 
									   const std::string &path)
	: SCRendererInterface(display, settings)
{
	const auto &size = display->getScreenSize();

	_ctx = nk_glfw3_init(size.width, size.height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearDepthf(1.0);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_DEPTH_TEST);

	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glActiveTexture(GL_TEXTURE0);

#ifdef MOBILE 
	glEnable(GL_POINT_SPRITE);
	auto shaderPath = path + "shaders/";
#else 
	glEnable(GL_PROGRAM_POINT_SIZE);
	auto shaderPath = path + "shaders/desktop/";
#endif
    
	glGenVertexArrays(1, &guiVAO);
	glBindVertexArray(guiVAO);
	glGenBuffers(1, &guiVBO);
	glBindBuffer(GL_ARRAY_BUFFER, guiVBO);

	//==================================================================

	ParticleShader = std::make_shared<GLShader>();
	ParticleShader->loadShader(shaderPath + "line");

	glBindAttribLocation(ParticleShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(ParticleShader->getProgramId(), ATTRIB_COLOR,  "color");
	glBindAttribLocation(ParticleShader->getProgramId(), ATTRIB_NORMAL, "rotvec");

	ParticleShader->linkProgram();
	ParticleShader->begin();

	ParticleShader->uniforms[UNI_PROJECTION_MAT] = ParticleShader->uniformLocation("projection"); 
	ParticleShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = ParticleShader->uniformLocation("modelView");
	ParticleShader->uniforms[UNI_TEX2] = ParticleShader->uniformLocation("pSize");
	ParticleShader->uniforms[UNI_TEX0] = ParticleShader->uniformLocation("pTex");

	//==================================================================

	SimpleShader = std::make_shared<GLShader>();
	SimpleShader->loadShader(shaderPath + "meshsimple");

	glBindAttribLocation(SimpleShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(SimpleShader->getProgramId(), ATTRIB_COORDS, "coord");

	SimpleShader->linkProgram();
	SimpleShader->begin();

	SimpleShader->uniforms[UNI_PROJECTION_MAT] = SimpleShader->uniformLocation("projection");
	SimpleShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = SimpleShader->uniformLocation("modelViewWorld");
	SimpleShader->uniforms[UNI_TEX0] = SimpleShader->uniformLocation("tex");
	SimpleShader->uniforms[UNI_TEX1] = SimpleShader->uniformLocation("mColor");

	//==================================================================
	
	TerrainShader = std::make_shared<GLShader>();
	TerrainShader->loadShader(shaderPath + "terrain");
	
	glBindAttribLocation(TerrainShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(TerrainShader->getProgramId(), ATTRIB_COORDS, "coord");

	TerrainShader->linkProgram();
	TerrainShader->begin();

	TerrainShader->uniforms[UNI_PROJECTION_MAT] = TerrainShader->uniformLocation("projection");
	TerrainShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = TerrainShader->uniformLocation("modelViewWorld");
	TerrainShader->uniforms[UNI_TEX0] = TerrainShader->uniformLocation("pTex");
	TerrainShader->uniforms[UNI_TEX1] = TerrainShader->uniformLocation("time");

	//==================================================================
	
	GuiShader = std::make_shared<GLShader>();
	GuiShader->loadShader(shaderPath + "gui");

	glBindAttribLocation(GuiShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(GuiShader->getProgramId(), ATTRIB_COORDS, "coord");

	GuiShader->linkProgram();
	GuiShader->begin();

	GuiShader->uniforms[UNI_PROJECTION_MAT] = GuiShader->uniformLocation("projection");
	GuiShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = GuiShader->uniformLocation("modelView");
	GuiShader->uniforms[UNI_TEX1] = GuiShader->uniformLocation("mColor");
	GuiShader->uniforms[UNI_TEX0] = GuiShader->uniformLocation("tex");

	//==================================================================

	FoliageShader = std::make_shared<GLShader>();
	FoliageShader->loadShader(shaderPath + "foliage");
	
	glBindAttribLocation(FoliageShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(FoliageShader->getProgramId(), ATTRIB_COORDS, "coord");

	FoliageShader->linkProgram();
	FoliageShader->begin();
	
	FoliageShader->uniforms[UNI_PROJECTION_MAT] = FoliageShader->uniformLocation("projection");
	FoliageShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = FoliageShader->uniformLocation("modelViewWorld");
	FoliageShader->uniforms[UNI_TEX0] = FoliageShader->uniformLocation("pTex");
	FoliageShader->uniforms[UNI_TEX1] = FoliageShader->uniformLocation("time");
	FoliageShader->uniforms[UNI_TEX2] = FoliageShader->uniformLocation("speed");
	FoliageShader->uniforms[UNI_TEX3] = FoliageShader->uniformLocation("amplitude");

	//==================================================================
	
	CutoffShader = std::make_shared<GLShader>();
	CutoffShader->loadShader(shaderPath + "cutoff");

	glBindAttribLocation(CutoffShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(CutoffShader->getProgramId(), ATTRIB_COORDS, "coord");

	CutoffShader->linkProgram();
	CutoffShader->begin();

	CutoffShader->uniforms[UNI_PROJECTION_MAT] = CutoffShader->uniformLocation("projection");
	CutoffShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = CutoffShader->uniformLocation("modelViewWorld");
	CutoffShader->uniforms[UNI_TEX0] = CutoffShader->uniformLocation("tex");
	CutoffShader->uniforms[UNI_TEX1] = CutoffShader->uniformLocation("mColor");
	CutoffShader->uniforms[UNI_TEX2] = CutoffShader->uniformLocation("cutoff");

	//==================================================================
	
	WaterShader = std::make_shared<GLShader>();
	WaterShader->loadShader(shaderPath + "water");

	glBindAttribLocation(WaterShader->getProgramId(), ATTRIB_VERTEX, "position");
	glBindAttribLocation(WaterShader->getProgramId(), ATTRIB_COORDS, "coord");

	WaterShader->linkProgram();
	WaterShader->begin();

	WaterShader->uniforms[UNI_PROJECTION_MAT] = WaterShader->uniformLocation("projection");
	WaterShader->uniforms[UNI_MODELVIEW_WORLD_MAT] = WaterShader->uniformLocation("modelViewWorld");
	WaterShader->uniforms[UNI_TEX0] = WaterShader->uniformLocation("texWater");
	WaterShader->uniforms[UNI_TEX1] = WaterShader->uniformLocation("texWaterCaustics");
	WaterShader->uniforms[UNI_TEX2] = WaterShader->uniformLocation("texWaterCausticsSecond");
	WaterShader->uniforms[UNI_TEX3] = WaterShader->uniformLocation("time");

	glBindVertexArray(0);
}

SCOpenGLRenderable::~SCOpenGLRenderable() noexcept
{
	nk_glfw3_shutdown();

	glDeleteBuffers(1, &guiVBO);
	glDeleteVertexArrays(1, &guiVAO);

	glUseProgram(0);
	glBindVertexArray(0);
}

void SCOpenGLRenderable::loadFonts(const std::string &path)
{
#if defined(__ANDROID__)
    auto orbitronPath = path + "Lato-Light.ttf";
#else
	auto orbitronPath = path + "orbitron-light.ttf";
#endif

	nk_glfw3_font_stash_begin(&_atlas);
	_orbitron30 = nk_font_atlas_add_from_file(_atlas, orbitronPath.c_str(), 50, 0);
	_orbitron15 = nk_font_atlas_add_from_file(_atlas, orbitronPath.c_str(), 25, 0);
	nk_glfw3_font_stash_end();

	nk_style_set_font(_ctx, &_orbitron30->handle);

	_fontlist.push_back(_orbitron30);
	_fontlist.push_back(_orbitron15);
}

void SCOpenGLRenderable::updateViewPort(const SCScreenSize &screenSize)
{
	nk_glfw3_update(screenSize.width, screenSize.height);
}

void SCOpenGLRenderable::beginDrawing()
{
	auto size = getDisplay()->getScreenSize();

	glViewport(0, 0, size.width, size.height);
	glClearColor(107/255.0, 130/255.0, 146/255.0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SCOpenGLRenderable::endDrawing()
{
	nk_glfw3_render(NK_ANTI_ALIASING_ON, MAX_VERTEX_BUFFER, MAX_ELEMENT_BUFFER);

#ifndef MOBILE
	const auto glfwDisplay = static_cast<GLFWwindow*>(getDisplay()->getContext());
	assert(glfwDisplay);

	glfwSwapBuffers(glfwDisplay);
#endif
	glBindVertexArray(0);
}

bool SCOpenGLRenderable::Get2DOGLPos(float px, float py, float pz, float &winx, float &winy)
{
	GLdouble wx, wy, wz;
	
	GLint viewport[4];

	const auto &size = getDisplay()->getScreenSize();

	viewport[0] = 0;
	viewport[1] = 0;
	viewport[2] = size.width;
	viewport[3] = size.height;

#if defined(__EMSCRIPTEN__) || defined(MOBILE)

    glhProjectf(px, py, pz, ModelView.getMatrix(), Projection.getMatrix(), viewport, &wx, &wy, &wz);

#else

    GLdouble mv[16]; //ModelView.getMatrix()
    GLdouble proj[16]; //Projection.getMatrix()

    for (int i=0; i<16; ++i) {

        mv[i] = ModelView.getMatrix()[i];
        proj[i] = Projection.getMatrix()[i];
    }

	gluProject(px, py, pz, mv, proj, viewport, &wx, &wy, &wz);
#endif

	winx = wx;
	winy = size.height - wy;

	return true; //wz >= 0 && wz < 1;
}

void SCOpenGLRenderable::DebugBlit(float x1, float y1, float rx, float ry, float resX, float resY)
{
	auto current = GuiShader;
	const auto &size = getDisplay()->getScreenSize();

	glMatrix4x4 projOrtho; 
	glMatrix4x4 modelOrtho; 

	projOrtho.setOrtho(0, size.width, size.height, 0, 1, 10);
	modelOrtho.translate(0, 0, -2);

	glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, projOrtho.getMatrix());
	glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, modelOrtho.getMatrix());

	float squareVertices[] = {
		x1      , y1 + ry, 0, 0.0f, 1.0f,
		x1 + rx , y1 + ry, 0, resX, 1.0f,
		x1      , y1     , 0, 0.0f, 1.0f - resY,
		x1 + rx , y1     , 0, resX, 1.0f - resY
	};

	glBindVertexArray(guiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_COORDS);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}

void SCOpenGLRenderable::RectFill(float x1, float y1, float x2, float y2, int r, int g, int b, int a)
{

	auto current = GuiShader;
	const auto &size = getDisplay()->getScreenSize();

	glMatrix4x4 projOrtho;
	glMatrix4x4 modelOrtho;

	projOrtho.setOrtho(0, size.width, size.height, 0, 1, 10);
	modelOrtho.translate(0, 0, -2);

	glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, projOrtho.getMatrix());
	glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, modelOrtho.getMatrix());

	float alpha = (float)a / 255.0f;
	float color[4] = { (float)r / 255.0f,(float)g / 255.0f, (float)b / 255.0f, alpha };
	glUniform4fv(current->uniforms[UNI_TEX1], 1, color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank);
	glUniform1i(current->uniforms[UNI_TEX0], 0);

	float squareVertices[] = {
		x1 - 0.375f , y2 - 0.375f, 0, 1, 1,
		x2 - 0.375f , y2 - 0.375f, 0, 0, 1,
		x1 - 0.375f , y1 - 0.375f, 0, 1, 0,
		x2 - 0.375f , y1 - 0.375f, 0, 0, 0
	};

	glBindVertexArray(guiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_COORDS);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SCOpenGLRenderable::RectFillGradient(int x1, int y1, int x2, int y2, int r1, int g1, int b1, float a, int r2, int g2, int b2, float a2)
{
	auto current = GuiShader;
	const auto &size = getDisplay()->getScreenSize();

	glMatrix4x4 projOrtho;
	glMatrix4x4 modelOrtho;

	projOrtho.setOrtho(0, size.width, size.height, 0, 1, 10);
	modelOrtho.translate(0, 0, -2);

	glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, projOrtho.getMatrix());
	glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, modelOrtho.getMatrix());

	float color[4] = { (float)r1 / 255.0f * a,(float)g1 / 255.0f * a, (float)b1 / 255.0f * a, a };
	glUniform4fv(current->uniforms[UNI_TEX1], 1, color);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, blank);
	glUniform1i(current->uniforms[UNI_TEX0], 0);

	float squareVertices[] = {
		x1 - 0.375f , y2 - 0.375f, 0, 1, 1,
		x2 - 0.375f , y2 - 0.375f, 0, 0, 1,
		x1 - 0.375f , y1 - 0.375f, 0, 1, 0,
		x2 - 0.375f , y1 - 0.375f, 0, 0, 0
	};

	glBindVertexArray(guiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_COORDS);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void SCOpenGLRenderable::RectFillGradientH(int x1, int y1, int x2, int y2, int r1, int g1, int b1, float a, int r2, int g2, int b2, float a2)
{
	auto current = GuiShader;
	const auto &size = getDisplay()->getScreenSize();

	glMatrix4x4 projOrtho;
	glMatrix4x4 modelOrtho;

	projOrtho.setOrtho(0, size.width, size.height, 0, 1, 10);
	modelOrtho.translate(0, 0, -2);

	glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, projOrtho.getMatrix());
	glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, modelOrtho.getMatrix());

	float color[4] = { (float)r1 / 255.0f * a,(float)g1 / 255.0f * a, (float)b1 / 255.0f * a, a };
	glUniform4fv(current->uniforms[UNI_TEX1], 1, color);

	float squareVertices[] = {
		x1 - 0.375f , y1 - 0.375f, 0, 1, 1,
		x1 - 0.375f , y2 - 0.375f, 0, 0, 1,
		x2 - 0.375f , y1 - 0.375f, 0, 1, 0,
		x2 - 0.375f , y2 - 0.375f, 0, 0, 0

	};

	glBindVertexArray(guiVAO);
	glBindBuffer(GL_ARRAY_BUFFER, guiVBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * 5 * sizeof(float), squareVertices, GL_STATIC_DRAW);

	glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 5 * sizeof(float), (void*)(0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_VERTEX);
	glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, 5 * sizeof(float), (void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(ATTRIB_COORDS);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

}
