#include "ChunkLoader.h"

using namespace godot;

void ChunkLoader::_register_methods() {
	register_method("update_chunk_loadings", &ChunkLoader::updateChunkLoadings);
	register_method("finish_load_chunk", &ChunkLoader::finishLoadChunk);
	register_method("load_chunk", &ChunkLoader::loadChunk);

	register_property<ChunkLoader, unsigned>("radius", &ChunkLoader::radius, 8);
	register_property<ChunkLoader, Dictionary>("block_types", &ChunkLoader::blockTypes, Dictionary());
	register_property<ChunkLoader, Ref<OpenSimplexNoise>>("noise", &ChunkLoader::noise, Ref<OpenSimplexNoise>());
}

ChunkLoader::~ChunkLoader() {
	for (auto& kv : chunks)
		delete kv.second;

	thread->free();
}

void ChunkLoader::_init() {
	thread = Thread::_new();
}

void ChunkLoader::updateChunkLoadings(const int x, const int y) {
	if (chunks.find(std::pair<int, int>(x, y)) == chunks.end() && !thread->is_active())
		thread->start(this, "load_chunk", Vector2(x, y));
}

void ChunkLoader::finishLoadChunk() {
	Vector2 coords = thread->wait_to_finish().operator Vector2();
	add_child(chunks.at(std::pair<int, int>(coords.x, coords.y)));
}

Variant ChunkLoader::loadChunk(Variant userdata) {
	Vector2 coords = userdata.operator Vector2();

	Chunk* chunk = Chunk::_new();
	chunk->init(coords.x, coords.y, noise, blockTypes);
	chunks.emplace(std::pair<int, int>(coords.x, coords.y), chunk);

	call_deferred("finish_load_chunk");
	return coords;
}
