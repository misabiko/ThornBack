#include "WorldData.h"
#include <File.hpp>
#include <algorithm>

using namespace godot;

WorldData::BlockData* WorldData::getBlock(const std::map<std::pair<int, int>, std::vector<BlockData>>::iterator& chunk, const int& x, const int& y, const int& z) {
	return &chunk->second.at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z);
}

WorldData::BlockData* WorldData::getBlock(const std::pair<int, int>& chunk, const int& x, const int& y, const int& z) {
	return &chunks.at(chunk).at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z);
}

unsigned WorldData::getBlockType(const int chunkX, const int chunkY, const int x, const int y, const int z) {
    return chunks.at(std::pair<int, int>(chunkX, chunkY)).at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z).type;
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
			Vector2 n = Vector2((x + chunkX) / 5.0f, (z + chunkY) / 5.0f) / 20 - Vector2(0.5, 0.5);
						
			float e1 = ease((0.5 + noiseE->get_noise_2dv(7.567 * n) / 2) + 0.02, -8.57);
			
			float e2 = ease(((0.244 * noiseE->get_noise_2dv(9.588 * n) * e1) + 1) / 2 + 0.041, -3.36) * 2 - 1;
			
			float e3 = ease(((0.182 * noiseE->get_noise_2dv(25.246 * n) * std::max<double>(e1 - 0.433, 0)) + 1) / 2 , -1.57) * 2 - 1;
			
			float e = e2 + e3;

			y = std::floor(std::clamp<double>((e + 3) * 15.3, 0, 127));

			getBlock(it.first, x, y, z)->set(4, true);

			for (int j = 0; j < y; j++)
				getBlock(it.first, x, j, z)->set(1, true);
		}
}

double WorldData::ease(double s, double curve) {
	if (s < 0)
		s = 0;
	else if (s > 1.0)
		s = 1.0;
	if (curve > 0)	{
		if (curve < 1.0)
			return 1.0 - std::pow(1.0 - s, 1.0 / curve);
		else
			return std::pow(s, curve);
	} else if (curve < 0) {
		//inout ease

		if (s < 0.5)
			return std::pow(s * 2.0, -curve) * 0.5;
		else
			return (1.0 - std::pow(1.0 - (s - 0.5) * 2.0, -curve)) * 0.5 + 0.5;
	} else
		return 0; // no ease (raw)
}

void WorldData::load() {
	Ref<File> saveFile = File::_new();
	saveFile->open("user://world.save", File::READ);

	if (saveFile->is_open()) {
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
		Godot::print("World loaded!");
	}else
		Godot::print("Couldn't open save file!");

	saveFile->close();
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
		
		saveFile->close();
		Godot::print("World saved!");
	}else
		Godot::print("Couldn't create save file!");
}

void WorldData::_init() {
	noiseE.instance();
	noiseM.instance();

	noiseE->set_octaves(1);
	noiseE->set_period(2);

	noiseM->set_octaves(1);
	noiseM->set_period(1);
}

void WorldData::_register_methods() {
    register_method("get_block_type", &WorldData::getBlockType);
}