extends Node

var noise = OpenSimplexNoise.new()
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
	noise.seed = 43
	noise.octaves = 4
	noise.period = 20.0
	noise.persistence = 0.8
	
	$ChunkLoader.block_types = BlockTypes
	$ChunkLoader.noise = noise
	$ChunkLoader.init()
	
	$Player/DebugHelper.add_property("delay", $ChunkLoader.get_path())
	$Player/DebugHelper.add_method("Chunks to load", "get_backlog_size", $ChunkLoader.get_path())0

func _exit_tree():
	for type in BlockTypes:
		type.material.unreference()