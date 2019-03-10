extends KinematicBody

const WALK_SPEED = 320
const JUMP_FORCE = 10
const FLY_SPEED = 300
const GRAVITY = -9.8
var vel = Vector3()

func _process(delta):
	var move = Vector3()
	
	if Input.is_key_pressed(KEY_W):
		move -= transform.basis.z
	if Input.is_key_pressed(KEY_S):
		move += transform.basis.z
	if Input.is_key_pressed(KEY_A):
		move -= transform.basis.x
	if Input.is_key_pressed(KEY_D):
		move += transform.basis.x
	
	move = move.normalized() * WALK_SPEED
	move.y = FLY_SPEED * (int(Input.is_key_pressed(KEY_SPACE)) - int(Input.is_key_pressed(KEY_R)))
	move_and_slide(move * delta)
#	vel.x = move.x
#	vel.z = move.z
#
#	if Input.is_key_pressed(KEY_SPACE) && vel.y == 0:
#		vel.y += JUMP_FORCE
#
#	vel.y += GRAVITY * delta
#
#	vel = move_and_slide(move.normalized() * WALK_SPEED * delta + vel)