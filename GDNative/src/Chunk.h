#ifndef MESHER_H
#define MESHER_H

#include <Godot.hpp>
#include <MeshInstance.hpp>
#include <vector>
#include <array>
#include <ArrayMesh.hpp>
#include <OpenSimplexNoise.hpp>
#include <SurfaceTool.hpp>

namespace godot {
	class Chunk : public MeshInstance {
		GODOT_CLASS(Chunk, MeshInstance)
		private:
		enum Direction {
			SOUTH,
			NORTH,
			EAST,
			WEST,
			TOP,
			BOTTOM
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

		std::vector<std::vector<std::vector<VoxelFace*>>> voxels;

		std::vector<Vector3> vertices;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;
		std::vector<unsigned> indices;

		VoxelFace* getVoxelFace(const unsigned& x, const unsigned& y, const unsigned& z, Direction side);

		public:
		unsigned short CHUNK_SIZE = 16;
		unsigned short WORLD_HEIGHT = 128;
		unsigned short SURFACE_HEIGHT = 60;
		Dictionary blockTypes;
		Ref<OpenSimplexNoise> noise;
		SurfaceTool* surfaceTool;

		~Chunk();

		static void _register_methods();

		void _init();

		void init(int x, int y);

		void newBlock(const int x, const int y, const int z, int type);

		void greedyMesher();

		void updateMesh();

		void addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, VoxelFace* voxel, bool back_face);
	};
}

#endif