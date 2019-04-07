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

func _ready():	
	$ChunkLoader.block_types = BlockTypes
	$ChunkLoader.init()
	
	$Player/DebugHelper.add_property("delay", $ChunkLoader.get_path())
	$Player/DebugHelper.add_method("Chunks to load", "get_backlog_size", $ChunkLoader.get_path())

func _exit_tree():
	for type in BlockTypes:
		type.material.unreference()