#ifndef CHUNKLOADER_H
#define CHUNKLOADER_H

#include <Godot.hpp>
#include <Node.hpp>
#include <KinematicBody.hpp>
#include "Chunk.h"
#include <map>
#include <Thread.hpp>

namespace godot {
	class ChunkLoader : public Node {
		GODOT_CLASS(ChunkLoader, Node)
		private:
		Ref<Thread> thread;
		Dictionary blockTypes;
		Ref<OpenSimplexNoise> noise;
		std::map<std::pair<int, int>, Chunk *> chunks;

		public:
		unsigned radius;

		~ChunkLoader();

		static void _register_methods();

		void _init();

		void updateChunkLoadings(Vector2 coords);

		void loadChunk(Variant userdata);
	};
}

#endif