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

		struct TypeData {
			String name;
			Ref<SpatialMaterial> material;
			bool opaque;
			bool colored;
			static const int64_t flags = Texture::FLAG_MIPMAPS | Texture::FLAG_REPEAT;

			TypeData(String name, Ref<StreamTexture> texture, const bool& opaque = true, const bool& colored = false) {
				this->name = name;
				this->opaque = opaque;
				this->colored = colored;

				material.instance();
				material->set_texture(SpatialMaterial::TEXTURE_ALBEDO, texture);
			}
		};

		std::vector<TypeData> types;
		Array textures;
		
		public:
		~BlockLibrary();
		
		TypeData& get(unsigned idx);

		void set_textures(Array textures);

		Array get_textures();

		void _init();

		void _ready();

		static void _register_methods();
	};
}

#endif