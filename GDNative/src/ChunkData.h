#ifndef CHUNKDATA_H
#define CHUNKDATA_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <vector>

#define CHUNK_SIZE 16
#define WORLD_HEIGHT 128

namespace godot {
	class ChunkData : public Resource {
		GODOT_CLASS(ChunkData, Resource)

		struct BlockData {
			unsigned type;
			bool rendered;

			bool equals(BlockData* other) {
				return other->rendered == rendered && other->type == type;
			}
		};

		std::vector<BlockData> blocks;

		public:
		BlockData* getBlock(const int& x, const int& y, const int& z);

		String serialize();

		void _init();

		static void _register_methods();
	};
}

#endif