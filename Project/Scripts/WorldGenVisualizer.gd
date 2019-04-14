tool
extends Node

export(OpenSimplexNoise) var e_noise = OpenSimplexNoise.new()
export(OpenSimplexNoise) var m_noise = OpenSimplexNoise.new()
export(int) var seed_e setget set_seed_e
export(int) var seed_m setget set_seed_m
export(int, 1, 500, 2) var width = 16 setget set_width
export(int, 1, 15) var radius = 1 setget set_radius
export(bool) var do_floor := false setget set_do_floor
export(bool) var show_map := true setget set_show_map
export(bool) var show_moisture := false setget set_show_moisture
export(bool) var show_moisture_h := true setget set_show_moisture_h
export(float, 0.01, 3) var height = 1 setget set_height
export(float, 1, 5) var freq1 = 1 setget set_freq1
export(float, 1, 5) var freq2 = 1 setget set_freq2
export(float, 1, 5) var freq3 = 1 setget set_freq3
export(float, 1, 5) var freq4 = 1 setget set_freq4
export(float, -1, 1) var amp1 = 0.1 setget set_amp1
export(float, -1, 1) var amp2 = 0.1 setget set_amp2
export(float, -1, 1) var amp3 = 0.1 setget set_amp3
export(float, 0, 2) var amp4 = 0.1 setget set_amp4
export(float, -1, 1) var mult2 = 0 setget set_mult2
export(float, -1, 1) var mult3 = 0 setget set_mult3
export(bool) var enable1 = true setget set_enable1
export(bool) var enable2 = false setget set_enable2
export(bool) var enable3 = false setget set_enable3
export(float, 0, 1) var snow = 0.8 setget set_snow
export(float, 0, 1) var stone = 0.7 setget set_stone
export(bool) var use_water = true setget set_use_water
export(float, 0, 1) var water = 0.3 setget set_water
export(float, 0, 1) var deep_water = 0.2 setget set_deep_water
export(float, 0, 1) var beach = 0.35 setget set_beach
export(float, EASE) var dist1 setget set_dist1
export(float, -1, 1) var dist1_offset setget set_dist1_offset
export(float, EASE) var dist2 setget set_dist2
export(float, -1, 1) var dist2_offset setget set_dist2_offset
export(float, EASE) var dist3 setget set_dist3
export(float, -1, 1) var dist3_offset setget set_dist3_offset
export(float, EASE) var dist_mult2 setget set_dist_mult2
export(float, -1, 1) var dist_mult2_offset setget set_dist_mult2_offset
export(float, EASE) var dist_mult3 setget set_dist_mult3
export(float, -1, 1) var dist_mult3_offset setget set_dist_mult3_offset
export(float, EASE) var dist_post setget set_dist_post
export(bool) var use_dist_post setget set_use_dist_post
export(float, -1, 1) var dist_post_offset setget set_dist_post_offset
export(float, EASE) var distm setget set_distm
export(bool) var use_distm setget set_use_distm
export(float, -1, 1) var dist_offsetm setget set_dist_offsetm
export(bool) var do_update = true setget set_do_update
export(int, "Custom", "Mountains", "Plains") var mode setget set_mode

func _ready():
	update()

func update():
	if do_update && has_node("Map"):
		match mode:
			0:
				custom()
			1:
				mountains()
			2:
				plains()

func set_mode(new_mode):
	mode = new_mode
	update()

func custom():
	if show_map:
		for i in range(width * radius):
			for j in range(width * radius):
				var n = Vector2(i, j) / width - Vector2.ONE * 0.5 * radius
				var e : float
				var color : Color
				var c : float = 1
				var m = ease(amp4 * (0.5 + m_noise.get_noise_2dv(freq4 * n) / 2) + dist_offsetm, distm) * 2 - 1
				
				var e1 = amp1 * (0.5 + e_noise.get_noise_2dv(freq1 * n) / 2)
				e1 = ease(e1 + dist1_offset, dist1)
				
				var e2 = (amp2 * e_noise.get_noise_2dv(freq2 * n) * (
					ease(e1 - dist_mult2_offset, dist_mult2) * mult2 if mult2 > 0 else (
						ease((1 - e1) - dist_mult2_offset, dist_mult2) * (-mult2) if mult2 < 0 else 1
					)
				))
				e2 = ease((e2 + 1) / 2 + dist2_offset, dist2) * 2 - 1
				
				var e3 = (amp3 * e_noise.get_noise_2dv(freq3 * n) * (ease(e1 - dist_mult3_offset, dist_mult3) * mult3 if mult3 > 0 else (ease((1 - e1) - dist_mult3_offset, dist_mult3) * (-mult3) if mult3 < 0 else 1)))
				e3 = ease((e3 + 1) / 2 + dist3_offset, dist3) * 2 - 1
				
				e = (
						(e1 if enable1 else 0) +
						(e2 if enable2 else 0) +
						(e3 if enable3 else 0)
					)
				
				if use_dist_post:
					e = ease(e + dist_post_offset, dist_post)
				
				color = biome(e, m)
				if use_water and e < water:
					e = water
				
				$Map.multimesh.set_instance_color(i + width * radius * j, color)
				$Map.multimesh.set_instance_transform(
					i + width * radius * j,
					Transform().translated(Vector3(
						i - width * radius / 2,
						try_floor(e * height * width),
						j - width * radius / 2
					)))
	if show_moisture:
		for i in range(width * radius):
			for j in range(width * radius):
				var n = Vector2(i, j) / width - Vector2.ONE * 0.5 * radius
				var e : float
				var color : Color
				var c : float = 1
				var m = amp4 * m_noise.get_noise_2dv(freq4 * n)
				m = ease(m + dist_offsetm, distm)
				
				if show_moisture_h:
					e = m
					c = (e + 1) / 2
				else:
					c = (m + 1) / 2
				color = Color(c, c, c)
				
				$MoistureMap.multimesh.set_instance_color(i + width * radius * j, color)
				$MoistureMap.multimesh.set_instance_transform(
					i + width * radius * j,
					Transform().translated(Vector3(
						i - width * radius / 2,
						try_floor(e * height * width),
						j - width * radius / 2
					)))

func plains():
	for i in range(width * radius):
		for j in range(width * radius):
			var n = Vector2(i, j) / width - Vector2.ONE * 0.5 * radius
			var m = ease(0.1 * (0.5 + m_noise.get_noise_2dv(n) / 2) + 0.454, 1) * 2 - 1
			
			var e1 = ease((0.5 + e_noise.get_noise_2dv(7.567 * n) / 2) + 0.02, -8.57)
			
			var e2 = ease(((0.244 * e_noise.get_noise_2dv(9.588 * n) * e1) + 1) / 2 + 0.041, -3.36) * 2 - 1
			
			var e3 = ease(((0.182 * e_noise.get_noise_2dv(25.246 * n) * max(e1 - 0.433, 0)) + 1) / 2 , -1.57) * 2 - 1
			
			var e = e2 + e3
			
			var color = biome(e, m)
			if use_water and e < water:
				e = water
			
			$Map.multimesh.set_instance_color(i + width * radius * j, color)
			$Map.multimesh.set_instance_transform(
				i + width * radius * j,
				Transform().translated(Vector3(
					i - width * radius / 2,
					try_floor(e * height * width),
					j - width * radius / 2
				)))

func mountains():
	for i in range(width * radius):
		for j in range(width * radius):
			var n = Vector2(i, j) / width - Vector2.ONE * 0.5 * radius
			var e : float
			var color : Color
			var c : float = 1
			var m = ease(amp4 * (0.5 + m_noise.get_noise_2dv(freq4 * n) / 2) + dist_offsetm, distm) * 2 - 1
			
			var e1 = ease(0.5 + e_noise.get_noise_2dv(6.788 * n) / 2 - 0.089, -4)
			
			var e2 = 0.637 * e_noise.get_noise_2dv(25.566 * n) * e1 * mult2
			
			var e3 = (0.09 * e_noise.get_noise_2dv(33.42 * n) * ease((1 - e1) - 0.406, -2))
			
			e = ease(e1 + e2 + e3 - 0.015, -0.93)
			
			color = biome(e, m)
			if use_water and e < water:
				e = water
			
			$Map.multimesh.set_instance_color(i + width * radius * j, color)
			$Map.multimesh.set_instance_transform(
				i + width * radius * j,
				Transform().translated(Vector3(
					i - width * radius / 2,
					try_floor(e * height * width),
					j - width * radius / 2
				)))

func biome(e, m):
	if use_water and e < deep_water:
		return Color("#1904b7")
	elif use_water and e < water:
		return Color("#0449b7")
	elif e < beach + m:
		return Color("#f8dfb2")
	elif e >= snow + m:
		return Color.white
	elif e >= stone + m:
		return Color.gray
	else:
		return Color(0, 0.52, 0.125, 1)

func set_seed_e(new_seed):
	seed_e = new_seed
	e_noise.seed = seed_e
	update()
func set_seed_m(new_seed):
	seed_m = new_seed
	m_noise.seed = seed_m
	update()

func set_do_floor(new_do_floor):
	do_floor = new_do_floor
	update()
func set_show_map(new_show_map):
	show_map = new_show_map
	$Map.visible = show_map
	update()
func set_show_moisture(new_show_moisture):
	show_moisture = new_show_moisture
	$MoistureMap.visible = show_moisture
	update()
func set_show_moisture_h(new_show_moisture_h):
	show_moisture_h = new_show_moisture_h
	update()

func set_width(new_width):
	width = new_width
	if has_node("Map"):
		$Map.multimesh.instance_count = width * width * radius * radius
		$MoistureMap.multimesh.instance_count = width * width * radius * radius
	update()
func set_height(new_height):
	height = new_height
	update()
func set_radius(new_radius):
	radius = new_radius
	if has_node("Map"):
		$Map.multimesh.instance_count = width * width * radius * radius
		$MoistureMap.multimesh.instance_count = width * width * radius * radius
	update()
	
func set_mult2(new_mult):
	mult2 = new_mult
	update()
func set_mult3(new_mult):
	mult3 = new_mult
	update()

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
func set_freq4(new_freq):
	freq4 = new_freq
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
func set_amp4(new_amp):
	amp4 = new_amp
	update()

func set_dist1(new_dist):
	dist1 = new_dist
	update()
func set_dist2(new_dist):
	dist2 = new_dist
	update()
func set_dist3(new_dist):
	dist3 = new_dist
	update()
func set_dist_mult2(new_dist):
	dist_mult2 = new_dist
	update()
func set_dist_mult3(new_dist):
	dist_mult3 = new_dist
	update()
func set_dist_post(new_dist):
	dist_post = new_dist
	update()
func set_distm(new_distm):
	distm = new_distm
	update()

func set_use_dist_post(new_use_dist):
	use_dist_post = new_use_dist
	update()
func set_use_distm(new_use_distm):
	use_distm = new_use_distm
	update()

func set_dist1_offset(new_dist_offset):
	dist1_offset = new_dist_offset
	update()
func set_dist2_offset(new_dist_offset):
	dist2_offset = new_dist_offset
	update()
func set_dist3_offset(new_dist_offset):
	dist3_offset = new_dist_offset
	update()
func set_dist_mult2_offset(new_dist_offset):
	dist_mult2_offset = new_dist_offset
	update()
func set_dist_mult3_offset(new_dist_offset):
	dist_mult3_offset = new_dist_offset
	update()
func set_dist_post_offset(new_dist_offset):
	dist_post_offset = new_dist_offset
	update()
func set_dist_offsetm(new_dist_offsetm):
	dist_offsetm = new_dist_offsetm
	update()

func set_use_water(new_use_water):
	use_water = new_use_water
	update()

func set_snow(new_snow):
	snow = new_snow
	update()
func set_water(new_water):
	water = new_water
	update()
func set_deep_water(new_deep_water):
	deep_water = new_deep_water
	update()
func set_stone(new_stone):
	stone = new_stone
	update()
func set_beach(new_beach):
	beach = new_beach
	update()

func set_do_update(new_do_update):
	do_update = new_do_update
	if do_update:
		update()
func try_floor(value):
	if do_floor:
		return floor(value)
	else:
		return value



"""
Nice mountains with lakes
[node name=""WorldGenVisualizer"" type=""Node""]
script = ExtResource( 1 )
e_noise = SubResource( 1 )
m_noise = SubResource( 2 )
seed_e = 5
seed_m = 1
width = 2
do_floor = true
height = 0.182
freq1 = 6.788
freq2 = 25.566
freq3 = 33.42
freq4 = 25.214
amp1 = 1.0
amp2 = 0.637
amp3 = 0.09
amp4 = 0.049
mult2 = 1.0
mult3 = -1.0
enable2 = true
enable3 = true
snow = 0.928
stone = 0.8
water = 0.224
deep_water = 0.112
beach = 0.283
dist1 = -4.0
dist1_offset = -0.089
dist2 = 1.0
dist3 = 1.0
dist_mult2 = 1.0
dist_mult3 = -2.0
dist_mult3_offset = 0.406
dist_post = -0.933033
dist_post_offset = -0.015
distm = 0.287175
use_distm = true

Decent plains
[node name=""WorldGenVisualizer"" type=""Node""]
script = ExtResource( 1 )
e_noise = SubResource( 1 )
m_noise = SubResource( 2 )
seed_e = 8
seed_m = 2
width = 2
do_floor = true
height = 0.153
freq1 = 7.567
freq2 = 9.588
freq3 = 25.246
amp1 = 1.0
amp2 = 0.244
amp3 = 0.182
mult2 = 1.0
mult3 = 1.0
enable1 = false
enable2 = true
enable3 = true
snow = 0.95
stone = 0.9
water = 0.1
deep_water = 0.08
beach = 0.12
dist1 = -8.57419
dist1_offset = 0.02
dist2 = -3.36358
dist2_offset = 0.041
dist3 = -1.56917
dist3_offset = -0.001
dist_mult2 = 1.0
dist_mult3 = 1.0
dist_mult3_offset = 0.433
dist_post = 1.0
distm = 1.0
dist_offsetm = 0.454
"""