#include "ItemLibrary.h"
#include <Image.hpp>
#include <Texture.hpp>

using namespace godot;

void ItemLibrary::_register_methods() {
    register_property<ItemLibrary, Array>("textures", &ItemLibrary::setTextures, &ItemLibrary::getTextures, Array(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_TYPE_STRING, "17/17:Texture");
	register_method("get_item_mesh", &ItemLibrary::getItemMesh);
}

void ItemLibrary::_init() {}

Ref<ArrayMesh> ItemLibrary::getItemMesh(const unsigned typeId) {
	Ref<ArrayMesh> mesh = ArrayMesh::_new();
	MeshData data;

	Ref<Image> img = ((Ref<Texture>)textures[typeId])->get_data();
	img->lock();
	for (int y = 0; y < img->get_height(); y++)
		for (int x = 0; x < img->get_width(); x++) {
			Color pixel = img->get_pixel(x, y);
			if (pixel.a) {
				addQuad(data, pixel, x, y, true);
			}
		}
	img->unlock();

	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, data.getArrays());
	return mesh;
}

void ItemLibrary::addQuad(MeshData& data, const Color& color, const int& x, const int& y, const bool& front) {
	Vector3 normal = Vector3(0, 0, static_cast<real_t>(front ? -1 : 1));
	for (int i = 0; i < 4; i++) {
		data.normals.push_back(normal);
		data.colors.push_back(color);
	}

	/* {2,0,1, 1,3,2} */
	unsigned vertexCount = data.vertices.size();
	data.indices.push_back(vertexCount + 2);
	data.indices.push_back(vertexCount);
	data.indices.push_back(vertexCount + 1);

	data.indices.push_back(vertexCount + 1);
	data.indices.push_back(vertexCount + 3);
	data.indices.push_back(vertexCount + 2);

	data.vertices.push_back(Vector3(static_cast<real_t>((x - 8) * 0.0625), static_cast<real_t>((8 - y) * 0.0625), static_cast<real_t>(-0.0625 / 2))); //TOP_LEFT
	data.vertices.push_back(Vector3(static_cast<real_t>((x - 7) * 0.0625), static_cast<real_t>((8 - y) * 0.0625), static_cast<real_t>(-0.0625 / 2))); //TOP_RIGHT
	data.vertices.push_back(Vector3(static_cast<real_t>((x - 8) * 0.0625), static_cast<real_t>((7 - y) * 0.0625), static_cast<real_t>(-0.0625 / 2))); //BOTTOM_LEFT
	data.vertices.push_back(Vector3(static_cast<real_t>((x - 7) * 0.0625), static_cast<real_t>((7 - y) * 0.0625), static_cast<real_t>(-0.0625 / 2))); //BOTTOM_RIGHT
}

void ItemLibrary::setTextures(Array textures) {
	this->textures = textures;
}

Array ItemLibrary::getTextures() {
	return textures;
}