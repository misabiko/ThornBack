#include "Chunk.h"
#include <BoxShape.hpp>
#include <PoolArrays.hpp>

using namespace godot;

bool Chunk::wireframe = false;

void Chunk::_register_methods() {
	register_method("init", &Chunk::init);
	register_method("update_mesh", &Chunk::updateMesh);
	register_method("_process", &Chunk::_process);
	register_method("_ready", &Chunk::_ready);
	register_method("set_block", &Chunk::setBlock);
	register_method("clear_block", &Chunk::clearBlock);

	register_property<Chunk, bool>("use_wireframe", &Chunk::setWireframe, &Chunk::getWireframe, false);
}

void Chunk::_init() {}

void Chunk::_ready() {
	get_node("../..")->call_deferred("add_chunk", Vector2(static_cast<real_t>(coords.first), static_cast<real_t>(coords.second)), get_path());
	Array owners = staticBody->get_shape_owners();
	for (int i = 0; i < owners.size(); i++)
		staticBody->shape_owner_set_disabled(owners[i], false);

	set_visible(true);
	set_process(false);
}

void Chunk::_process(float delta) {
	if (mustRemesh)
		collisionMesher();
	updateMesh();

	mustRemesh = false;
	set_process(false);
}

void Chunk::init(int x, int y, Ref<WorldData> worldData, Ref<BlockLibrary> blockLibrary) {
	this->blockLibrary = blockLibrary;
	this->worldData = worldData;

	coords = std::pair<int, int>(x, y);
	worldData->tryInit(coords);

	set_visible(false);

	set_translation(Vector3(static_cast<real_t>(x), 0, static_cast<real_t>(y)) * static_cast<real_t>(CHUNK_SIZE));

	staticBody = StaticBody::_new();
	staticBody->set_collision_layer_bit(2, true);
	add_child(staticBody);

	set_mesh(ArrayMesh::_new());
	collisionMesher();

	updateMesh();
}

void Chunk::setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type) {
	worldData->getBlock(coords, x, y, z)->set(type, true);

	mustRemesh = true;
	set_process(true);
}

void Chunk::clearBlock(const unsigned x, const unsigned y, const unsigned z) {
	worldData->getBlock(coords, x, y, z)->set(0, false);

	mustRemesh = true;
	set_process(true);
}

void Chunk::updateMesh() {
	Ref<ArrayMesh> mesh = get_mesh();
	while (mesh->get_surface_count())
		mesh->surface_remove(0);

	for (auto& [material, surface] : surfaces) {
		Array arrays;
		arrays.resize(Mesh::ARRAY_MAX);
		arrays[Mesh::ARRAY_VERTEX] = surface.vertices;
		arrays[Mesh::ARRAY_NORMAL] = surface.normals;
		arrays[Mesh::ARRAY_TEX_UV] = surface.uvs;
		arrays[Mesh::ARRAY_INDEX] = surface.indices;

		mesh->add_surface_from_arrays(wireframe ? Mesh::PRIMITIVE_LINES : Mesh::PRIMITIVE_TRIANGLES, arrays);
		mesh->surface_set_material(mesh->get_surface_count() - 1, blockLibrary->getMaterial(material));
	}
}

void Chunk::collisionMesher() {
	{
		for (auto &[material, surface] : surfaces)
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

					pos = Vector3(static_cast<real_t>(i), static_cast<real_t>(j), static_cast<real_t>(k));

					Ref<BoxShape> box;
					box.instance();
					box->set_extents(cubeSize * 0.5f);
					int64_t shapeOwner = staticBody->create_shape_owner(staticBody);

					staticBody->shape_owner_set_disabled(shapeOwner, true);
					staticBody->shape_owner_set_transform(shapeOwner, Transform().translated(pos + cubeSize * 0.5f));
					staticBody->shape_owner_add_shape(shapeOwner, box);

					blockLibrary->addCube(pos, cubeSize, blockLibrary->get(worldData->getBlock(coords, i, j, k)->type), surfaces);
				}
}

void Chunk::setWireframe(bool wireframe) {
	this->wireframe = wireframe;
	set_process(true);
}

bool Chunk::getWireframe() {
	return wireframe;
}