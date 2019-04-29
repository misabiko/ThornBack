#include "Chunk.h"
#include <BoxShape.hpp>
#include <PoolArrays.hpp>

using namespace godot;

void Chunk::_register_methods() {
	register_method("init", &Chunk::init);
	register_method("update_mesh", &Chunk::updateMesh);
	register_method("_process", &Chunk::_process);
	register_method("_ready", &Chunk::_ready);
	register_method("set_block", &Chunk::setBlock);
	register_method("clear_block", &Chunk::clearBlock);
}

void Chunk::_init() {}

void Chunk::_ready() {
	get_node("../..")->call_deferred("add_chunk", Vector2(coords.first, coords.second), get_path());
	Array owners = staticBody->get_shape_owners();
	for (int i = 0; i < owners.size(); i++)
		staticBody->shape_owner_set_disabled(owners[i], false);

	set_visible(true);
	set_process(false);
}

void Chunk::_process(float delta) {
	collisionMesher();
	updateMesh();

	set_process(false);
}

void Chunk::init(int x, int y, Ref<WorldData> worldData, Ref<BlockLibrary> blockLibrary) {
	this->blockLibrary = blockLibrary;
	this->worldData = worldData;

	coords = std::pair<int, int>(x, y);
	worldData->tryInit(coords);

	set_visible(false);

	set_translation(Vector3(x, 0, y) * CHUNK_SIZE);

	staticBody = StaticBody::_new();
	add_child(staticBody);

	set_mesh(ArrayMesh::_new());
	collisionMesher();

	updateMesh();
}

void Chunk::setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type) {
	worldData->getBlock(coords, x, y, z)->set(type, true);

	set_process(true);
}

void Chunk::clearBlock(const unsigned x, const unsigned y, const unsigned z) {
	worldData->getBlock(coords, x, y, z)->set(0, false);

	set_process(true);
}

void Chunk::addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, unsigned type, Direction side) {
	SurfaceData& surface = surfaces[type];

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

	std::array<int, 6> newIndices;
	if (side % 2)
		newIndices = {2,3,1, 1,0,2};
	else
		newIndices = {2,0,1, 1,3,2};

	for (const unsigned& i : newIndices)
		surface.indices.push_back(surface.vertices.size() + i);

	surface.uvs.push_back(Vector2(0, h));
	surface.uvs.push_back(Vector2(w, h));
	surface.uvs.push_back(Vector2(0, 0));
	surface.uvs.push_back(Vector2(w, 0));

	for (int i = 0; i < 4; i++)
		surface.normals.push_back(normal);

	surface.vertices.push_back(bottom_left);
	surface.vertices.push_back(bottom_right);
	surface.vertices.push_back(top_left);
	surface.vertices.push_back(top_right);
}

void Chunk::updateMesh() {
	Ref<ArrayMesh> mesh = get_mesh();
	for (auto& [material, surface] : surfaces) {
		Array arrays;
		arrays.resize(Mesh::ARRAY_MAX);
		arrays[Mesh::ARRAY_VERTEX] = surface.vertices;
		arrays[Mesh::ARRAY_NORMAL] = surface.normals;
		arrays[Mesh::ARRAY_TEX_UV] = surface.uvs;
		arrays[Mesh::ARRAY_INDEX] = surface.indices;

		mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
		mesh->surface_set_material(mesh->get_surface_count() - 1, blockLibrary->getMaterial(material));
	}
}

void Chunk::collisionMesher() {
	{
		Ref<ArrayMesh> mesh = get_mesh();
		while (mesh->get_surface_count())
			mesh->surface_remove(0);
		for (auto& [material, surface] : surfaces)
			surface.clear();
		
		Array owners = staticBody->get_shape_owners();
		for (int i = 0; i < owners.size(); i++)
			staticBody->shape_owner_clear_shapes(owners[i]);
	}

	bool voxelMask[CHUNK_SIZE][WORLD_HEIGHT][CHUNK_SIZE] = {false};

	Vector3 pos, cubeSize;
	for (int i = 0; i < CHUNK_SIZE; i++)
		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int k = 0; k < CHUNK_SIZE; k++)
				if (!voxelMask[i][j][k] && worldData->getBlock(coords, i, j, k)->type) {
					voxelMask[i][j][k] = true;
					cubeSize = Vector3(1, 1, 1);

					for (int di = 1; di < CHUNK_SIZE - i; di++)
						if (voxelMask[i + di][j][k] || !worldData->getBlock(coords, i + di, j, k)->equals(worldData->getBlock(coords, i, j, k))) {
							cubeSize.x += di - 1;
							goto fullbreak1;
						}else
							voxelMask[i + di][j][k] = true;
					
					cubeSize.x += CHUNK_SIZE - i - 1;	//This is skipped if goto fullbreak1
					fullbreak1:

					for (int dk = 1; dk < CHUNK_SIZE - k; dk++) {
						for (int di = 0; di < cubeSize.x; di++)
							if (voxelMask[i + di][j][k + dk] || !worldData->getBlock(coords, i + di, j, k + dk)->equals(worldData->getBlock(coords, i, j, k))) {
								cubeSize.z += dk - 1;
								goto fullbreak2;
							}
						
						for (int di = 0; di < cubeSize.x; di++)
							voxelMask[i + di][j][k + dk] = true;
					}

					cubeSize.z += CHUNK_SIZE - k - 1;	//This is skipped if goto fullbreak2
					fullbreak2:

					for (int dj = 1; dj < WORLD_HEIGHT - j; dj++) {
						for (int dk = 0; dk < cubeSize.z; dk++)
							for (int di = 0; di < cubeSize.x; di++)
								if (voxelMask[i + di][j + dj][k + dk] || !worldData->getBlock(coords, i + di, j + dj, k + dk)->equals(worldData->getBlock(coords, i, j, k))) {
									cubeSize.y += dj - 1;
									goto fullbreak3;
								}
						
						for (int dk = 0; dk < cubeSize.z; dk++)
							for (int di = 0; di < cubeSize.x; di++)
								voxelMask[i + di][j + dj][k + dk] = true;
					}

					cubeSize.y += WORLD_HEIGHT - j - 1;	//This is skipped if goto fullbreak3
					fullbreak3:

					pos = Vector3(i, j, k);

					Ref<BoxShape> box;
					box.instance();
					box->set_extents(cubeSize * 0.5f);
					int64_t shapeOwner = staticBody->create_shape_owner(staticBody);

					staticBody->shape_owner_set_disabled(shapeOwner, true);
					staticBody->shape_owner_set_transform(shapeOwner, Transform().translated(pos + cubeSize * 0.5f));
					staticBody->shape_owner_add_shape(shapeOwner, box);

					addCube(pos, cubeSize, blockLibrary->get(worldData->getBlock(coords, i, j, k)->type));
				}
}

void Chunk::addCube(Vector3 origin, Vector3 size, const BlockLibrary::TypeData& type) {
	addQuad(
		origin + Vector3(0,			0,			size.z),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	0,			size.z),
		size.x, size.y,
		type.materials[0], SOUTH
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			size.y,		0),
		origin + Vector3(size.x,	size.y,		0),
		origin + Vector3(size.x,	0,			0),
		size.x, size.y,
		type.materials[1], NORTH
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			size.y,		0),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(0,			0,			size.z),
		size.z, size.y,
		type.materials[2], WEST
	);
	
	addQuad(
		origin + Vector3(size.x,	0,			0),
		origin + Vector3(size.x,	size.y,		0),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	0,			size.z),
		size.z, size.y,
		type.materials[3], EAST
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			0,		size.z),
		origin + Vector3(size.x,	0,		size.z),
		origin + Vector3(size.x,	0,			0),
		size.x, size.z,
		type.materials[4], BOTTOM
	);
	
	addQuad(
		origin + Vector3(0,			size.y,			0),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	size.y,			0),
		size.x, size.z,
		type.materials[5], TOP
	);
}