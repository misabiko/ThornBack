extends Node

var chunks = {}

func _ready():
	$ChunkLoader.init()
	
	$Player/DebugHelper.add_method("Chunks to load", "get_backlog_size", $ChunkLoader.get_path())
	$Player/DebugHelper.add_property("load_chunks", $ChunkLoader.get_path())

func add_chunk(coords, chunk_path):
	chunks[coords] = chunk_path

func get_chunk(pos):
	return chunks[Vector2(floor(pos.x / 16), floor(pos.z / 16))]

func get_chunk_coords(pos):
	return Vector2(floor(pos.x / 16), floor(pos.z / 16))

func world_to_chunk(pos):
	return Vector3(
		0.0 if fmod(pos.x, 16) == 0 else (16 + fmod(pos.x, 16) if pos.x < 0 else fmod(pos.x, 16)),
		pos.y,
		0.0 if fmod(pos.z, 16) == 0 else (16 + fmod(pos.z, 16) if pos.z < 0 else fmod(pos.z, 16))
	)

func add_block(pos, type):
	var selected_pos = world_to_chunk(pos)
	get_node(get_chunk(pos)).set_block(selected_pos.x, selected_pos.y, selected_pos.z, type)

func remove_block(pos):
	var selected_pos = world_to_chunk(pos)
	get_node(get_chunk(pos)).clear_block(selected_pos.x, selected_pos.y, selected_pos.z)

func break_block(pos):
	var selected_pos = world_to_chunk(pos)
	var chunk_coords = get_chunk_coords(pos)
	var id = $ChunkLoader.world_data.get_block_type(chunk_coords.x, chunk_coords.y, selected_pos.x, selected_pos.y, selected_pos.z)
	get_node(get_chunk(pos)).clear_block(selected_pos.x, selected_pos.y, selected_pos.z)
	$ItemSpawner.spawn_block(id, pos)

func get_block_id(pos):
	var selected_pos = world_to_chunk(pos)
	var chunk_coords = get_chunk_coords(pos)
	return $ChunkLoader.world_data.get_block_type(chunk_coords.x, chunk_coords.y, selected_pos.x, selected_pos.y, selected_pos.z)

func _input(event):
	if event is InputEventKey and event.is_pressed():
		match event.scancode:
			KEY_J:
				$ChunkLoader.save()
			KEY_U:
				$ChunkLoader.load_chunks = !$ChunkLoader.load_chunks
			KEY_O:
				for chunk in $ChunkLoader.get_children():
					chunk.use_wireframe = !chunk.use_wireframe