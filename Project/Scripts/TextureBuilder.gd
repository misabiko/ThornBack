tool
extends EditorScript

const top = preload("res://newstart_textures/assets/minecraft/textures/blocks/grass_top.png")
const side = preload("res://newstart_textures/assets/minecraft/textures/blocks/grass_side.png")
const bottom = preload("res://newstart_textures/assets/minecraft/textures/blocks/dirt.png")

func _run():
	var img = Image.new()
	img.copy_from(top.get_data())
	
	img.save_png("res://Resources/Blocks/grass_top.stex")
	
#func _run():
#	var img = Image.new()
#	img.copy_from(top.get_data())
#
#	var top_data = top.get_data()
#	var side_data = side.get_data()
#	var bottom_data = bottom.get_data()
#	var width = top.get_width()
#	var height = top.get_height()
#	var src_rect = Rect2(0, 0, width, height)
#
#	img.create(width * 3, height * 2, false, Image.FORMAT_RGBA8)
#
#	img.blit_rect(top_data, src_rect, Vector2())
#	img.blit_rect(bottom_data, src_rect, Vector2(width, 0))
#
#	img.blit_rect(side_data, src_rect, Vector2(width * 2, 0))
#	img.blit_rect(side_data, src_rect, Vector2(0, height))
#
#	img.blit_rect(side_data, src_rect, Vector2(width, height))
#	img.blit_rect(side_data, src_rect, Vector2(width * 2, height))
#
#	print(ResourceSaver.save("res://Resources/Blocks/grass.stex", img))
