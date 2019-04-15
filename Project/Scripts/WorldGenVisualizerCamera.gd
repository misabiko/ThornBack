extends Spatial

var anchor := Vector3(0, 60, 0)
var rotating := false
var offset := Vector3.BACK * 80

func _process(delta):
	#translation = anchor + offset
	pass

func _input(event):
	if event is InputEventMouseButton:
		match event.button_index:
			BUTTON_MIDDLE:
				rotating = event.pressed
				Input.set_mouse_mode(Input.MOUSE_MODE_CAPTURED if rotating else Input.MOUSE_MODE_VISIBLE)
			BUTTON_WHEEL_UP:
				if event.pressed and $Camera.translation.z > 5:
					$Camera.translation.z -= 1
			BUTTON_WHEEL_DOWN:
				if event.pressed:
					$Camera.translation.z += 1
					
	elif event is InputEventMouseMotion and rotating:
		rotate_y(-event.relative.x * 0.005)
		$Camera.rotate(transform.basis.x, -event.relative.y * 0.005)
	elif event is InputEventKey and event.pressed:
		match event.scancode:
			KEY_SPACE:
				anchor.y += 1
			KEY_R:
				anchor.y -= 1
