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
var debug_object = {}
var flying : bool = false

func _ready():
	screen_center = get_viewport().size / 2
	raycast_exceptions = [self]
	
	update_debug()

func _process(delta):
	update_debug_label($DebugLabel, debug_object)

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
		var old_coords = get_chunk_coord()
		vel = move_and_slide(vel, Vector3.UP)
		
		var new_coords = get_chunk_coord()
		if new_coords != old_coords:
			emit_signal("enter_chunk", new_coords)
			
		update_debug()

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

func _input(event):
	if event is InputEventMouseButton:
		if event.is_pressed():
			match event.button_index:
				BUTTON_LEFT:
					update_selection_highlight()
					if aimed_collider:
						start_breaking()
				BUTTON_RIGHT:
					update_selection_highlight()
					if aimed_collider:
						var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5) + selected_normal)
						aimed_collider.get_parent().set_block(selectedPos.x, selectedPos.y, selectedPos.z, 1)
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
	debug_object["is_on_floor"] = is_on_floor()
	debug_object["breaking_stage"] = breaking_stage
	debug_object["flying"] = flying

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
		var selectedPos = world_to_chunk(selection_highlight.translation - Vector3(0.5, 0.5, 0.5))
		aimed_collider.get_parent().clear_block(selectedPos.x, selectedPos.y, selectedPos.z)
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