extends Node

const Item = preload("res://Scenes/Item.tscn")

func spawn_block(id, coord):
	var item = Item.instance()
	item.get_node("MeshInstance").mesh = $"../ChunkLoader".block_library.get_block_mesh(id)
	item.translation = coord + Vector3(0.5, 0.5, 0.5)
	item.id = id
	add_child(item)