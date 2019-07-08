#pragma once

#include "SCRendererInterface.h"

namespace sc {

	class SCOpenGLRenderable : public SCRendererInterface {

	public:
		
		SCOpenGLRenderable(const std::shared_ptr<SCDisplay> &display, 
						   const std::shared_ptr<SCSettings> &settings,
						   const std::string &path);

		virtual ~SCOpenGLRenderable() noexcept;

		virtual void updateViewPort(const SCScreenSize &screenSize) override;
		virtual void beginDrawing() override;
		virtual void endDrawing() override;

		void loadFonts(const std::string &path);

		std::shared_ptr<GLShader> ParticleShader;
		std::shared_ptr<GLShader> SimpleShader;
		std::shared_ptr<GLShader> TerrainShader;
		std::shared_ptr<GLShader> GuiShader;
		std::shared_ptr<GLShader> FoliageShader;
		std::shared_ptr<GLShader> CutoffShader;
		std::shared_ptr<GLShader> WaterShader;

		GLuint blank;

		bool Get2DOGLPos(float px, float py, float pz, float & winx, float & winy);
		void DebugBlit(float x1, float y1, float rx, float ry, float resX = 1, float resY = 1);
		void RectFill(float x1, float y1, float x2, float y2, int r, int g, int b, int a);
		void RectFillGradient(int x1, int y1, int x2, int y2, int r1, int g1, int b1, float a, int r2, int g2, int b2, float a2);
		void RectFillGradientH(int x1, int y1, int x2, int y2, int r1, int g1, int b1, float a, int r2, int g2, int b2, float a2);
		void DrawBox(float x1, float y1, float z1, float x2, float y2, float z2, xMat34 & UserMatrix);
	
	private:

		struct nk_font_atlas *_atlas;
		struct nk_font *_orbitron30 = nullptr;
		struct nk_font *_orbitron15 = nullptr;
	};
}

