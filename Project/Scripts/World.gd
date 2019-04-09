extends Node

var BlockTypes = [{
		"name": "Dirt",
		"material": preload("res://Materials/dirt.tres")
	}, {
		"name": "Cobblestone",
		"material": preload("res://Materials/cobblestone.tres")
	}, {
		"name": "Grass",
		"material": preload("res://Materials/grass.tres")
	}
]
var chunks = {}

func _ready():	
	$ChunkLoader.block_types = BlockTypes
	$ChunkLoader.init()
	
	$Player/DebugHelper.add_property("delay", $ChunkLoader.get_path())
	$Player/DebugHelper.add_method("Chunks to load", "get_backlog_size", $ChunkLoader.get_path())

func add_chunk(coords, chunk_path):
	chunks[coords] = chunk_path

func get_chunk(pos):
	return chunks[Vector2(floor(pos.x / 16), floor(pos.z / 16))]

func get_chunk_coords(pos):
	return Vector2(floor(pos.x / 16), floor(pos.z / 16))

func world_to_chunk(pos):
	return Vector3(
		0 if fmod(pos.x, 16) == 0 else (16 + fmod(pos.x, 16) if pos.x < 0 else fmod(pos.x, 16)),
		pos.y,
		0 if fmod(pos.z, 16) == 0 else (16 + fmod(pos.z, 16) if pos.z < 0 else fmod(pos.z, 16))
	)

func add_block(pos, type):
	var selectedPos = world_to_chunk(pos)
	get_node(get_chunk(pos)).set_block(selectedPos.x, selectedPos.y, selectedPos.z, type)

func remove_block(pos):
	var selectedPos = world_to_chunk(pos)
	get_node(get_chunk(pos)).clear_block(selectedPos.x, selectedPos.y, selectedPos.z)

func _input(event):
	if event is InputEventKey and event.is_pressed():
		match event.scancode:
			KEY_J:
				$ChunkLoader.save()

func _exit_tree():
	for type in BlockTypes:
		type.material.unreference()