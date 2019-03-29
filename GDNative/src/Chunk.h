#ifndef MESHER_H
#define MESHER_H

#include <Godot.hpp>
#include <MeshInstance.hpp>
#include <vector>
#include <array>
#include <ArrayMesh.hpp>
#include <OpenSimplexNoise.hpp>
#include <SurfaceTool.hpp>
#include <StaticBody.hpp>
#include <Material.hpp>

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

		struct VoxelFace {
			bool transparent;
			unsigned type;
			Direction side;

			VoxelFace() : transparent(true), type(0), side(SOUTH) {}

			bool equals(VoxelFace* other) {
				return other->transparent == transparent && other->type == type;
			}
		};

		struct SurfaceData {
			std::vector<Vector3> vertices;
			std::vector<Vector3> normals;
			std::vector<Vector2> uvs;
			std::vector<Color> colors;
			std::vector<unsigned> indices;
		};

		std::vector<std::vector<std::vector<VoxelFace*>>> voxels;

		Ref<Material> materials[3];
		SurfaceData surfaces[3];
		std::pair<int, int> coords;
		Dictionary blockTypes;
		Ref<OpenSimplexNoise> noise;

		VoxelFace* getVoxelFace(const unsigned& x, const unsigned& y, const unsigned& z, Direction side);

		public:
		unsigned short CHUNK_SIZE = 16;
		unsigned short WORLD_HEIGHT = 128;
		unsigned short SURFACE_HEIGHT = 60;
		SurfaceTool* surfaceTool;
		StaticBody* staticBody;

		~Chunk();

		static void _register_methods();

		void _init();

		void _ready();

		void _process(float delta);

		void init(int x, int y, Ref<OpenSimplexNoise> noise, Dictionary blockTypes);

		void setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type);

		void clearBlock(const unsigned x, const unsigned y, const unsigned z);

		void greedyMesher();

		void updateMesh();

		void addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, unsigned type, Direction side);

		void addCube(Vector3 origin, Vector3 size, unsigned type);

		void collisionMesher();
	};
}

#endif