extends Node

const Block = preload("res://Block.tscn")
onready var World = $".."
const CHUNK_SIZE = 16
const WORLD_HEIGHT = 10

var xOffset
var zOffset

func _ready():
	for x in range(CHUNK_SIZE):
		for z in range(CHUNK_SIZE):
			var block = Block.instance()
			var pos = Vector2(x + xOffset * CHUNK_SIZE, z + zOffset * CHUNK_SIZE)
			block.translate(Vector3(pos.x, floor(World.noise.get_noise_2dv(pos) * 4), pos.y))
			add_child(block)

func _init(x, z):
	xOffset = x
	zOffset = z