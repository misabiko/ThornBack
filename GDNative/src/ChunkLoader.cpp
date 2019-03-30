#include "ChunkLoader.h"
#include <cmath>
#include <OS.hpp>

using namespace godot;

void ChunkLoader::_register_methods() {
	register_method("update_chunk_loadings", &ChunkLoader::updateChunkLoadings);
	register_method("load_chunk", &ChunkLoader::loadChunk);
	register_method("_ready", &ChunkLoader::_ready);
	register_method("_process", &ChunkLoader::_process);

	register_property<ChunkLoader, Dictionary>("block_types", &ChunkLoader::blockTypes, Dictionary());
	register_property<ChunkLoader, Ref<OpenSimplexNoise>>("noise", &ChunkLoader::noise, Ref<OpenSimplexNoise>());
	register_property<ChunkLoader, unsigned>("radius", &ChunkLoader::setRadius, &ChunkLoader::getRadius, 8);
	register_property<ChunkLoader, unsigned>("delay", &ChunkLoader::delay, 100);
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
	mutex.instance();
	radius = 8;
	delay = 100;
	radiusSquared = 2 * radius * radius;
	loadingComp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
		return a.first * a.first + a.second * a.second < b.first * b.first + b.second * b.second;
	};
}

void ChunkLoader::_ready() {
	updateChunkLoadings(Vector2(0, 0));
}

void ChunkLoader::_process(float delta) {
	if (!loadingBacklog.empty() && !thread->is_active())
		thread->start(this, "load_chunk");
}

void ChunkLoader::updateChunkLoadings(Vector2 coords) {
	loadingBacklog.clear();
	
	for (int x = coords.x - radius; x < coords.x + radius; x++)
		for (int y = coords.y - radius; y < coords.y + radius; y++)
			if (
				std::pow(x - coords.x, 2) + std::pow(y - coords.y, 2) <= radiusSquared &&
				chunks.find(std::pair<int, int>(x, y)) == chunks.end()
			)
				loadingBacklog.emplace_back(x, y);
	
	loadingBacklog.sort(loadingComp);
}

void ChunkLoader::loadChunk(Variant userdata) {
	while (!loadingBacklog.empty()) {
		std::pair<int, int> coords = loadingBacklog.front();
		loadingBacklog.pop_front();

		Chunk *chunk = Chunk::_new();
		chunk->init(coords.first, coords.second, noise, blockTypes);
		call_deferred("add_child", chunk);
		chunks.emplace(coords, chunk);
		OS::get_singleton()->delay_msec(delay);
	}

	thread->call_deferred("wait_to_finish");
}

void ChunkLoader::setRadius(unsigned radius) {
	this->radius = radius;
	radiusSquared = 2 * radius * radius;
}

unsigned ChunkLoader::getRadius() {
	return radius;
}
