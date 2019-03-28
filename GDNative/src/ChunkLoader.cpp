#include "ChunkLoader.h"

using namespace godot;

void ChunkLoader::_register_methods() {
	register_method("on_exit_chunk", &ChunkLoader::onExitChunk);

	register_property<ChunkLoader, unsigned>("radius", &ChunkLoader::radius, 8);
	register_property<ChunkLoader, Dictionary>("block_types", &ChunkLoader::blockTypes, Dictionary());
	register_property<ChunkLoader, Ref<OpenSimplexNoise>>("noise", &ChunkLoader::noise, Ref<OpenSimplexNoise>());
}

ChunkLoader::~ChunkLoader() {
	for (auto& kv : chunks)
		delete kv.second;
}

void ChunkLoader::_init() {}

void ChunkLoader::onExitChunk(const int x, const int y) {
	Chunk* chunk = chunks[std::pair<int, int>(x, y)];

	if (!chunk) {
		chunk = Chunk::_new();
		chunk->blockTypes = blockTypes;
		chunk->noise = noise;
		chunk->init(x, y);
		add_child(chunk);
	}
}
