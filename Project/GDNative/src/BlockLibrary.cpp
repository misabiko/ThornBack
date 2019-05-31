#include "BlockLibrary.h"
#include <EditorPlugin.hpp>
#include <array>

using namespace godot;

BlockLibrary::~BlockLibrary() {
	for (int i = 0; i < textures.size(); i++)
		static_cast<Ref<StreamTexture>>(textures[i])->unreference();
}

void BlockLibrary::_register_methods() {
	register_property<BlockLibrary, Array>("textures", &BlockLibrary::setTextures, &BlockLibrary::getTextures, Array(), GODOT_METHOD_RPC_MODE_DISABLED, GODOT_PROPERTY_USAGE_DEFAULT, GODOT_PROPERTY_HINT_TYPE_STRING, "17/17:Texture");
	register_method("get_material", &BlockLibrary::getMaterial);
	register_method("get_block_mesh", &BlockLibrary::getBlockMesh);
}

BlockLibrary::TypeData& BlockLibrary::get(unsigned typeId) {
	return types[typeId - 1];
}

Ref<SpatialMaterial> BlockLibrary::getMaterial(const unsigned& idx) {
	return materials[idx];
}

Ref<ArrayMesh> BlockLibrary::getBlockMesh(const unsigned typeId) {
	Ref<ArrayMesh> mesh = ArrayMesh::_new();
	std::map<unsigned, SurfaceData> surfaces;
	addCube(Vector3(-0.5, -0.5, -0.5), Vector3(1, 1, 1), types[typeId - 1], surfaces);

	for (auto& [material, surface] : surfaces) {
		Array arrays;
		arrays.resize(Mesh::ARRAY_MAX);
		arrays[Mesh::ARRAY_VERTEX] = surface.vertices;
		arrays[Mesh::ARRAY_NORMAL] = surface.normals;
		arrays[Mesh::ARRAY_TEX_UV] = surface.uvs;
		arrays[Mesh::ARRAY_INDEX] = surface.indices;

		mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
		mesh->surface_set_material(mesh->get_surface_count() - 1, materials[material]);
	}

	return mesh;
}

void BlockLibrary::setTextures(Array textures) {
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

Array BlockLibrary::getTextures() {
	return textures;
}

void BlockLibrary::_init() {}

void BlockLibrary::addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, Direction side, std::map<unsigned, BlockLibrary::SurfaceData>::iterator surface) {
	Vector3 normal;
	switch (side) {
		case NORTH:
			normal = Vector3(0, 0, -1);
		break;
		case SOUTH:
			normal = Vector3(0, 0, 1);
		break;
		case EAST:
			normal = Vector3(1, 0, 0);
		break;
		case WEST:
			normal = Vector3(-1, 0, 0);
		break;
		case TOP:
			normal = Vector3(0, 1, 0);
		break;
		case BOTTOM:
			normal = Vector3(0, -1, 0);
		break;
	}

	/* if (wireframe) {
		std::array<int, 14> newIndices;
		if (side % 2)
			newIndices = {2,3,1,2, 1,0,2,1};
		else
			newIndices = {2,0,1,2, 1,3,2,1};

		for (const unsigned& i : newIndices)
			surface->second.indices.push_back(surface->second.vertices.size() + i);
	}else { */
		std::array<int, 6> newIndices;
		if (side % 2)
			newIndices = {2,3,1, 1,0,2};
		else
			newIndices = {2,0,1, 1,3,2};

		for (const unsigned& i : newIndices)
			surface->second.indices.push_back(surface->second.vertices.size() + i);
	//}

	for (int i = 0; i < 4; i++)
		surface->second.normals.push_back(normal);

	surface->second.uvs.push_back(Vector2(0, static_cast<real_t>(h)));
	surface->second.uvs.push_back(Vector2(static_cast<real_t>(w), static_cast<real_t>(h)));
	surface->second.uvs.push_back(Vector2(0, 0));
	surface->second.uvs.push_back(Vector2(static_cast<real_t>(w), 0));

	surface->second.vertices.push_back(bottom_left);
	surface->second.vertices.push_back(bottom_right);
	surface->second.vertices.push_back(top_left);
	surface->second.vertices.push_back(top_right);
}

void BlockLibrary::addCube(Vector3 origin, Vector3 size, const BlockLibrary::TypeData& type, std::map<unsigned, BlockLibrary::SurfaceData>& surfaces) {
	for (int i = 0; i < 6; i++)
		surfaces[type.materials[i]];
	addQuad(
		origin + Vector3(0,			0,			static_cast<real_t>(size.z)),
		origin + Vector3(0,			static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	0,			static_cast<real_t>(size.z)),
		static_cast<int>(size.x), static_cast<int>(size.y),
		SOUTH, surfaces.find(type.materials[0])
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			static_cast<real_t>(size.y),		0),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),		0),
		origin + Vector3(static_cast<real_t>(size.x),	0,			0),
		static_cast<int>(size.x), static_cast<int>(size.y),
		NORTH, surfaces.find(type.materials[1])
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			static_cast<real_t>(size.y),		0),
		origin + Vector3(0,			static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(0,			0,			static_cast<real_t>(size.z)),
		static_cast<int>(size.z), static_cast<int>(size.y),
		WEST, surfaces.find(type.materials[2])
	);
	
	addQuad(
		origin + Vector3(static_cast<real_t>(size.x),	0,			0),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),		0),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	0,			static_cast<real_t>(size.z)),
		static_cast<int>(size.z), static_cast<int>(size.y),
		EAST, surfaces.find(type.materials[3])
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			0,		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	0,		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	0,			0),
		static_cast<int>(size.x), static_cast<int>(size.z),
		BOTTOM, surfaces.find(type.materials[4])
	);
	
	addQuad(
		origin + Vector3(0,			static_cast<real_t>(size.y),			0),
		origin + Vector3(0,			static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),		static_cast<real_t>(size.z)),
		origin + Vector3(static_cast<real_t>(size.x),	static_cast<real_t>(size.y),			0),
		static_cast<int>(size.x), static_cast<int>(size.z),
		TOP, surfaces.find(type.materials[5])
	);
}