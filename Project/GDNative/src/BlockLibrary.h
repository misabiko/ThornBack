#ifndef BLOCKLIBRARY_H
#define BLOCKLIBRARY_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <SpatialMaterial.hpp>
#include <StreamTexture.hpp>
#include <vector>

namespace godot {
	class BlockLibrary : public Resource {
		GODOT_CLASS(BlockLibrary, Resource)

		std::vector<Ref<SpatialMaterial>> materials;

		public:
		struct TypeData {
			String name;

			unsigned materials[6];
			bool multiTextured;
			bool opaque;
			bool colored;
			static const int64_t flags = Texture::FLAG_MIPMAPS | Texture::FLAG_REPEAT;

			TypeData(String name, const unsigned& texture) : TypeData(name, texture, texture, texture, texture, texture, texture) {}

			TypeData(String name, const unsigned& texSouth, const unsigned& texNorth, const unsigned& texWest, const unsigned& texEast, const unsigned& texBottom, const unsigned& texTop)
				: name(name), multiTextured(true) {
					materials[0] = texSouth;
					materials[1] = texNorth;
					materials[2] = texWest;
					materials[3] = texEast;
					materials[4] = texBottom;
					materials[5] = texTop;
				}
		};

		private:
		std::vector<TypeData> types;
		Array textures;
		
		public:
		~BlockLibrary();
		
		TypeData& get(unsigned idx);

		Ref<SpatialMaterial> getMaterial(const unsigned& idx);

		void set_textures(Array textures);

		Array get_textures();

		void _init();

		void _ready();

		static void _register_methods();
	};
}

#endif