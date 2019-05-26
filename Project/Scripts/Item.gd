extends RigidBody

var anim_step : float = 0
onready var mesh_instance = $MeshInstance

var id : int = 0

func _process(delta):
	anim_step += 2.5 * delta - (2*PI if anim_step > 2*PI else 0.0)
	mesh_instance.translation.y = 0.1 * sin(anim_step)
	mesh_instance.rotate_y(0.25 * delta)

func _on_Area_body_entered(body):
	if body.name == "Player":
		if body.try_pick_up(id):
			$CollisionShape.disabled = true
			mode = MODE_STATIC
			$Tween.interpolate_property(self, "translation", translation, body.translation, 0.25, Tween.TRANS_CUBIC, Tween.EASE_IN)
			$Tween.start()


func _on_Tween_tween_completed(object, key):
	queue_free()
