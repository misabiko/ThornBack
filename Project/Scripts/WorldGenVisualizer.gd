tool
extends Node

export(OpenSimplexNoise) var noise1 = OpenSimplexNoise.new()
export(OpenSimplexNoise) var noise2 = OpenSimplexNoise.new()
export(OpenSimplexNoise) var noise3 = OpenSimplexNoise.new()
#export(int, 0, 1) var test_num setget set_test_num
export(int, 1, 500, 2) var width = 16 setget set_width
export(bool) var do_floor := false setget set_do_floor
#export(float, -1, 0) var min_height = 0 setget set_min_height
#export(float, 0, 1) var max_height = 1 setget set_max_height
export(float, 1, 50) var freq1 = 1 setget set_freq1
export(float, 1, 50) var freq2 = 1 setget set_freq2
export(float, 1, 50) var freq3 = 1 setget set_freq3
export(float, 0.1, 50) var amp1 = 1 setget set_amp1
export(float, 0.1, 50) var amp2 = 1 setget set_amp2
export(float, 0.1, 50) var amp3 = 1 setget set_amp3
export(bool) var enable1 = true setget set_enable1
export(bool) var enable2 = false setget set_enable2
export(bool) var enable3 = false setget set_enable3

func _ready():
	update()

func update():
	for i in range(width):
		for j in range(width):
			var n = Vector2(i, j) / width - Vector2(0.5, 0.5)
			$MultiMeshInstance.multimesh.set_instance_transform(
				i + width * j,
				Transform().translated(Vector3(
					i - width/2,
					try_floor(
						(amp1 * (0.5 + noise1.get_noise_2dv(freq1 * n) / 2) if enable1 else 0) +
						(amp2 * (0.5 + noise2.get_noise_2dv(freq2 * n) / 2) if enable2 else 0) +
						(amp3 * (0.5 + noise3.get_noise_2dv(freq3 * n) / 2) if enable3 else 0)
					),
					j - width/2
				)))	

#func set_test_num(new_test_num):
#	test_num = new_test_num
#	update()

func set_do_floor(new_do_floor):
	do_floor = new_do_floor
	update()

func set_width(new_width):
	width = new_width
	$MultiMeshInstance.multimesh.instance_count = width * width
	update()

#func set_min_height(new_min_height):
#	min_height = new_min_height
#	update()
#
#func set_max_height(new_max_height):
#	max_height = new_max_height
#	update()
	
func set_enable1(new_enable):
	enable1 = new_enable
	update()
	
func set_enable2(new_enable):
	enable2 = new_enable
	update()
	
func set_enable3(new_enable):
	enable3 = new_enable
	update()
	
func set_freq1(new_freq):
	freq1 = new_freq
	update()
	
func set_freq2(new_freq):
	freq2 = new_freq
	update()
	
func set_freq3(new_freq):
	freq3 = new_freq
	update()
	
func set_amp1(new_amp):
	amp1 = new_amp
	update()
	
func set_amp2(new_amp):
	amp2 = new_amp
	update()
	
func set_amp3(new_amp):
	amp3 = new_amp
	update()

func try_floor(value):
	if do_floor:
		return floor(value)
	else:
		return value