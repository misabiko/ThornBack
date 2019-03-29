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
		Thread* thread;

		public:
		unsigned radius;
		Dictionary blockTypes;
		Ref<OpenSimplexNoise> noise;
		std::map<std::pair<int, int>, Chunk*> chunks;

		~ChunkLoader();

		static void _register_methods();

		void _init();

		void updateChunkLoadings(const int x, const int y);

		void finishLoadChunk();

		void loadChunk(Variant userdata);
	};
}

#endif