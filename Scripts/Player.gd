extends KinematicBody

const WALK_SPEED = 10
const JUMP_FORCE = 10
const FLY_SPEED = 8
const GRAVITY = -9.8

var screen_center
var RAY_LENGTH : int = 6
var raycast_exceptions
export (NodePath) var selection_highlight_nodepath
onready var selection_highlight = get_node(selection_highlight_nodepath)
var aimed_collider
var selected_normal

signal enter_chunk(coords)

#var vel = Vector3()
var debug_object = {}

func _ready():
	screen_center = get_viewport().size / 2
	raycast_exceptions = [self]
	
	update_debug()

func _process(delta):
	process_inputs()
	update_debug_label($DebugLabel, debug_object)
#	vel.x = move.x
#	vel.z = move.z
#
#	if Input.is_key_pressed(KEY_SPACE) && vel.y == 0:
#		vel.y += JUMP_FORCE
#
#	vel.y += GRAVITY * delta
#
#	vel = move_and_slide(move.normalized() * WALK_SPEED * delta + vel)

func process_inputs():
	var move = Vector3()
	var shift_modifier = 1
	
	if Input.is_key_pressed(KEY_W):
		move -= transform.basis.z
	if Input.is_key_pressed(KEY_S):
		move += transform.basis.z
	if Input.is_key_pressed(KEY_A):
		move -= transform.basis.x
	if Input.is_key_pressed(KEY_D):
		move += transform.basis.x
	
	if Input.is_key_pressed(KEY_SHIFT):
		shift_modifier = 3
	
	move = move.normalized() * WALK_SPEED * shift_modifier
	move.y = shift_modifier * FLY_SPEED * (int(Input.is_key_pressed(KEY_SPACE)) - int(Input.is_key_pressed(KEY_R)))
	
	if move:
		var old_coords = get_chunk_coord()
		move_and_slide(move)
		
		var new_coords = get_chunk_coord()
		if new_coords != old_coords:
			emit_signal("enter_chunk", new_coords)
			
		update_debug()

func update_debug_label(label, object):
	var debug_text = ""
	for key in object:
		debug_text += key + ": " + str(object[key]) + "\n"
	
	label.text = debug_text

func _physics_process(delta):
	update_selection_highlight()

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		match event.button_index:
			BUTTON_LEFT:
				update_selection_highlight()
				if aimed_collider:
					var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5))
					aimed_collider.get_parent().clear_block(selectedPos.x, selectedPos.y, selectedPos.z)
			BUTTON_RIGHT:
				update_selection_highlight()
				if aimed_collider:
					var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5) + selected_normal)
					aimed_collider.get_parent().set_block(selectedPos.x, selectedPos.y, selectedPos.z, 1)

func world_to_chunk(pos):
	return Vector3(
		0 if fmod(pos.x, 16) == 0 else (16 + fmod(pos.x, 16) if pos.x < 0 else fmod(pos.x, 16)),
		pos.y,
		0 if fmod(pos.z, 16) == 0 else (16 + fmod(pos.z, 16) if pos.z < 0 else fmod(pos.z, 16))
	)

func get_chunk_coord():
	return Vector2(translation.x / 16, translation.z / 16).floor()

func update_debug():
	debug_object["Position"] = translation.floor()
	debug_object["Chunk"] = get_chunk_coord()

func update_selection_highlight():
	var space_state = get_world().direct_space_state
	var from = $Camera.project_ray_origin(screen_center)
	var result = space_state.intersect_ray(from, from + $Camera.project_ray_normal(screen_center) * RAY_LENGTH, raycast_exceptions)
	
	if !result.empty():
		selection_highlight.translation = (result.position - result.normal * 0.2).floor() + Vector3(0.5, 0.5, 0.5)
		selection_highlight.visible = true
		aimed_collider = result.collider
		selected_normal = (result.normal * 0.8).round().normalized()
	else:
		selection_highlight.visible = false
		aimed_collider = null