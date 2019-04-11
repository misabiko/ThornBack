tool
extends EditorPlugin

const BlockLibrary = preload("res://Scripts/BlockLibrary.gdns")
const WorldData = preload("res://Scripts/WorldData.gdns")

func _ready():
	add_custom_type("BlockLibrary", "Resource", BlockLibrary, null)
	add_custom_type("WorldData", "Resource", WorldData, null)
