extends Node

const Chunk = preload("res://Scripts/Chunk.gd")
var noise = OpenSimplexNoise.new()

func _ready():
	noise.seed = 43
	noise.octaves = 4
	noise.period = 20.0
	noise.persistence = 0.8
	
	var world_radius = 1
	
	for x in range(-world_radius, world_radius + 1):
		for y in range(-world_radius, world_radius + 1):
			print("Generating chunk [", x, ", ", y, "]...")
			add_child(Chunk.new(x, y))