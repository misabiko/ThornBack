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
	var surfaceTool = SurfaceTool.new()
	surfaceTool.begin(Mesh.PRIMITIVE_LINES)
	surfaceTool.add_color(Color.black)
	surfaceTool.add_vertex(Vector3(-0.501, 0.501, -0.501))
	surfaceTool.add_vertex(Vector3(0.501, 0.501, -0.501))
	surfaceTool.add_vertex(Vector3(0.501, 0.501, 0.501))
	surfaceTool.add_vertex(Vector3(-0.501, 0.501, 0.501))
	
	surfaceTool.add_vertex(Vector3(-0.501, -0.501, -0.501))
	surfaceTool.add_vertex(Vector3(0.501, -0.501, -0.501))
	surfaceTool.add_vertex(Vector3(0.501, -0.501, 0.501))
	surfaceTool.add_vertex(Vector3(-0.501, -0.501, 0.501))
	
	surfaceTool.add_index(0)
	surfaceTool.add_index(1)
	
	surfaceTool.add_index(1)
	surfaceTool.add_index(2)
	
	surfaceTool.add_index(2)
	surfaceTool.add_index(3)
	
	surfaceTool.add_index(3)
	surfaceTool.add_index(0)
	###
	surfaceTool.add_index(4)
	surfaceTool.add_index(5)
	
	surfaceTool.add_index(5)
	surfaceTool.add_index(6)
	
	surfaceTool.add_index(6)
	surfaceTool.add_index(7)
	
	surfaceTool.add_index(7)
	surfaceTool.add_index(4)
	###
	surfaceTool.add_index(0)
	surfaceTool.add_index(4)
	
	surfaceTool.add_index(1)
	surfaceTool.add_index(5)
	
	surfaceTool.add_index(2)
	surfaceTool.add_index(6)
	
	surfaceTool.add_index(3)
	surfaceTool.add_index(7)
	$MeshInstance.mesh = surfaceTool.commit()
	material_override.albedo_color = Color(1, 1, 1, 0)

func collides(offset):
	$Area.translation = translation + offset;
	print($Area.translation)
	print(!$Area.get_overlapping_bodies().empty())
	print("\n")
	return !$Area.get_overlapping_bodies().empty()

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
	material_override.albedo_color = Color(1, 1, 1, 0)
