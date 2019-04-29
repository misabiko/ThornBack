extends Spatial

var anchor := Vector3(0, 60, 0)
var rotating := false
var offset := Vector3.BACK * 80
var rot_x = 0
var rot_y = 0

func _process(delta):
	translation.y += delta * 50 * (int(Input.is_key_pressed(KEY_SPACE)) - int(Input.is_key_pressed(KEY_R)))

func _input(event):
	if event is InputEventMouseButton:
		match event.button_index:
			BUTTON_MIDDLE:
				rotating = event.pressed
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED if rotating else Input.MOUSE_MODE_VISIBLE)
			BUTTON_WHEEL_UP:
				if event.pressed and $Camera.translation.z > 5:
					$Camera.translation.z -= 3
			BUTTON_WHEEL_DOWN:
				if event.pressed:
					$Camera.translation.z += 3
					
	elif event is InputEventMouseMotion and rotating:
		rot_x -= event.relative.x * 0.005
		rot_y -= event.relative.y * 0.005
		
		transform.basis = Basis()
		rotate_object_local(Vector3.UP, rot_x)
		rotate_object_local(Vector3.RIGHT, rot_y)
