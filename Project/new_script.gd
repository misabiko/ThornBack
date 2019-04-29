extends Node

export(Array, StreamTexture) var textures
export(Array, Array, StreamTexture) var textures1

func _ready():
	for p in get_property_list():
		print(p)
		print(p.hint)