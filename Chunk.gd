extends Node

const Block = preload("res://Block.tscn")
const chunkSize = 16
const worldHeight = 10

func _ready():
	for x in range(chunkSize):
		for z in range(chunkSize):
			var block = Block.instance()
			block.translate(Vector3(x, 0, z))
			add_child(block)