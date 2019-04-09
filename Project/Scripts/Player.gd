extends KinematicBody

const WALK_SPEED = 5
const JUMP_FORCE = 8
const FLY_SPEED = 8
const GRAVITY = -30

var screen_center
var RAY_LENGTH : int = 6
var raycast_exceptions
export (NodePath) var selection_highlight_nodepath
onready var selection_highlight = get_node(selection_highlight_nodepath)
var aimed_collider
var selected_normal

var breaking : bool = false
var breaking_stage : int = 0

signal enter_chunk(coords)
signal start_breaking
signal step_breaking
signal stop_breaking

var vel = Vector3()
var flying : bool = false

func _ready():
	screen_center = get_viewport().size / 2
	raycast_exceptions = [self]
	
	$DebugHelper.add_method("Position", "debug_get_pos", get_path())
	$DebugHelper.add_method("Chunk", "get_chunk_coords", get_path())
	$DebugHelper.add_method("is_on_floor", "is_on_floor", get_path())
	$DebugHelper.add_property("breaking_stage", get_path())
	$DebugHelper.add_property("flying", get_path())

func debug_get_pos():
	return translation.floor()

func get_chunk_coords():
	return $"..".get_chunk_coords(translation)

func _physics_process(delta):
	if flying:
		process_inputs_flying()
	else:
		process_inputs(delta)
	
	if update_selection_highlight():
		stop_breaking()

func process_inputs(delta):
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
		shift_modifier = 1.5
	
	move = move.normalized() * WALK_SPEED * shift_modifier
	
	vel.x = move.x
	vel.z = move.z

	if Input.is_key_pressed(KEY_SPACE) && is_on_floor() && vel.y <= 0:
		vel.y += JUMP_FORCE
	else:
		vel.y += GRAVITY * delta

	if vel:
		var old_coords = get_chunk_coords()
		vel = move_and_slide(vel, Vector3.UP)
		
		var new_coords = get_chunk_coords()
		if new_coords != old_coords:
			emit_signal("enter_chunk", new_coords)

func process_inputs_flying():
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
		var old_coords = get_chunk_coords()
		move_and_slide(move)
		
		var new_coords = get_chunk_coords()
		if new_coords != old_coords:
			emit_signal("enter_chunk", new_coords)

func _input(event):
	if event is InputEventMouseButton:
		if event.is_pressed():
			match event.button_index:
				BUTTON_LEFT:
					update_selection_highlight()
					if aimed_collider:
						if flying:
							$"..".remove_block(selection_highlight.translation - Vector3(0.5, 0.5, 0.5))
						else:
							start_breaking()
				BUTTON_RIGHT:
					update_selection_highlight()
					if aimed_collider:
						$"..".add_block(selection_highlight.translation - Vector3(0.5, 0.5, 0.5) + selected_normal, 1)
		else:
			match event.button_index:
				BUTTON_LEFT:
					stop_breaking()
	elif event is InputEventKey:
		match event.scancode:
			KEY_SHIFT:
				var fov = $Camera.fov
				if event.is_pressed():
					if fov < 85:
						$CamZoom.interpolate_property($Camera, "fov", fov, 85, 0.5 * ((85 - fov) / 15), Tween.TRANS_EXPO, Tween.EASE_OUT)
					else:
						$Camera.fov = 85
				else:
					if fov > 70:
						$CamZoom.interpolate_property($Camera, "fov", fov, 70, 0.75 * ((fov - 70) / 15), Tween.TRANS_EXPO, Tween.EASE_OUT)
					else:
						$Camera.fov = 70
				$CamZoom.start()
			KEY_F:
				if event.is_pressed():
					flying = !flying

#Returns true if selected block changed or none is selected
func update_selection_highlight():
	var old_translation = selection_highlight.translation
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
	
	return aimed_collider == null or old_translation != selection_highlight.translation

func _on_BreakTimer_timeout():
	if breaking_stage == 9:
		$"..".remove_block(selection_highlight.translation - Vector3(0.5, 0.5, 0.5))
		stop_breaking()
	else:
		breaking_stage += 1
		emit_signal("step_breaking")

func start_breaking():
	$BreakTimer.start()
	emit_signal("start_breaking")
	breaking = true

func stop_breaking():
	if breaking:
		$BreakTimer.stop()
		emit_signal("stop_breaking")
		breaking = false
		breaking_stage = 0