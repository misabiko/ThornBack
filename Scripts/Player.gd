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

#var vel = Vector3()
var debug_object = {}

func _ready():
	screen_center = get_viewport().size / 2
	raycast_exceptions = [self]

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
		move_and_slide(move)
		
		debug_object["Position"] = translation.floor()

func update_debug_label(label, object):
	var debug_text = ""
	for key in object:
		debug_text += key + ": " + str(object[key]) + "\n"
	
	label.text = debug_text

func _physics_process(delta):
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

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		match event.button_index:
			BUTTON_LEFT:
				if aimed_collider:
					var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5) + selected_normal)
					print("Placing ", selectedPos)
					aimed_collider.get_parent().set_block(int(selectedPos.x) % 16, selectedPos.y, int(selectedPos.z) % 16, 1)
			BUTTON_RIGHT:
				if aimed_collider:
					var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5))
					print("Removing ", selectedPos)
					aimed_collider.get_parent().clear_block(int(selectedPos.x) % 16, selectedPos.y, int(selectedPos.z) % 16)

func world_to_chunk(pos):
	return Vector3(
		16 + fmod(pos.x, 16) if pos.x < 0 else fmod(pos.x, 16),
		pos.y,
		16 + fmod(pos.z, 16) if pos.z < 0 else fmod(pos.z, 16)
	)