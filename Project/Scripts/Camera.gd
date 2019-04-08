extends Camera
var sensitivity = 0.005
var angle_cap = PI / 2
var basis_up = Basis(Vector3.RIGHT, Vector3.BACK, Vector3.DOWN)
var basis_down = Basis(Vector3.RIGHT, Vector3.FORWARD, Vector3.UP)

func _input(event):
	if event is InputEventMouseButton:
		if event.button_index == BUTTON_LEFT:
			Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED)
	elif event is InputEventMouseMotion and Input.get_mouse_mode() == Input.MOUSE_MODE_CAPTURED:
		get_parent().rotate_y(-event.relative.x * sensitivity)
		rotate_x(-event.relative.y * sensitivity)
		var angle = transform.basis.get_euler().x
		if -event.relative.y > 0 and angle > angle_cap:
			transform.basis = basis_up
		elif -event.relative.y < 0 and angle < -angle_cap:
			transform.basis = basis_down