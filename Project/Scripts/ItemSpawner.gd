extends Node

const Item = preload("res://Scenes/Item.tscn")

func spawn_block(id, coord, impulse = Vector3()):
	var item = Item.instance()
	item.get_node("MeshInstance").mesh = $"../ChunkLoader".block_library.get_block_mesh(id)
#	item.get_node("MeshInstance").mesh = get_parent().item_library.get_item_mesh(id)
	item.translation = coord + Vector3(0.5, 0.5, 0.5)
	item.id = id
	if impulse != Vector3():
		item.apply_central_impulse(impulse)
	add_child(item)