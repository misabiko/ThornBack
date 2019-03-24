#include "Chunk.h"
#include <OS.hpp>
#include <string>

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
}

void Chunk::_init() {}

void Chunk::init(int x, int y) {
	Vector3 translation = Vector3(x, 0, y) * CHUNK_SIZE;
	set_translation(translation);
	long long lastTime = OS::get_singleton()->get_ticks_msec();

	voxels.resize(CHUNK_SIZE);

	for (int i = 0; i < CHUNK_SIZE; i++) {
		voxels[i].resize(WORLD_HEIGHT);

		for (int j = 0; j < WORLD_HEIGHT; j++) {
			voxels[i][j].resize(CHUNK_SIZE);

			for (int k = 0; k < CHUNK_SIZE; k++)
				voxels[i][j][k] = new VoxelFace();
		}
	}

	Godot::print(("Voxel data dec: " + std::to_string(OS::get_singleton()->get_ticks_msec() - lastTime)).c_str());
	lastTime = OS::get_singleton()->get_ticks_msec();

	for (int i = 0; i < CHUNK_SIZE; i++)
		for (int k = 0; k < CHUNK_SIZE; k++) {
			Vector3 pos = Vector3(i, 0, k);
			pos.y = std::floor((noise->get_noise_2d((pos.x + translation.x) / 5, (pos.z + translation.z) / 5) / 2 + 0.5) * SURFACE_HEIGHT);
			newBlock(pos.x, pos.y, pos.z, 1);

			for (int j = 0; j < pos.y; j++)
				newBlock(pos.x, j, pos.z, 2);
		}
	
	Godot::print(("Voxel data init: " + std::to_string(OS::get_singleton()->get_ticks_msec() - lastTime)).c_str());
	lastTime = OS::get_singleton()->get_ticks_msec();

	greedyMesher();
	
	Godot::print(("Chunk: " + std::to_string(OS::get_singleton()->get_ticks_msec() - lastTime)).c_str());
	lastTime = OS::get_singleton()->get_ticks_msec();

	surfaceTool = SurfaceTool::_new();
	updateMesh();
	
	Godot::print(("Mesh creation: " + std::to_string(OS::get_singleton()->get_ticks_msec() - lastTime)).c_str());
	lastTime = OS::get_singleton()->get_ticks_msec();

	Godot::print("\n");
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

void Chunk::addQuad(Vector3 bottom_left, Vector3 top_left, Vector3 top_right, Vector3 bottom_right, VoxelFace* voxel, bool backFace) {
	Vector2 uv = blockTypes.values()[voxel->type - 1].operator Dictionary()["texture"].operator Vector2();

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
		indices.push_back(vertices.size() + i);

	uvs.push_back(uv);
	uvs.push_back(uv + Vector2(0.25, 0));
	uvs.push_back(uv + Vector2(0, 0.5));
	uvs.push_back(uv + Vector2(0.25, 0.5));

	for (int i = 0; i < 4; i++)
		normals.push_back(normal);
	
	vertices.push_back(bottom_left);
	vertices.push_back(bottom_right);
	vertices.push_back(top_left);
	vertices.push_back(top_right);
}

void Chunk::updateMesh() {
	surfaceTool->begin(Mesh::PRIMITIVE_TRIANGLES);

	for (int i = 0; i < vertices.size(); i++) {
		surfaceTool->add_normal(normals[i]);
		surfaceTool->add_uv(uvs[i]);
		surfaceTool->add_vertex(vertices[i]);
	}

	for (int i = 0; i < indices.size(); i++)
		surfaceTool->add_index(indices[i]);
	
	surfaceTool->generate_tangents();
	set_mesh(surfaceTool->commit());
}

Chunk::VoxelFace* Chunk::getVoxelFace(const unsigned& x, const unsigned& y, const unsigned& z, Direction side) {
	VoxelFace* voxel = voxels[x][y][z];
	voxel->side = side;

	return voxel;
}