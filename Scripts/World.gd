extends Node

const Chunk = preload("res://GDNative/bin/Mesher.gdns")
const ChunkGD = preload("res://Scripts/Chunk.gd")
var noise = OpenSimplexNoise.new()
var BlockTypes = preload("res://Scripts/Block.gd").BlockTypes
var material = preload("res://Materials/block_material.tres")

func _ready():
	noise.seed = 43
	noise.octaves = 4
	noise.period = 20.0
	noise.persistence = 0.8
	
	var world_radius = 9
	
	var chunk
	#var chunkGD
	var startTime = OS.get_ticks_msec()
	for x in range(-world_radius, world_radius + 1):
		for y in range(-world_radius, world_radius + 1):
			print("Generating chunk [", x, ", ", y, "]...")
			#chunkGD = ChunkGD.new(x, y + 1)
			#add_child(chunkGD)
			chunk = Chunk.new()
			chunk.block_types = BlockTypes
			chunk.noise = noise
			chunk.material_override = material
			chunk.init(x, y)
			add_child(chunk)
	
	print("Total time: ", OS.get_ticks_msec() - startTime)