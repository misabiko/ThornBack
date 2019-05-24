extends Node

const Item = preload("res://Scenes/Item.tscn")
onready var player = $"../Player"

func spawn_block(id, coord):
	var item = Item.instance()
	item.get_node("MeshInstance").mesh = CubeMesh.new()
	item.translation = coord + Vector3(0.5, 0.5, 0.5)
	item.id = id
	add_child(item)