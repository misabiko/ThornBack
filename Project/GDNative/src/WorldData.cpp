#include "WorldData.h"
#include <string>

using namespace godot;

WorldData::BlockData* WorldData::getBlock(const std::map<std::pair<int, int>, std::vector<BlockData>>::iterator& chunk, const int& x, const int& y, const int& z) {
	return &chunk->second.at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z);
}

WorldData::BlockData* WorldData::getBlock(const std::pair<int, int>& chunk, const int& x, const int& y, const int& z) {
	return &chunks.at(chunk).at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z);
}

void WorldData::tryInit(const std::pair<int, int>& chunk) {
	if (chunks.find(chunk) != chunks.end())
		return;

	auto it = chunks.emplace(chunk, std::vector<BlockData>(CHUNK_VOLUME));
	int chunkX = chunk.first * CHUNK_SIZE;
	int chunkY = chunk.second * CHUNK_SIZE;

	int y = 0;
	for (int x = 0; x < CHUNK_SIZE; x++)
		for (int z = 0; z < CHUNK_SIZE; z++) {
			y = std::floor((noise->get_noise_2d((x + chunkX) / 5.0f, (z + chunkY) / 5.0f) / 2.0f + 0.5f) * 60.0f);
	
			getBlock(it.first, x, y, z)->set(1, true);

			for (int j = 0; j < y; j++)
				getBlock(it.first, x, j, z)->set(2, true);
		}
}

void WorldData::load() {

}

void WorldData::save() {
	for (auto const& [coords, blocks] : chunks) {
		std::string str;
		unsigned numSameBlocks = 1;
		auto it = blocks.begin();
		unsigned currType = (it++)->type;

		for (; it != blocks.end(); it++)
			if (it->type == currType)
				numSameBlocks++;
			else {
				str += std::to_string(currType) + ":" + std::to_string(numSameBlocks) + ",";
				currType = it->type;
				numSameBlocks = 0;
			}
	}
}

void WorldData::_init() {
	noise.instance();
	
	noise->set_seed(43);
	noise->set_octaves(4);
	noise->set_period(20.0);
	noise->set_persistence(0.8);
}

void WorldData::_register_methods() {
	register_method("save", &WorldData::save);
}