#ifndef ITEMLIBRARY_H
#define ITEMLIBRARY_H

#include <Godot.hpp>
#include <Resource.hpp>
#include <Ref.hpp>
#include <ArrayMesh.hpp>

namespace godot {
	class ItemLibrary : public Resource {
		GODOT_CLASS(ItemLibrary, Resource)

		Array textures;

		struct MeshData {
			Array vertices;
			PoolVector3Array normals;
			PoolColorArray colors;
			PoolIntArray indices;

			void clear() {
				vertices.resize(0);
				normals.resize(0);
				colors.resize(0);
				indices.resize(0);
			}

			Array getArrays() {
				Array arrays;
				arrays.resize(Mesh::ARRAY_MAX);
				arrays[Mesh::ARRAY_VERTEX] = vertices;
				arrays[Mesh::ARRAY_NORMAL] = normals;
				arrays[Mesh::ARRAY_COLOR] = colors;
				arrays[Mesh::ARRAY_INDEX] = indices;
				return arrays;
			}
		};

		public:

		void _init();

		static void _register_methods();

		void setTextures(Array textures);

		Array getTextures();

		void addQuad(MeshData& data, const Color& color, const int& x, const int& y, const bool& front);

		Ref<ArrayMesh> getItemMesh(const unsigned typeId);
	};
}

#endif