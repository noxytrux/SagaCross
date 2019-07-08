#pragma once

#include "SCGround.h"

extern float SparcleVertex[ 3 * 10000 ] , *pSparcleVertex;
extern float SparcleColor[ 4 * 10000 ] , *pSparcleColor;
extern float SparcleRotation[ 3 * 10000 ] , *pSparcleRotation;

namespace sc {

	inline float fast_absf(float g)
	{
		unsigned int & gg = *(unsigned int*)&g;
		gg &= 0x7FFFFFFF;
		return *(float*)(&gg);
	}

	class SCSparcle
	{
	public:

		float Pos[3];
		float Acc[3];
		float Wind[3];

		float Time;
		float MaxTime;
		float graviti;
		float h;

		float color[4];
		float angle;
		float random;
		bool rotated;

		SCSparcle(const std::shared_ptr<SCGround> &ground, bool isRotated = true) : _ground(ground)
		{
			rotated = isRotated;
			Time = 0.0f;
			graviti = -10;

			angle = 0.0f;
			random = rotated ? rand() % 64 : ((float)(rand() % 360) * M_PI / 180.0);
		}

		bool Draw(float dt)
		{
			Time += dt;

			if (Time > MaxTime) {
				return true;
			}

			float k = Time / MaxTime;

			Pos[0] += (Acc[0] + Wind[0]) * dt;
			Pos[1] += (Acc[1] + Wind[1]) * dt;
			Pos[2] += (Acc[2] + Wind[2]) * dt;

			Acc[1] += graviti * dt;

			if (Acc[0] * Acc[0] + Acc[2] * Acc[2] > 0.08f) {

				h = _ground->GetHeight(Pos[0], Pos[2]);
			}
			
			if (Pos[1] < h && Acc[1] < -0.05f) 
			{
				Pos[1] = h;

				xVec3 in(Acc);

				float length = in.magnitudeSquared();

				in /= length;

				xVec3 norm(_ground->GetNormal(Pos[0], Pos[2]));

				xVec3 out = xVec3::reflect(in, norm); 

				out *= length * 0.3;

				memcpy(Acc, &out, 12);
			}

			angle = rotated ? (Time + random) : random;

			color[3] = 1.0 - fast_absf(k * 2.0 - 1.0); 

			float c[4] = { color[0] * color[3], color[1] * color[3], color[2] * color[3], color[3] };
			float r[3] = { sinf(angle), cosf(angle), (1.0f - k) };

			memcpy(pSparcleVertex, Pos, 12); pSparcleVertex += 3;
			memcpy(pSparcleColor, c, 16); pSparcleColor += 4;
			memcpy(pSparcleRotation, r, 12); pSparcleRotation += 3;

			return false;
		}

	private:

		std::shared_ptr<SCGround> _ground;
	};

	class SCSparclesManager final {

	public:

		std::vector< SCSparcle > S;
		
		float graviti;
		float Wind[3];
		float p_size_min;
		float p_size_max;
		float p_size_decrase;
		bool rotated;

		GLuint texture;

		SCSparclesManager(const std::shared_ptr<SCGround> &ground, 
						  const std::shared_ptr<SCOpenGLRenderable> &renderer,
						  int t = 255, float g = -10, int s = 3, float w1 = 0, float w2 = 0, float w3 = 0) 
			: _ground(ground)
			, _renderer(renderer)
			, graviti(g)
			, p_size_min(s)
			, p_size_max(s)
		{
			rotated = true;
			p_size_decrase = 0.0f;

			pSparcleVertex = SparcleVertex;
			pSparcleColor = SparcleColor;
			pSparcleRotation = SparcleRotation;
		}

		void Clear() {

			S.clear();
		}

		void Add(float x, float y, float z, float vx, float vy, float vz, int num, GLubyte r, GLubyte g, GLubyte b, float MaxTime = 4 + rand() % 10 * 0.2f)
		{

			SCSparcle s(_ground, rotated);

			s.Pos[0] = x;
			s.Pos[1] = y;
			s.Pos[2] = z;
			memcpy(s.Wind, Wind, 12);
			s.graviti = graviti;

			s.MaxTime = MaxTime;

			for (int i = 0; i < num; ++i) 
			{
				s.color[0] = (float)r / 255.0f; 
				s.color[1] = (float)g / 255.0f; 
				s.color[2] = (float)b / 255.0f; 

				s.Acc[0] = vx + (rand() % 20 - 10) * 0.1;
				s.Acc[1] = (vy + (rand() % 20 - 10) * 0.1) * (rand() % 3 * 0.3f + 0.3f);
				s.Acc[2] = vz + (rand() % 20 - 10) * 0.1;

				S.push_back(s);
			}

		}

		static void Start() {

			glDisable(GL_TEXTURE_2D);
		}

		static void End() {

			glEnable(GL_TEXTURE_2D);
		}

		void Render(float dt)
		{
			auto current = _renderer->ParticleShader;

			glActiveTexture(GL_TEXTURE0);
			glUniform1i(current->uniforms[UNI_TEX0], 0);
			glBindTexture(GL_TEXTURE_2D, texture);

			pSparcleVertex = SparcleVertex;
			pSparcleColor = SparcleColor;
			pSparcleRotation = SparcleRotation;

			glEnable(GL_BLEND);

			glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.m());
			glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _renderer->ModelView.m());

			for (int i = 0; i < S.size(); ) {
			
				if (S[i].Draw(dt))
				{
					S.erase(S.begin() + i);
				}
				else {
				
					++i;
				}
			}

			glUniform3f(current->uniforms[UNI_TEX2], p_size_min, p_size_max, p_size_decrase);

			glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, 0, SparcleVertex);
			glEnableVertexAttribArray(ATTRIB_VERTEX);
			glVertexAttribPointer(ATTRIB_COLOR, 4, GL_FLOAT, 0, 0, SparcleColor);
			glEnableVertexAttribArray(ATTRIB_COLOR);
			glVertexAttribPointer(ATTRIB_NORMAL, 3, GL_FLOAT, 0, 0, SparcleRotation);
			glEnableVertexAttribArray(ATTRIB_NORMAL);

			glDrawArrays(GL_POINTS, 0, S.size());

			glDisable(GL_BLEND);
			glLineWidth(1.0f);
		}

	private:
			
		std::shared_ptr<SCGround> _ground;
		std::shared_ptr<SCOpenGLRenderable> _renderer;
	};

	extern SCSparclesManager

	Sparcles,
	LightSparcles,
	ParticleSmoke,
	TankSmoke,
	TreeParticles,
	HouseParticles,
	TankSmokeWater,
	TankSmokeGrass;
}

