extends Node

const Chunk = preload("res://Chunk.tscn")

func _ready():
	for x in range(3):
		for y in range(3):
			var chunk = Chunk.instance()
			chunk.translate(Vector3(x, 0, y) * chunk.chunkSize)
			add_child(chunk)