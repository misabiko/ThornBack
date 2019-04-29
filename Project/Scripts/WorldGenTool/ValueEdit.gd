extends HBoxContainer

export(String) var property
export(NodePath) var node_path = "/root/WorldGenVisualizer"
export(bool) var auto_update = true
export(float) var min_value = 0
export(float) var max_value = 1
export(float) var step = 0.01
var node : Node


func _ready():
	node = get_node(node_path)
	$Label.text = property
	$HSlider.min_value = min_value
	$HSlider.max_value = max_value
	$HSlider.step = step

func _on_HSlider_value_changed(value):
	node.set(property, value)
	if auto_update:
		node.update()
