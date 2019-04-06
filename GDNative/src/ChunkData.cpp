#include "ChunkData.h"
#include <string>

using namespace godot;

ChunkData::BlockData* ChunkData::getBlock(const int& x, const int& y, const int& z) {
	return &blocks.at(x + CHUNK_SIZE * y + CHUNK_SIZE * WORLD_HEIGHT * z);
}

String ChunkData::serialize() {
	std::string str;
	unsigned numSameBlocks = 1;
	auto it = blocks.begin();
	unsigned currType = (it)->type;

	for (; it != blocks.end(); it++)
		if (it->type == currType)
			numSameBlocks++;
		else {
			str += std::to_string(currType) + ":" + std::to_string(numSameBlocks) + ",";
			currType = it->type;
			numSameBlocks = 0;
		}

	return String(str.c_str());
}

void ChunkData::_init() {
	blocks.resize(CHUNK_SIZE * WORLD_HEIGHT * CHUNK_SIZE);
}

void ChunkData::_register_methods() {
	register_method("serialize", &ChunkData::serialize);
}