#ifndef BLOCKLIBRARY_H
#define BLOCKLIBRARY_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <SpatialMaterial.hpp>
#include <StreamTexture.hpp>
#include <Ref.hpp>
#include <ArrayMesh.hpp>
#include <vector>
#include <map>

namespace godot {
	class BlockLibrary : public Resource {
		GODOT_CLASS(BlockLibrary, Resource)
		
		enum Direction {
			NORTH = 0,
			SOUTH = 1,
			EAST = 2,
			WEST = 3,
			TOP = 4,
			BOTTOM = 5
		};

		std::vector<Ref<SpatialMaterial>> materials;

		public:
		struct SurfaceData {
			Array vertices;
			PoolVector3Array normals;
			PoolVector2Array uvs;
			PoolIntArray indices;

			void clear() {
				vertices.resize(0);
				normals.resize(0);
				uvs.resize(0);
				indices.resize(0);
			}

			void add(const SurfaceData& surface) {
				int vertexCount = vertices.size();
				for (int i = 0; i < vertexCount; i++)
					vertices.append(surface.vertices[i]);
				normals.append_array(surface.normals);
				uvs.append_array(surface.uvs);

				PoolIntArray::Read sIndices(surface.indices.read());
				for (int i = 0; i < surface.indices.size(); i++)
					indices.append(vertexCount + sIndices[i]);
			}
		};

		struct TypeData {
			String name;

			unsigned materials[6];
			bool multiTextured;
			bool opaque;
			bool colored;
			static const int64_t flags = Texture::FLAG_MIPMAPS | Texture::FLAG_REPEAT;

			TypeData(String name, const unsigned& texture) : TypeData(name, texture, texture, texture, texture, texture, texture) {}

			TypeData(String name, const unsigned& texSouth, const unsigned& texNorth, const unsigned& texWest, const unsigned& texEast, const unsigned& texBottom, const unsigned& texTop)
				: name(name), multiTextured(true) {
					materials[0] = texSouth;
					materials[1] = texNorth;
					materials[2] = texWest;
					materials[3] = texEast;
					materials[4] = texBottom;
					materials[5] = texTop;
				}
		};

		private:
		std::vector<TypeData> types;
		Array textures;
		
		public:
		~BlockLibrary();
		
		TypeData& get(unsigned typeId);

		Ref<SpatialMaterial> getMaterial(const unsigned& idx);

		Ref<ArrayMesh> getBlockMesh(const unsigned typeId);

		void setTextures(Array textures);

		Array getTextures();

		void addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, Direction side, std::map<unsigned, BlockLibrary::SurfaceData>::iterator surface);

		void addCube(Vector3 origin, Vector3 size, const BlockLibrary::TypeData& type, std::map<unsigned, BlockLibrary::SurfaceData>& surfaces);

		void _init();

		void _ready();

		static void _register_methods();
	};
}

#endif