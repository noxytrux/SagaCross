#pragma once 

#include "Core-pch.hpp"
#include "SCFastMesh.h"

namespace sc {

	class SCRandomMeshPoint {
	public:

		struct buff {
			int vertCount;
			VertexData *ActualData;
			int faceSize;
			GLushort *Fdata;

			buff() {
				vertCount = 0;
				faceSize = 0;
				ActualData = NULL;
				Fdata = NULL;
			}

			~buff() {
				vertCount = 0;
				faceSize = 0;
				delete[] ActualData;
				delete[] Fdata;
			}
		};

		buff *Buffers;
		int BuffersCount;

		float ScaleX;
		float ScaleY;
		float ScaleZ;

		SCRandomMeshPoint(const std::string &filename)
		{
			ScaleX = 0.05f;
			ScaleY = 0.05f;
			ScaleZ = 0.05f;

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
					BuffersCount = nmesh;
					Buffers = new buff[BuffersCount];

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

						fread(&nvert, sizeof(DWORD), 1, f);
						fread(&nface, sizeof(DWORD), 1, f);

						Buffers[mindex].vertCount = nvert;
						Buffers[mindex].ActualData = new VertexData[nvert];
						Buffers[mindex].faceSize = nface;
						Buffers[mindex].Fdata = new GLushort[3 * Buffers[mindex].faceSize];

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

							Buffers[mindex].ActualData[index].position = xVec3(x, y, z);
							Buffers[mindex].ActualData[index].coord = xVec2(u, v);
							Buffers[mindex].ActualData[index].normal = xVec3(nx, ny, nz);
							Buffers[mindex].ActualData[index].tangent = xVec3(0, 0, 0);
						}

						for (index = 0; index < nface; index++)
						{

							fread(&v1, sizeof(DWORD), 1, f);
							fread(&v2, sizeof(DWORD), 1, f);
							fread(&v3, sizeof(DWORD), 1, f);
							fread(&nx, sizeof(FLOAT), 1, f);
							fread(&ny, sizeof(FLOAT), 1, f);
							fread(&nz, sizeof(FLOAT), 1, f);

							Buffers[mindex].Fdata[index * 3] = v1;
							Buffers[mindex].Fdata[index * 3 + 1] = v2;
							Buffers[mindex].Fdata[index * 3 + 2] = v3;
						}

					}

					fclose(f);

				}
			}
		}

		~SCRandomMeshPoint()
		{

			if (Buffers) {

				delete[] Buffers;
				Buffers = NULL;
			}
		}

		xVec3 GetRandomPoint() {

			int t = rand() % Buffers[0].faceSize;

			int v1 = Buffers[0].Fdata[t * 3];
			int v2 = Buffers[0].Fdata[t * 3 + 1];
			int v3 = Buffers[0].Fdata[t * 3 + 2];

			const xVec3 & vA = xVec3(Buffers[0].ActualData[v1].position.get());
			const xVec3 & vB = xVec3(Buffers[0].ActualData[v2].position.get());
			const xVec3 & vC = xVec3(Buffers[0].ActualData[v3].position.get());

			float randU = rand() / float(RAND_MAX);
			float randV = rand() / float(RAND_MAX);

			xVec3 out = vB + (vA - vB) * randU + (vC - vB) * randV * (1.0 - randU);

			return out;
		}

	};

}