#include "Chunk.h"
#include <OS.hpp>
#include <string>
#include <BoxShape.hpp>

//TODO comment collisionMerger
//TODO comment greedyMerger
//TODO Add vcvar64 thing to hyper

using namespace godot;

void Chunk::_register_methods() {
	register_method("init", &Chunk::init);
	register_method("update_mesh", &Chunk::updateMesh);
	register_method("_process", &Chunk::_process);
	register_method("_ready", &Chunk::_ready);
	register_method("set_block", &Chunk::setBlock);
	register_method("clear_block", &Chunk::clearBlock);

	register_property<Chunk, Dictionary>("block_types", &Chunk::blockTypes, Dictionary());
	register_property<Chunk, Ref<OpenSimplexNoise>>("noise", &Chunk::noise, Ref<OpenSimplexNoise>());
	register_property<Chunk, unsigned short>("CHUNK_SIZE", &Chunk::CHUNK_SIZE, 16);
	register_property<Chunk, unsigned short>("WORLD_HEIGHT", &Chunk::WORLD_HEIGHT, 128);
	register_property<Chunk, unsigned short>("SURFACE_HEIGHT", &Chunk::SURFACE_HEIGHT, 60);
}

Chunk::~Chunk() {
	Godot::print("boop");
	for (int i = 0; i < voxels.size(); i++)
		for (int j = 0; j < voxels[0].size(); j++)
			for (int k = 0; k < voxels[0][0].size(); k++)
				if (voxels[i][j][k])
					delete voxels[i][j][k];

	surfaceTool.unref();
	staticBody->free();

	for (int i = 0; i < 3; i++)
		materials[i].unref();
}

void Chunk::_init() {}

void Chunk::_ready() {
	Array owners = staticBody->get_shape_owners();
	for (int i = 0; i < owners.size(); i++)
		staticBody->shape_owner_set_disabled(owners[i], false);

	set_visible(true);
}

void Chunk::_process(float delta) {
	collisionMesher();
	updateMesh();

	set_process(false);
}

void Chunk::init(int x, int y, Ref<OpenSimplexNoise> noise, Dictionary blockTypes) {
	this->noise = noise;
	this->blockTypes = blockTypes;
	coords = std::pair<int, int>(x, y);

	set_visible(false);

	{
		Array blockArray = blockTypes.values();
		for (int i = 0; i < 3; i++)
			materials[i] = Ref<Material>(blockArray[i].operator Dictionary()["material"]);
	}

	Vector3 translation = Vector3(x, 0, y) * CHUNK_SIZE;
	set_translation(translation);

	voxels.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; i++) {
		voxels[i].resize(WORLD_HEIGHT);

		for (int j = 0; j < WORLD_HEIGHT; j++) {
			voxels[i][j].resize(CHUNK_SIZE);

			for (int k = 0; k < CHUNK_SIZE; k++)
				voxels[i][j][k] = new VoxelFace();
		}
	}

	for (int i = 0; i < CHUNK_SIZE; i++)
		for (int k = 0; k < CHUNK_SIZE; k++) {
			Vector3 pos = Vector3(i, 0, k);
			pos.y = std::floor((noise->get_noise_2d((pos.x + translation.x) / 5, (pos.z + translation.z) / 5) / 2 + 0.5) * SURFACE_HEIGHT);
			setBlock(pos.x, pos.y, pos.z, 1);

			for (int j = 0; j < pos.y; j++)
				setBlock(pos.x, j, pos.z, 2);
		}

	staticBody = StaticBody::_new();
	add_child(staticBody);
	collisionMesher();

	surfaceTool.instance();
	updateMesh();
}

void Chunk::setBlock(const unsigned x, const unsigned y, const unsigned z, const unsigned type) {
	voxels[x][y][z]->type = type;
	voxels[x][y][z]->transparent = false;

	set_process(true);
}

void Chunk::clearBlock(const unsigned x, const unsigned y, const unsigned z) {
	voxels[x][y][z]->type = 0;
	voxels[x][y][z]->transparent = true;

	set_process(true);
}

void Chunk::addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, unsigned type, Direction side) {
	SurfaceData& surface = surfaces[type - 1];

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

	surface.uvs.push_back(Vector2(0, 0));
	surface.uvs.push_back(Vector2(w, 0));
	surface.uvs.push_back(Vector2(0, h));
	surface.uvs.push_back(Vector2(w, h));

	for (int i = 0; i < 4; i++)
		surface.normals.push_back(normal);
	
	surface.vertices.push_back(bottom_left);
	surface.vertices.push_back(bottom_right);
	surface.vertices.push_back(top_left);
	surface.vertices.push_back(top_right);
}

void Chunk::updateMesh() {
	for (int i = 0; i < 3; i++)
		if (surfaces[i].vertices.size()) {
			surfaceTool->begin(Mesh::PRIMITIVE_TRIANGLES);

			for (int j = 0; j < surfaces[i].vertices.size(); j++) {
				surfaceTool->add_normal(surfaces[i].normals[j]);
				surfaceTool->add_uv(surfaces[i].uvs[j]);
				surfaceTool->add_vertex(surfaces[i].vertices[j]);
			}

			for (int j = 0; j < surfaces[i].indices.size(); j++)
				surfaceTool->add_index(surfaces[i].indices[j]);
			
			surfaceTool->generate_tangents();

			surfaceTool->set_material(materials[i]);

			if (get_mesh().is_null())
				set_mesh(surfaceTool->commit());
			else
				surfaceTool->commit(get_mesh());
		}
}

void Chunk::collisionMesher() {
	{
		Ref<ArrayMesh> mesh = get_mesh();
		if (!mesh.is_null()) {
			for (int i = 0; mesh->get_surface_count(); i++) {	//;)
				mesh->surface_remove(0);
				surfaces[i] = SurfaceData();
			}
		}
		
		Array owners = staticBody->get_shape_owners();
		for (int i = 0; i < owners.size(); i++)
			staticBody->shape_owner_clear_shapes(owners[i]);
	}

	std::vector<std::vector<std::vector<bool>>> voxelMask;
	voxelMask.resize(CHUNK_SIZE);
	for (int i = 0; i < CHUNK_SIZE; i++) {
		voxelMask[i].resize(WORLD_HEIGHT);
		for (int j = 0; j < WORLD_HEIGHT; j++) {
			voxelMask[i][j].resize(CHUNK_SIZE);
			for (int k = 0; k < CHUNK_SIZE; k++)
				voxelMask[i][j][k] = false;
		}
	}

	Vector3 pos, cubeSize;
	for (int i = 0; i < CHUNK_SIZE; i++)
		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int k = 0; k < CHUNK_SIZE; k++)
				if (!voxelMask[i][j][k] && voxels[i][j][k]->type) {
					voxelMask[i][j][k] = true;
					cubeSize = Vector3(1, 1, 1);

					//TODO break labels
					bool done = false;
					for (int di = 1; di < CHUNK_SIZE - i; di++)
						if (voxelMask[i + di][j][k] || !voxels[i + di][j][k]->equals(voxels[i][j][k])) {
							cubeSize.x += di - 1;
							done = true;
							break;
						}else
							voxelMask[i + di][j][k] = true;
					
					if (!done)
						cubeSize.x += CHUNK_SIZE - i - 1;

					done = false;
					for (int dk = 1; dk < CHUNK_SIZE - k; dk++) {
						for (int di = 0; di < cubeSize.x; di++)
							if (voxelMask[i + di][j][k + dk] || !voxels[i + di][j][k + dk]->equals(voxels[i][j][k])) {
								cubeSize.z += dk - 1;
								done = true;
								break;
							}
						
						if (done)
							break;
						else
							for (int di = 0; di < cubeSize.x; di++)
								voxelMask[i + di][j][k + dk] = true;
					}

					if (!done)
						cubeSize.z += CHUNK_SIZE - k - 1;

					done = false;
					for (int dj = 1; dj < WORLD_HEIGHT - j; dj++) {
						for (int dk = 0; dk < cubeSize.z; dk++) {
							for (int di = 0; di < cubeSize.x; di++)
								if (voxelMask[i + di][j + dj][k + dk] || !voxels[i + di][j + dj][k + dk]->equals(voxels[i][j][k])) {
									cubeSize.y += dj - 1;
									done = true;
									break;
								}
							if (done)
								break;
						}
						if (done)
							break;
						else
							for (int dk = 0; dk < cubeSize.z; dk++)
								for (int di = 0; di < cubeSize.x; di++)
									voxelMask[i + di][j + dj][k + dk] = true;
					}

					if (!done)
						cubeSize.y += WORLD_HEIGHT - j - 1;

					pos = Vector3(i, j, k);

					Ref<BoxShape> box;
					box.instance();
					box->set_extents(cubeSize * 0.5f);
					int64_t shapeOwner = staticBody->create_shape_owner(staticBody);

					staticBody->shape_owner_set_disabled(shapeOwner, true);
					staticBody->shape_owner_set_transform(shapeOwner, Transform().translated(pos + cubeSize * 0.5f));
					staticBody->shape_owner_add_shape(shapeOwner, box);

					addCube(pos, cubeSize, voxels[i][j][k]->type);
				}
}

void Chunk::addCube(Vector3 origin, Vector3 size, unsigned type) {
	addQuad(
		origin + Vector3(0,			0,			size.z),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	0,			size.z),
		size.x, size.y,
		type, SOUTH
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			size.y,		0),
		origin + Vector3(size.x,	size.y,		0),
		origin + Vector3(size.x,	0,			0),
		size.x, size.y,
		type, NORTH
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			size.y,		0),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(0,			0,			size.z),
		size.z, size.y,
		type, WEST
	);
	
	addQuad(
		origin + Vector3(size.x,			0,			0),
		origin + Vector3(size.x,	size.y,		0),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	0,			size.z),
		size.z, size.y,
		type, EAST
	);
	
	addQuad(
		origin,
		origin + Vector3(0,			0,		size.z),
		origin + Vector3(size.x,	0,		size.z),
		origin + Vector3(size.x,	0,			0),
		size.x, size.z,
		type, BOTTOM
	);
	
	addQuad(
		origin + Vector3(0,			size.y,			0),
		origin + Vector3(0,			size.y,		size.z),
		origin + Vector3(size.x,	size.y,		size.z),
		origin + Vector3(size.x,	size.y,			0),
		size.x, size.z,
		type, TOP
	);
}

Chunk::VoxelFace* Chunk::getVoxelFace(const unsigned& x, const unsigned& y, const unsigned& z, Direction side) {
	VoxelFace* voxel = voxels[x][y][z];
	voxel->side = side;

	return voxel;
}