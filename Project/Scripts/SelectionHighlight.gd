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

onready var areas = [$AreaUp, $AreaBottom, $AreaRight, $AreaLeft, $AreaFront, $AreaRear]
var direction

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

func update(raycast_result):
	var old_translation = translation
	
	if !raycast_result.empty():
		translation = (raycast_result.position - raycast_result.normal * 0.2).floor() + Vector3(0.5, 0.5, 0.5)
		visible = true
		var selected_normal = (raycast_result.normal * 0.8).round().normalized()
		if selected_normal.y >= 0.8:
			direction = 0
		elif selected_normal.y <= -0.8:
			direction = 1
		elif selected_normal.x >= 0.8:
			direction = 2
		elif selected_normal.x <= -0.8:
			direction = 3
		elif selected_normal.z >= 0.8:
			direction = 5
		elif selected_normal.z <= -0.8:
			direction = 4
	else:
		visible = false
	
	return old_translation != translation

func collides():
	return !areas[direction].get_overlapping_bodies().empty()

func get_normal():
	match direction:
		0:
			return Vector3.UP
		1:
			return Vector3.DOWN
		2:
			return Vector3.RIGHT
		3:
			return Vector3.LEFT
		4:
			return Vector3.FORWARD
		5:
			return Vector3.BACK

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