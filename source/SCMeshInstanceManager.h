//
//  MeshInstanceManager.h
//  SagaTanks
//
//  Created by Marcin Pędzimąż on 01.05.2013.
//  Copyright (c) 2013 Marcin Pędzimąż. All rights reserved.
//

#pragma once

#include "glTextureLoader.hpp"
#include "SCFastMesh.h"
#include "SCFastAnimatedMesh.h"
#include "SCFastCutoffMesh.h"
#include "SCFastOceanMesh.h"
#include "SCFastWaterMesh.h"

namespace sc {

	typedef SC_ENUM(uint32_t, SCMeshType) {

		SCMeshTypeAnimated,
		SCMeshTypeOcean,
		SCMeshTypeWater,
		SCMeshTypeCutOff,
		SCMeshTypeNormal
	};

	class SCMeshInstance final {

	public:

		SCMeshInstance(const std::shared_ptr<SCRendererInterface> &renderer) : _renderer(renderer) {}
		~SCMeshInstance() noexcept = default;

		std::shared_ptr<SCFastMesh> GetMesh(const std::string & name, const std::string &path, SCMeshType type) {

			auto i = _list.find(name + std::to_string(type));

			if (i != _list.end()) {

				return i->second;
			}

			auto filePath = path + name + ".gmf";

			if (!filePath.empty()) {

				std::shared_ptr<SCFastMesh> s = nullptr;

				switch (type) {
				
				case SCMeshTypeAnimated: {

					s = std::make_shared<SCFastAnimatedMesh>(filePath, _renderer);
					break;
				}

				case SCMeshTypeOcean: {
					
					auto ptr = std::make_shared<SCFastOceanMesh>(filePath, _renderer);

					ptr->caustic = textureLoader.loadFile(path + "textures/mesh_greece-sea-surface.png", GL_LINEAR, 0, GL_REPEAT, false);
					ptr->causticSecond = textureLoader.loadFile(path + "textures/mesh_greece-sea-surface_rotated.png", GL_LINEAR, 0, GL_REPEAT, false);

					s = ptr;

					break;
				}

				case SCMeshTypeWater: {
					auto ptr = std::make_shared<SCFastWaterMesh>(filePath, _renderer);

					ptr->tex = textureLoader.loadFile("textures/fx_water-stream.png", GL_LINEAR, 0, GL_REPEAT, false);

					s = ptr;

					break;
				}

				case SCMeshTypeCutOff: {
					s = std::make_shared<SCFastCutOffMesh>(filePath, _renderer);
					break;
				}

				case SCMeshTypeNormal: {
					s = std::make_shared<SCFastMesh>(filePath, _renderer);
					break;
				}

				default:
					break;
				}

				_list[name] = s;

				return s;
			}

			return nullptr;
		}

	private:

		std::map< std::string, std::shared_ptr<SCFastMesh>> _list;
		std::shared_ptr<SCRendererInterface> _renderer;
	};
}


