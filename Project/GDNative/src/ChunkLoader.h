#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <Godot.hpp>
#include <Node.hpp>
#include <KinematicBody.hpp>
#include "Chunk.h"
#include <map>
#include <Thread.hpp>
#include <Mutex.hpp>
#include <list>
#include <functional>
#include "WorldData.h"
#include "BlockLibrary.h"

namespace godot {
	class ChunkLoader : public Node {
		GODOT_CLASS(ChunkLoader, Node)
		private:
		Ref<Thread> thread;
		Ref<Mutex> mutex;

		std::map<std::pair<int, int>, Chunk*> chunks;
		WorldData* worldData;
		Ref<BlockLibrary> blockLibrary;
		std::list<std::pair<int, int>> loadingBacklog;
		std::function<bool(std::pair<int, int>, std::pair<int, int>)> loadingComp;

		int radiusSquared;
		int radius;
		int initialRadius;
		std::pair<int, int> lastCoords;
		bool quitRequested;

		public:
		int delay;

		static void _register_methods();

		~ChunkLoader();

		void _init();

		void init();

		void _process(float delta);

		void updateChunkLoadings(Vector2 coords);

		void loadChunk(Variant userdata);

		void setRadius(int radius);

		int getRadius();

		int getBacklogSize();

		void save();

		void load();

		void _notification(const int what);
	};
}

#endif