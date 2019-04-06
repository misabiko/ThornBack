#ifndef CHUNK_H
#define CHUNK_H

#include <Godot.hpp>
#include <MeshInstance.hpp>
#include <vector>
#include <array>
#include <ArrayMesh.hpp>
#include <OpenSimplexNoise.hpp>
#include <SurfaceTool.hpp>
#include <StaticBody.hpp>
#include <Material.hpp>
#include "ChunkData.h"

namespace godot {
	class Chunk : public MeshInstance {
		GODOT_CLASS(Chunk, MeshInstance)
		private:
		enum Direction {
			NORTH = 0,
			SOUTH = 1,
			EAST = 2,
			WEST = 3,
			TOP = 4,
			BOTTOM = 5
		};

		struct SurfaceData {
			std::vector<Vector3> vertices;
			std::vector<Vector3> normals;
			std::vector<Vector2> uvs;
			std::vector<Color> colors;
			std::vector<unsigned> indices;
		};

		SurfaceData surfaces[3];
		std::pair<int, int> coords;
		Array blockTypes;
		Ref<OpenSimplexNoise> noise;

		public:
		Ref<ChunkData> chunkData;
		Ref<SurfaceTool> surfaceTool;
		StaticBody* staticBody;

		static void _register_methods();

		void _init();

		void _ready();

		void _process(float delta);

		void init(int x, int y, Ref<OpenSimplexNoise> noise, Array blockTypes);

		void setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type);

		void clearBlock(const unsigned x, const unsigned y, const unsigned z);

		void updateMesh();

		void addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, unsigned type, Direction side);

		void addCube(Vector3 origin, Vector3 size, unsigned type);

		void collisionMesher();
	};
}

#endif