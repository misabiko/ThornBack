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

void BlockLibrary::set_textures(Array textures) {
	this->textures = textures;
	types.emplace_back("Dirt", textures[0]);
	types.emplace_back("Stone", textures[1]);
	types.emplace_back("Cobblestone", textures[2]);
	types.emplace_back("Grass", textures[3]);
	//Color(0, 0.52, 0.125, 1)
	types.emplace_back("Sand", textures[4]);
	types.emplace_back("Gravel", textures[5]);
	types.emplace_back("Wool", textures[6]);
}

Array BlockLibrary::get_textures() {
	return textures;
}

void BlockLibrary::_init() {}

void BlockLibrary::_register_methods() {
	register_property<BlockLibrary, Array>("textures", &BlockLibrary::set_textures, &BlockLibrary::get_textures, Array(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_TYPE_STRING, "17/17:Texture");
}