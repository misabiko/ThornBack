extends Node

var slots : Array
var selected_slot : int = 0
var SLOT_COUNT = 9*4

enum ItemTypes {EMPTY, PLACEABLE, REUSEABLE, DISPOSABLE}
signal slot_updated(slot, id, count)

func _ready():
	$"../DebugHelper".add_property("selected_slot", get_path())
	$"../DebugHelper".add_method("selected_type", "getSelectedId", get_path())
	
	slots = []
	slots.resize(SLOT_COUNT)
	
	for i in range(SLOT_COUNT):
		slots[i] = {
			"id": -1,
			"count": 0
		}

func addItem(id, count):
	var empty_slot = -1
	
	for i in range(SLOT_COUNT):
		if slots[i].id == id:
			slots[i].count += count
			emit_signal("slot_updated", i, id, slots[i].count)
			return
		elif empty_slot == -1 and slots[i].id == -1:
			empty_slot = i
	
	if empty_slot != -1:
		slots[empty_slot].id = id
		slots[empty_slot].count = count
		emit_signal("slot_updated", empty_slot, id, count)

func removeItem(slot, count):
	if slots[slot].count > count:
		slots[slot].count -= count
		emit_signal("slot_updated", slot, slots[slot].id, slots[slot].count)
	else:
		clearSlot(slot)

func removeSelectedItem(count):
	removeItem(selected_slot, count)

func clearSlot(slot):
	slots[slot].id = -1
	slots[slot].count = 0
	emit_signal("slot_updated", slot, -1, 0)

func getSelectedId():
	return slots[selected_slot].id

#TODO Store ItemTypes in BlockLibrary
func getSelectedType():
	if slots[selected_slot].id < 0:
		return ItemTypes.EMPTY
	else:
		return ItemTypes.PLACEABLE

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		match event.button_index:
			BUTTON_WHEEL_UP:
				if selected_slot != 9:
					selected_slot += 1
			BUTTON_WHEEL_DOWN:
				if selected_slot != 0:
					selected_slot -= 1