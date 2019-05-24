extends RigidBody

var anim_step : float = 0
onready var mesh_instance = $MeshInstance

var id : int = 0

func _process(delta):
	anim_step += 2.5 * delta - (2*PI if anim_step > 2*PI else 0)
	mesh_instance.translation.y = 0.1 * sin(anim_step)
	mesh_instance.rotate_y(0.25 * delta)

func _on_Area_body_entered(body):
	if body.name == "Player":
		if body.try_pick_up(id):
			queue_free()
