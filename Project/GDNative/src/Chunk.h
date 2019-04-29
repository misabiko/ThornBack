#ifndef CHUNK_H
#define CHUNK_H

#include <Godot.hpp>
#include <MeshInstance.hpp>
#include <map>
#include <array>
#include <ArrayMesh.hpp>
#include <StaticBody.hpp>
#include <Material.hpp>
#include "WorldData.h"
#include "BlockLibrary.h"

namespace godot {
	class Chunk : public MeshInstance {
		GODOT_CLASS(Chunk, MeshInstance)
		
		enum Direction {
			NORTH = 0,
			SOUTH = 1,
			EAST = 2,
			WEST = 3,
			TOP = 4,
			BOTTOM = 5
		};

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
		};

		std::map<unsigned, SurfaceData> surfaces;
		std::pair<int, int> coords;
		Ref<BlockLibrary> blockLibrary;

		public:
		Ref<WorldData> worldData;
		StaticBody* staticBody;

		static void _register_methods();

		void _init();

		void _ready();

		void _process(float delta);

		void init(int x, int y, Ref<WorldData> worldData, Ref<BlockLibrary> blockLibrary);

		void setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type);

		void clearBlock(const unsigned x, const unsigned y, const unsigned z);

		void updateMesh();

		void addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, unsigned type, Direction side);

		void addCube(Vector3 origin, Vector3 size, const BlockLibrary::TypeData &type);

		void collisionMesher();
	};
}

#endif