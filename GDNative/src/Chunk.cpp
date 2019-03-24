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
	register_method("greedy_mesher", &Chunk::greedyMesher);
	register_method("update_mesh", &Chunk::updateMesh);

	register_property<Chunk, Dictionary>("block_types", &Chunk::blockTypes, Dictionary());
	register_property<Chunk, Ref<OpenSimplexNoise>>("noise", &Chunk::noise, Ref<OpenSimplexNoise>());
	register_property<Chunk, unsigned short>("CHUNK_SIZE", &Chunk::CHUNK_SIZE, 16);
	register_property<Chunk, unsigned short>("WORLD_HEIGHT", &Chunk::WORLD_HEIGHT, 128);
	register_property<Chunk, unsigned short>("SURFACE_HEIGHT", &Chunk::SURFACE_HEIGHT, 60);
}

Chunk::~Chunk() {
	for (int i = 0; i < voxels.size(); i++)
		for (int j = 0; j < voxels[0].size(); j++)
			for (int k = 0; k < voxels[0][0].size(); k++)
				if (voxels[i][j][k])
					delete voxels[i][j][k];
	
	surfaceTool->free();
	staticBody->free();

	for (int i = 0; i < 3; i++)
		materials[i].unref();
}

void Chunk::_init() {}

void Chunk::init(int x, int y) {
	//int64_t lastTime = OS::get_singleton()->get_ticks_msec();
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
			newBlock(pos.x, pos.y, pos.z, 1);

			for (int j = 0; j < pos.y; j++)
				newBlock(pos.x, j, pos.z, 2);
		}

	greedyMesher();

	surfaceTool = SurfaceTool::_new();
	updateMesh();

	staticBody = StaticBody::_new();
	add_child(staticBody);
	collisionMesher();

	//Godot::print((std::to_string(OS::get_singleton()->get_ticks_msec() - lastTime) + "ms").c_str());
}

void Chunk::newBlock(const int x, const int y, const int z, int type) {
	voxels[x][y][z]->type = type;
	voxels[x][y][z]->transparent = false;
}

void Chunk::greedyMesher() {
	int n;
	Direction side;
	int quad_width, quad_height;

	std::array<short, 3> x;
	std::array<short, 3> q;
	std::array<short, 3> chunk_size{CHUNK_SIZE, WORLD_HEIGHT, CHUNK_SIZE};
	std::array<short, 3> du;
	std::array<short, 3> dv;

	std::vector<VoxelFace*> mask = std::vector<VoxelFace*>(CHUNK_SIZE * WORLD_HEIGHT);

	VoxelFace* voxel_face1 = nullptr;
	VoxelFace* voxel_face2 = nullptr;

	for (bool backFace = true, b = false; b != backFace; backFace = backFace && b, b = !b) {
		for (int axis = 0; axis < 3; axis++) {
			int u = (axis + 1) % 3;
			int v = (axis + 2) % 3;

			x.fill(0);
			q.fill(0);
			q[axis] = 1;

			switch (axis) {
				case 0:
					side = backFace ? WEST : EAST;
					break;
				case 1:
					side = backFace ? BOTTOM : TOP;
					break;
				case 2:
					side = backFace ? SOUTH : NORTH;
					break;
			}

			for (x[axis] = -1; x[axis] < chunk_size[axis];) {
				n = 0;

				for (x[v] = 0; x[v] < chunk_size[v]; x[v]++) {
					for (x[u] = 0; x[u] < chunk_size[u]; x[u]++) {
						voxel_face1 = x[axis] >= 0 ? getVoxelFace(x[0], x[1], x[2], side) : nullptr;
						voxel_face2 = x[axis] < chunk_size[axis] - 1 ? getVoxelFace(x[0] + q[0], x[1] + q[1], x[2] + q[2], side) : nullptr;

						mask[n] = (voxel_face1 != nullptr && voxel_face2 != nullptr && voxel_face1->equals(voxel_face2)) ? nullptr : (backFace ? voxel_face2 : voxel_face1);
						n++;
					}
				}

				x[axis]++;

				n = 0;

				for (int j = 0; j < chunk_size[v]; j++)
					for (int i = 0; i < chunk_size[u];) {
						if (mask[n] != nullptr) {
							for (quad_width = 1; i + quad_width < chunk_size[u] && mask[n + quad_width] != nullptr && mask[n + quad_width]->equals(mask[n]); quad_width++);

							bool done = false;
							for (quad_height = 1; j + quad_height < chunk_size[v]; quad_height++) {
								for (int k = 0; k < quad_width; k++)
									if (mask[n + k + quad_height * chunk_size[u]] == nullptr || !mask[n + k + quad_height * chunk_size[u]]->equals(mask[n])) {
										done = true;
										break;
									}
								
								if (done)
									break;
							}

							if (!mask[n]->transparent) {
								x[u] = i;
								x[v] = j;

								du.fill(0);
								du[u] = quad_width;

								dv.fill(0);
								dv[v] = quad_height;

								addQuad(
									Vector3(x[0],					x[1],					x[2]),
									Vector3(x[0] + du[0],			x[1] + du[1],			x[2] + du[2]),
									Vector3(x[0] + du[0] + dv[0],	x[1] + du[1] + dv[1],	x[2] + du[2] + dv[2]),
									Vector3(x[0] + dv[0],			x[1] + dv[1],			x[2] + dv[2]),
									quad_width, quad_height,
									mask[n], backFace
								);
							}

							for (int l = 0; l < quad_height; l++)
								for (int k = 0; k < quad_width; k++)
									mask[n + k + l * chunk_size[u]] = nullptr;
							
							i += quad_width;
							n += quad_width;
						}else {
							i++;
							n++;
						}
					}
			}
		}
	}
}

void Chunk::addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, int w, int h, VoxelFace* voxel, bool backFace) {
	SurfaceData& surface = surfaces[voxel->type - 1];

	Vector3 normal;
	switch (voxel->side) {
		case SOUTH:
			normal = Vector3(0, 0, 1);
		break;
		case NORTH:
			normal = Vector3(0, 0, -1);
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
	if (backFace)
		newIndices = {2,3,1, 1,0,2};
	else
		newIndices = {2,0,1, 1,3,2};

	for (const unsigned& i : newIndices)
		surface.indices.push_back(surface.vertices.size() + i);

	surface.uvs.push_back(Vector2(0, 0));
	surface.uvs.push_back(Vector2(h, 0));
	surface.uvs.push_back(Vector2(0, w));
	surface.uvs.push_back(Vector2(h, w));

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

	Vector3 cubeSize;
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

					BoxShape* box = BoxShape::_new();
					box->set_extents(cubeSize * 0.5f);
					int64_t shapeOwner = staticBody->create_shape_owner(staticBody);

					staticBody->shape_owner_set_transform(shapeOwner, Transform().translated(Vector3(i, j, k) + cubeSize * 0.5f));
					staticBody->shape_owner_add_shape(shapeOwner, box);
				}
}

Chunk::VoxelFace* Chunk::getVoxelFace(const unsigned& x, const unsigned& y, const unsigned& z, Direction side) {
	VoxelFace* voxel = voxels[x][y][z];
	voxel->side = side;

	return voxel;
}