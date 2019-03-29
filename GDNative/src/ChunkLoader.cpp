#include "ChunkLoader.h"

using namespace godot;

void ChunkLoader::_register_methods() {
	register_method("update_chunk_loadings", &ChunkLoader::updateChunkLoadings);
	register_method("load_chunk", &ChunkLoader::loadChunk);

	register_property<ChunkLoader, unsigned>("radius", &ChunkLoader::radius, 8);
	register_property<ChunkLoader, Dictionary>("block_types", &ChunkLoader::blockTypes, Dictionary());
	register_property<ChunkLoader, Ref<OpenSimplexNoise>>("noise", &ChunkLoader::noise, Ref<OpenSimplexNoise>());
}

ChunkLoader::~ChunkLoader() {
	for (auto& kv : chunks)
		delete kv.second;

	if (thread->is_active())
		thread->wait_to_finish();
	thread.unref();
}

void ChunkLoader::_init() {
	thread.instance();
}

void ChunkLoader::updateChunkLoadings(Vector2 coords) {
	if (chunks.find(std::pair<int, int>(coords.x, coords.y)) == chunks.end() && !thread->is_active())
		thread->start(this, "load_chunk", coords);
}

void ChunkLoader::loadChunk(Variant userdata) {
	Vector2 coords = userdata.operator Vector2();

	Chunk* chunk = Chunk::_new();
	chunk->init(coords.x, coords.y, noise, blockTypes);
	call_deferred("add_child", chunk);
	chunks.emplace(std::pair<int, int>(coords.x, coords.y), chunk);

	thread->call_deferred("wait_to_finish");
}
