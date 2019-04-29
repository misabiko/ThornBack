#include "BlockLibrary.h"
#include <EditorPlugin.hpp>

using namespace godot;

BlockLibrary::~BlockLibrary() {
	for (int i = 0; i < textures.size(); i++)
		static_cast<Ref<StreamTexture>>(textures[i])->unreference();
}

BlockLibrary::TypeData& BlockLibrary::get(unsigned idx) {
	return types[idx - 1];
}

Ref<SpatialMaterial> BlockLibrary::getMaterial(const unsigned& idx) {
	return materials[idx];
}

void BlockLibrary::set_textures(Array textures) {
	this->textures = textures;

	materials.resize(textures.size());
	for (int i = 0; i < textures.size(); i++) {
		materials[i].instance();
		materials[i]->set_texture(SpatialMaterial::TEXTURE_ALBEDO, textures[i]);
	}

	types.emplace_back("Dirt", 0);					//1
	types.emplace_back("Stone", 1);					//2
	types.emplace_back("Cobblestone", 2);			//3
	types.emplace_back("Grass", 4, 4, 4, 4, 0, 3);	//4
	//Color(0, 0.52, 0.125, 1)
	types.emplace_back("Sand", 5);					//5
	types.emplace_back("Gravel", 6);				//6
	types.emplace_back("Wool", 7);					//7
}

Array BlockLibrary::get_textures() {
	return textures;
}

void BlockLibrary::_init() {}

void BlockLibrary::_register_methods() {
	register_property<BlockLibrary, Array>("textures", &BlockLibrary::set_textures, &BlockLibrary::get_textures, Array(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_TYPE_STRING, "17/17:Texture");
}