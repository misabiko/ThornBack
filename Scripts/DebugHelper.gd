extends Label

var properties = {}
var methods = {}

func get_str():
	var debug_text = ""
	for key in properties:
		debug_text += key + ": " + str(get_node(properties[key]).get(key)) + "\n"
	for key in methods:
		debug_text += key + ": " + str(get_node(methods[key].path).call(methods[key].method)) + "\n"
	
	return debug_text

func add_property(name, path):
	properties[name] = path

func add_method(name, method, path):
	methods[name] = {
		"method": method,
		"path": path
	}

func _process(delta):
	text = get_str()