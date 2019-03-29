extends Node2D

var quads = []
var size = 100
var unit = 10
var radius = 3
var unload_radius = 20
var loading = []
var unloading = []
var timer_i = 0
var timers = []

func _ready():
	for i in range(10):
		timers.push_back(Timer.new())
		timers.back().connect("timeout", self, "load_chunk")
		timers.back().wait_time = rand_range(0.05, 0.15)
		timers.back().start()
		add_child(timers.back())
	for i in range(10):
		timers.push_back(Timer.new())
		timers.back().connect("timeout", self, "unload_chunk")
		timers.back().wait_time = rand_range(0.05, 0.15)
		timers.back().start()
		add_child(timers.back())
	
	quads.resize(size)
	for x in range(size):
		quads[x] = []
		quads[x].resize(size)
		for y in range(size):
			quads[x][y] = ColorRect.new()
			quads[x][y].color = Color.white
			quads[x][y].rect_position = Vector2(x, y) * unit
			quads[x][y].rect_size = Vector2(unit, unit)
			add_child(quads[x][y])

func _input(event):
	if event is InputEventMouseMotion:
		var pos = (event.position / unit).floor()
		loading.clear()
		unloading.clear()
		for x in range(size):
			for y in range(size):
				if ((x - pos.x)*(x - pos.x) + (y - pos.y)*(y - pos.y)) <= 2*radius*radius:
					if quads[x][y].color != Color.black:
						loading.push_back(Vector2(x, y))
				elif quads[x][y].color == Color.black and ((x - pos.x)*(x - pos.x) + (y - pos.y)*(y - pos.y)) > 2*unload_radius*unload_radius:
					unloading.push_back(Vector2(x, y))
					
	elif event is InputEventMouseButton and event.is_pressed():
		match event.button_index:
			BUTTON_WHEEL_DOWN:
				if Input.is_key_pressed(KEY_SHIFT):
					unload_radius -= 1
				else:
					radius -= 1
			BUTTON_WHEEL_UP:
				if Input.is_key_pressed(KEY_SHIFT):
					unload_radius += 1
				else:
					radius += 1
			BUTTON_RIGHT:
				for x in range(size):
					for y in range(size):
						quads[x][y].color = Color.white
				unloading.clear()
			BUTTON_LEFT:
				print("loading: ", loading.size())
				print("radius: ", radius)
				print("unloading: ", unloading.size())
				print("unload_radius: ", unload_radius)
				print("")

func load_chunk():
	if !loading.empty():
		quads[loading.front().x][loading.front().y].color = Color.black
		loading.pop_front()

func unload_chunk():
	if !unloading.empty():
		quads[unloading.front().x][unloading.front().y].color = Color.white
		unloading.pop_front()