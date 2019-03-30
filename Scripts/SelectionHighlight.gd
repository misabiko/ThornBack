extends MeshInstance

var breaking_stage = 0
var textures = [
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_0.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_1.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_2.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_3.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_4.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_5.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_6.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_7.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_8.png"),
	preload("res://newstart_textures/assets/minecraft/textures/blocks/destroy_stage_9.png")
]

func _ready():
	material_override.albedo_color = Color(1, 1, 1, 0.25)

func _on_Player_start_breaking():
	breaking_stage = 0
	material_override.albedo_texture = textures[breaking_stage]
	material_override.albedo_color = Color(1, 1, 1, 0.75)

func _on_Player_step_breaking():
	breaking_stage += 1
	material_override.albedo_texture = textures[breaking_stage]

func _on_Player_stop_breaking():
	breaking_stage = 0
	material_override.albedo_texture = null
	material_override.albedo_color = Color(1, 1, 1, 0.25)
