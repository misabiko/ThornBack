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

		std::map<unsigned, BlockLibrary::SurfaceData> surfaces;
		std::pair<int, int> coords;
		Ref<BlockLibrary> blockLibrary;

		public:
		Ref<WorldData> worldData;
		StaticBody* staticBody;
		bool mustRemesh;
		static bool wireframe;

		static void _register_methods();

		void _init();

		void _ready();

		void _process(float delta);

		void init(int x, int y, Ref<WorldData> worldData, Ref<BlockLibrary> blockLibrary);

		void setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type);

		void clearBlock(const unsigned x, const unsigned y, const unsigned z);

		void updateMesh();

		void collisionMesher();

		void setWireframe(bool wireframe);

		bool getWireframe();
	};
}

#endif