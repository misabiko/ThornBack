extends Camera
var sensitivity = 0.005

func _input(event):
	if event is InputEventMouseButton:
		match event.button_index:
			BUTTON_LEFT:
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
			BUTTON_RIGHT:
				Input.set_mouse_mode(Input.MOUSE_MODE_VISIBLE)
	elif event is InputEventMouseMotion:
		get_parent().rotate_y(-event.relative.x * sensitivity)
		rotate_x(-event.relative.y * sensitivity)