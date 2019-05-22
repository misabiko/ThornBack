#ifndef WORLDDATA_H
#define WORLDDATA_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <Ref.hpp>
#include <OpenSimplexNoise.hpp>
#include <map>
#include <vector>

#define CHUNK_SIZE 16
#define WORLD_HEIGHT 128
#define CHUNK_VOLUME CHUNK_SIZE * WORLD_HEIGHT * CHUNK_SIZE

namespace godot {
	class WorldData : public Resource {
		GODOT_CLASS(WorldData, Resource)

		struct BlockData {
			unsigned type;
			bool rendered;

			void set(const unsigned& type, const bool& rendered) {
				this->type = type;
				this->rendered = rendered;
			}

			bool equals(BlockData* other) {
				return other->rendered == rendered && other->type == type;
			}
		};

		std::map<std::pair<int, int>, std::vector<BlockData>> chunks;
		Ref<OpenSimplexNoise> noiseE;
		Ref<OpenSimplexNoise> noiseM;

		BlockData* getBlock(const std::map<std::pair<int, int>, std::vector<BlockData>>::iterator& chunk, const int& x, const int& y, const int& z);

		public:		
		BlockData* getBlock(const std::pair<int, int>& chunk, const int& x, const int& y, const int& z);

		unsigned getBlockType(const int chunkX, const int chunkY, const int x, const int y, const int z);

		void tryInit(const std::pair<int, int>& chunk);

		double ease(double s, double curve);

		void load();

		void save();

		void _init();

		static void _register_methods();
	};
}

#endif