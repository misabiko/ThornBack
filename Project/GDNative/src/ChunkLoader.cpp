#include "ChunkLoader.h"
#include <cmath>
#include <OS.hpp>
#include <string>
#include <Resource.hpp>
#include <MainLoop.hpp>

using namespace godot;

void ChunkLoader::_register_methods() {
	register_method("update_chunk_loadings", &ChunkLoader::updateChunkLoadings);
	register_method("load_chunk", &ChunkLoader::loadChunk);
	register_method("_process", &ChunkLoader::_process);
	register_method("init", &ChunkLoader::init);
	register_method("get_backlog_size", &ChunkLoader::getBacklogSize);
	register_method("save", &ChunkLoader::save);
	register_method("load", &ChunkLoader::load);
	register_method("_notification", &ChunkLoader::_notification);

	register_property<ChunkLoader, Array>("block_types", &ChunkLoader::blockTypes, Array());
	register_property<ChunkLoader, int>("radius", &ChunkLoader::setRadius, &ChunkLoader::getRadius, 8);
	register_property<ChunkLoader, int>("delay", &ChunkLoader::delay, 100);
}

ChunkLoader::~ChunkLoader() {
	if (thread->is_active())
		thread->wait_to_finish();

	worldData->unreference();
	worldData->free();
}

void ChunkLoader::_init() {
	thread.instance();
	mutex.instance();
	worldData = WorldData::_new();
	
	delay = 100;
	radius = 8;
	radiusSquared = 2 * radius * radius;
	loadingComp = [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
		return a.first * a.first + a.second * a.second < b.first * b.first + b.second * b.second;
	};
	quitRequested = false;
}

void ChunkLoader::init() {
	worldData->load();

	float backlogSize = 256;
	float numLoaded = 0;

	for (int x = -8; x < 8; x++) {
		for (int y = -8; y < 8; y++)	{
			Chunk* chunk = Chunk::_new();
			chunk->init(x, y, worldData, blockTypes);
			add_child(chunk);
			chunks.emplace(std::pair<int, int>(x, y), chunk);
			numLoaded++;
		}

		Godot::print((std::to_string(static_cast<int>((numLoaded / backlogSize) * 100)) + "%").c_str());
	}
	updateChunkLoadings(Vector2(0, 0));
}

void ChunkLoader::_process(float delta) {
	if (!loadingBacklog.empty() && !thread->is_active())
		thread->start(this, "load_chunk");
}

void ChunkLoader::updateChunkLoadings(Vector2 coords) {
	mutex->lock();
	lastCoords = std::pair<int, int>(coords.x, coords.y);
	loadingBacklog.clear();

	for (int x = lastCoords.first - radius; x < lastCoords.first + radius; x++)
		for (int y = lastCoords.second - radius; y < lastCoords.second + radius; y++) {
			if (
				std::pow(x - lastCoords.first, 2) + std::pow(y - lastCoords.second, 2) <= radiusSquared &&
				chunks.find(std::pair<int, int>(x, y)) == chunks.end()
			)
				loadingBacklog.emplace_back(x, y);
		}
	
	if (!loadingBacklog.empty())
		loadingBacklog.sort(loadingComp);
	
	mutex->unlock();
}

void ChunkLoader::loadChunk(Variant userdata) {
	mutex->lock();
	bool hasBacklog = !loadingBacklog.empty();

	while (hasBacklog && !quitRequested) {
		std::pair<int, int> coords = loadingBacklog.front();
		loadingBacklog.pop_front();
		mutex->unlock();

		Chunk *chunk = Chunk::_new();
		chunk->init(coords.first, coords.second, worldData, blockTypes);
		call_deferred("add_child", chunk);
		chunks.emplace(coords, chunk);

		mutex->lock();

		if (hasBacklog = !loadingBacklog.empty())
			OS::get_singleton()->delay_msec(std::min<int>((std::pow(loadingBacklog.front().first - lastCoords.first, 2) + std::pow(loadingBacklog.front().second - lastCoords.second, 2)) * 10, 500));
	}
	
	mutex->unlock();

	thread->call_deferred("wait_to_finish");
}

void ChunkLoader::setRadius(int radius) {
	this->radius = radius;
	radiusSquared = 2 * radius * radius;
}

int ChunkLoader::getRadius() {
	return radius;
}

int ChunkLoader::getBacklogSize() {
	return loadingBacklog.size();
}

void ChunkLoader::save() {
	worldData->save();
}

void ChunkLoader::load() {
	worldData->load();
}

void ChunkLoader::_notification(const int what) {
	if (what == MainLoop::NOTIFICATION_WM_QUIT_REQUEST)
		quitRequested = true;
}
