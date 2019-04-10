#include "WorldData.h"
#include <File.hpp>

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
	Ref<File> saveFile = File::_new();
	saveFile.instance();
	saveFile->open("user://world.save", File::READ);

	std::pair<int, int> coords;
	unsigned numSameBlocks;
	unsigned currType;

	while (!saveFile->eof_reached()) {
		coords.first = saveFile->get_32();
		coords.second = saveFile->get_32();

		chunks.emplace(coords, std::vector<BlockData>(CHUNK_VOLUME));
		auto it = chunks.at(coords).begin();

		while (numSameBlocks = saveFile->get_32()) {
			currType = saveFile->get_8();

			for (int i = 0; i < numSameBlocks; i++, it++)
				it->set(currType, currType != 0);
		}
	}

	saveFile->close();
	Godot::print("World loaded!");
}

void WorldData::save() {
	Ref<File> saveFile = File::_new();
	saveFile.instance();
	saveFile->open("user://world.save", File::WRITE_READ);

	if (saveFile->is_open()) {
		for (auto const& [coords, blocks] : chunks) {
			saveFile->store_32(coords.first);
			saveFile->store_32(coords.second);

			unsigned numSameBlocks = 1;
			auto it = blocks.begin();
			unsigned lastType = (it++)->type;

			for (; it != blocks.end(); it++)
				if (it->type == lastType)
					numSameBlocks++;
				else {
					saveFile->store_32(numSameBlocks);
					saveFile->store_8(lastType);
					lastType = it->type;
					numSameBlocks = 1;
				}
			
			saveFile->store_32(0);
		}
	}else
		Godot::print("Couldn't create save file!");

	saveFile->close();
	Godot::print("World saved!");
}

void WorldData::_init() {
	noise.instance();
	
	noise->set_seed(43);
	noise->set_octaves(4);
	noise->set_period(20.0);
	noise->set_persistence(0.8);
}

void WorldData::_register_methods() {}