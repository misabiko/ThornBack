extends Node

const Item = preload("res://Scenes/Item.tscn")

func _input(event):
	if event is InputEventKey and event.is_pressed():
		match event.scancode:
			KEY_H:
				spawn_block(1, $"../Player".translation - $"../Player".get_global_transform().basis.z)

func spawn_block(type, pos):
	var item = Item.instance()
	item.get_node("MeshInstance").mesh = CubeMesh.new()
	item.translation = pos
	add_child(item)
	print("pop")