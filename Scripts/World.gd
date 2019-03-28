extends Node

const Chunk = preload("res://GDNative/bin/Chunk.gdns")
var noise = OpenSimplexNoise.new()
var BlockTypes = preload("res://Scripts/Block.gd").BlockTypes

func _ready():
	noise.seed = 43
	noise.octaves = 4
	noise.period = 20.0
	noise.persistence = 0.8
	
	var world_radius = 2
	
	$ChunkLoader.block_types = BlockTypes
	$ChunkLoader.noise = noise