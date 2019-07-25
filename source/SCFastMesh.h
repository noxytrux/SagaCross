#pragma once

#include "Core-pch.hpp"
#include "SCOpenGLRenderer.h"

namespace sc {

	constexpr int32_t GMF_HEDERID = 0xB3D0;

	enum MESH_TYPE {
		MESH_BUMP,
		MESH_NORMAL,
		MESH_BOTH,
		MESH_ALPHA
	};

	typedef struct mdata {
		xVec3 position;
		xVec2 coord;
	} VertexData; //loading data struct

	typedef struct {
		GLuint  VAO;
		GLuint  VBOBuffer;
		GLuint  IndexBuffer;
		int		faceSize;
		int     vertexCount;

		GLuint DiffTex;
		GLuint NormalTex;
		MESH_TYPE TYPE;
	} SCMeshInfo; //mesh VAO definition

	class SCFastMesh {

	public:
		
		SCFastMesh(const std::string &filename, const std::shared_ptr<SCRendererInterface> &renderer) 
			: _renderer(std::dynamic_pointer_cast<SCOpenGLRenderable>(renderer)) 
		{
			_userMatrix.id();
			_finalWorld.loadIdentity();
			_worldmodelview.loadIdentity();

			_scaleX = 0.05f;
			_scaleY = 0.05f;
			_scaleZ = 0.05f;
			_AABBRay = -1.0;

			typedef uint8_t		BYTE;    
			typedef uint16_t	WORD;    
			typedef uint32_t	DWORD;
			typedef float		FLOAT;

			WORD heder = 0;

			DWORD mindex, index;
			DWORD nmesh, nvert, nface;
			FLOAT x, y, z, u, v;
			DWORD v1, v2, v3;
			FLOAT nx, ny, nz;
			char c, namestr[256];

			FILE* f = fopen(filename.c_str(), "rb");

			if (f == 0)
			{
				std::cout << "GMF::Wrong File Name or File not exist: " << filename << std::endl;
			}
			else
			{

				fread(&heder, sizeof(WORD), 1, f);

				if (heder != GMF_HEDERID)
				{	
					fclose(f);
				}
				else {

					fread(&nmesh, sizeof(DWORD), 1, f);

					_buffers.clear();
					_buffers.resize(nmesh, SCMeshInfo());
				
					for (mindex = 0; mindex < nmesh; mindex++)
					{
						index = 0;

						do
						{
							c = getc(f);
							namestr[index] = c;
							index++;
						} while (c != 0 && index < 256);

						namestr[255] = '\0';

						_buffers[mindex].DiffTex = 0;
						_buffers[mindex].NormalTex = 0;
						_buffers[mindex].TYPE = MESH_NORMAL;

						fread(&nvert, sizeof(DWORD), 1, f);
						fread(&nface, sizeof(DWORD), 1, f);

						std::vector<VertexData> ActualData;
						ActualData.resize(nvert);

						_buffers[mindex].faceSize = nface;
						_buffers[mindex].vertexCount = nvert;

						GLushort *Fdata = new GLushort[3 * _buffers[mindex].faceSize]; 

						float tlenght;
						xVec3 Temp;

						for (index = 0; index < nvert; index++)
						{

							fread(&x, sizeof(FLOAT), 1, f);
							fread(&y, sizeof(FLOAT), 1, f);
							fread(&z, sizeof(FLOAT), 1, f);
							fread(&u, sizeof(FLOAT), 1, f);
							fread(&v, sizeof(FLOAT), 1, f);
							fread(&nx, sizeof(FLOAT), 1, f);
							fread(&ny, sizeof(FLOAT), 1, f);
							fread(&nz, sizeof(FLOAT), 1, f);

							ActualData[index].position = xVec3(x, y, z);
                            ActualData[index].coord = xVec2(u, v);
						
							Temp = ActualData[index].position;
							tlenght = Temp.magnitude();

                            if (tlenght > _AABBRay) {

                                _AABBRay = tlenght;
                            }
						}

						for (index = 0; index < nface; index++)
						{

							fread(&v1, sizeof(DWORD), 1, f);
							fread(&v2, sizeof(DWORD), 1, f);
							fread(&v3, sizeof(DWORD), 1, f);
							fread(&nx, sizeof(FLOAT), 1, f);
							fread(&ny, sizeof(FLOAT), 1, f);
							fread(&nz, sizeof(FLOAT), 1, f);

							Fdata[index * 3] = v1;
							Fdata[index * 3 + 1] = v2;
							Fdata[index * 3 + 2] = v3;
						}

						glGenVertexArrays(1, &_buffers[mindex].VAO);
						glBindVertexArray(_buffers[mindex].VAO);

						glGenBuffers(1, &_buffers[mindex].VBOBuffer);
						glBindBuffer(GL_ARRAY_BUFFER, _buffers[mindex].VBOBuffer);
						glBufferData(GL_ARRAY_BUFFER, _buffers[mindex].vertexCount * sizeof(VertexData), ActualData.data(), GL_STATIC_DRAW);

						glVertexAttribPointer(ATTRIB_VERTEX, 3, GL_FLOAT, 0, sizeof(VertexData), (void*)OFFSET(VertexData, position));
						glEnableVertexAttribArray(ATTRIB_VERTEX);
						glVertexAttribPointer(ATTRIB_COORDS, 2, GL_FLOAT, 0, sizeof(VertexData), (void*)OFFSET(VertexData, coord));
						glEnableVertexAttribArray(ATTRIB_COORDS);

						glGenBuffers(1, &_buffers[mindex].IndexBuffer);
						glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffers[mindex].IndexBuffer);
						glBufferData(GL_ELEMENT_ARRAY_BUFFER, _buffers[mindex].faceSize * 3 * sizeof(GLushort), Fdata, GL_STATIC_DRAW);

						glBindBuffer(GL_ARRAY_BUFFER, 0);
						glBindVertexArray(0);

						if (Fdata) {

							delete[] Fdata;
							Fdata = nullptr;
						}

					}//for meshindex

					fclose(f);

				}//if heder OK


			}//if exist

		}

		virtual ~SCFastMesh() {

			for (int i = 0; i < _buffers.size(); i++) {

				glDeleteBuffers(1, &_buffers[i].VBOBuffer);
				glDeleteBuffers(1, &_buffers[i].IndexBuffer);
				glDeleteVertexArrays(1, &_buffers[i].VAO);
				_buffers[i].faceSize = 0;
				_buffers[i].vertexCount = 0;
			}
		}

		virtual void Draw() {

			xMat33 scale = xMat33(xVec3(_scaleX, 0, 0), xVec3(0, _scaleY, 0), xVec3(0, 0, _scaleZ));

            float userMat[16];
            _userMatrix.getColumnMajor44(userMat);
            glMatrix4x4 scaleMat; scaleMat.scale(_scaleX, _scaleY, _scaleZ);

            _finalWorld.multiply(_finalWorld.getMatrix(), userMat, scaleMat.getMatrix());

			auto pos = _finalWorld.t();

			if (!_renderer->getFrustum()->SphereInFrustum(pos.x, pos.y, pos.z, _AABBRay * std::max(_scaleX, std::max(_scaleY, _scaleZ)))) return;

            _worldmodelview.multiply(_worldmodelview.getMatrix(), _renderer->ModelView.getMatrix(), _finalWorld.getMatrix());

			auto current = _renderer->SimpleShader; 

			glUniformMatrix4fv(current->uniforms[UNI_PROJECTION_MAT], 1, false, _renderer->Projection.getMatrix());
			glUniformMatrix4fv(current->uniforms[UNI_MODELVIEW_WORLD_MAT], 1, false, _worldmodelview.getMatrix());

			for (int i = 0; i < _buffers.size(); i++) { //draw all mesh

				glBindVertexArray(_buffers[i].VAO);
				glDrawElements(GL_TRIANGLES, _buffers[i].faceSize * 3, GL_UNSIGNED_SHORT, (void*)0);
			}

			glBindVertexArray(0);
		}

		void setScale(float s) {
		
			setScale(xVec3(s));
		}

		void setScale(const xVec3 &s) {
		
			_scaleX = s.x;
			_scaleY = s.y;
			_scaleZ = s.z;
		}

		xMat34& getUserMatrix() { return _userMatrix; }
		void setUserMatrix(const xMat34 &userMatrix) { _userMatrix = userMatrix; }

		const std::shared_ptr<SCOpenGLRenderable> &getRenderer() const { return _renderer; }

		float getAABBRay() const { return _AABBRay; }
		float getScaleX() const { return _scaleX; }
		float getScaleY() const { return _scaleY; }
		float getScaleZ() const { return _scaleZ; }

	protected:
		
		std::shared_ptr<SCOpenGLRenderable> _renderer;
		std::vector<SCMeshInfo> _buffers;
		
		glMatrix4x4 _finalWorld; //final world matrix
		glMatrix4x4 _worldmodelview; //world x modelview

		xMat34 _userMatrix;
		float _scaleX;
		float _scaleY;
		float _scaleZ;
		float _AABBRay;
	};
}
