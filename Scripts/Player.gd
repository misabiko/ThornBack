extends KinematicBody

const WALK_SPEED = 10
const JUMP_FORCE = 10
const FLY_SPEED = 8
const GRAVITY = -9.8

var screen_center
const RAY_LENGTH : int = 30

#var vel = Vector3()
var debug_object = {}

func _ready():
	screen_center = get_viewport().size / 2
	print("screen_center: ", screen_center)

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
	var result = space_state.intersect_ray(from, from + $Camera.project_ray_normal(screen_center) * RAY_LENGTH, [self])
	update_debug_label($RayCastLabel, result)