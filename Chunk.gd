extends Node

const Block = preload("res://Block.tscn")
onready var World = $".."
const CHUNK_SIZE = 16
const SURFACE_HEIGHT = 16
const WORLD_HEIGHT = 64

var xOffset
var zOffset

func _ready():
	for x in range(CHUNK_SIZE):
		for z in range(CHUNK_SIZE):
			var pos = Vector3(x + xOffset * CHUNK_SIZE, 0, z + zOffset * CHUNK_SIZE)
			pos.y = floor((World.noise.get_noise_2d(pos.x / 5, pos.z / 5) / 2 + 0.5) * SURFACE_HEIGHT)
			if x == 0 && z == 0:
				print(pos)
			newBlock(pos)
			
			#for y in range(pos.y):
			#	newBlock(Vector3(pos.x, y, pos.z)) 

func newBlock(pos):
	var block = Block.instance()
	block.translate(pos)
	add_child(block)

func _init(x, z):
	xOffset = x
	zOffset = z