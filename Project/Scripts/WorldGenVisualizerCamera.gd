extends Camera

var anchor := Vector3(0, 60, 0)
var rotating := false
var offset := Vector3.BACK

func _process(delta):
	translation = anchor + offset

func _input(event):
	if event is InputEventMouseButton:
		match event.button_index:
			BUTTON_MIDDLE:
				rotating = event.pressed
			BUTTON_WHEEL_UP:
				if event.pressed and offset.length() > 1:
					offset = offset.normalized() * (offset.length() - 1)
			BUTTON_WHEEL_DOWN:
				if event.pressed:
					offset = offset.normalized() * (offset.length() + 1)
					
	elif event is InputEventMouseMotion and rotating:
		pass
	elif event is InputEventKey and event.pressed:
		match event.scancode:
			KEY_SPACE:
				anchor.y += 1
			KEY_R:
				anchor.y -= 1
