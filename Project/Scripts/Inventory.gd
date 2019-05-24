extends Node

var slots : Array
var selected_slot : int = 0 setget set_selected_slot
var SLOT_COUNT = 9*4

enum ItemTypes {EMPTY, PLACEABLE, REUSEABLE, DISPOSABLE}
signal slot_updated(slot, id, count)
signal changed_selected_slot(last_slot, new_slot)

func _ready():	
	slots = []
	slots.resize(SLOT_COUNT)
	
	for i in range(SLOT_COUNT):
		slots[i] = {
			"id": -1,
			"count": 0
		}

func add_item(id, count):
	if slots[selected_slot].id == -1:
		slots[selected_slot].id = id
		slots[selected_slot].count = count
		emit_signal("slot_updated", selected_slot, id, count)
	elif slots[selected_slot].id == id:
		slots[selected_slot].count += count
		emit_signal("slot_updated", selected_slot, id, slots[selected_slot].count)
	else:
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

func remove_item(slot, count):
	if slots[slot].count > count:
		slots[slot].count -= count
		emit_signal("slot_updated", slot, slots[slot].id, slots[slot].count)
	else:
		clear_slot(slot)

func remove_selected_item(count):
	remove_item(selected_slot, count)

func clear_slot(slot):
	slots[slot].id = -1
	slots[slot].count = 0
	emit_signal("slot_updated", slot, -1, 0)

func get_selectedId():
	return slots[selected_slot].id

#TODO Store ItemTypes in BlockLibrary
func get_selected_type():
	if slots[selected_slot].id < 0:
		return ItemTypes.EMPTY
	else:
		return ItemTypes.PLACEABLE

func _input(event):
	if event is InputEventMouseButton and event.is_pressed():
		match event.button_index:
			BUTTON_WHEEL_UP:
				set_selected_slot(selected_slot + 1 if selected_slot != 8 else 0)
			BUTTON_WHEEL_DOWN:
				set_selected_slot(selected_slot - 1 if selected_slot != 0 else 8)
	elif event is InputEventKey and event.is_pressed():
		match event.scancode:
			KEY_1:
				set_selected_slot(0)
			KEY_2:
				set_selected_slot(1)
			KEY_3:
				set_selected_slot(2)
			KEY_4:
				set_selected_slot(3)
			KEY_5:
				set_selected_slot(4)
			KEY_6:
				set_selected_slot(5)
			KEY_7:
				set_selected_slot(6)
			KEY_8:
				set_selected_slot(7)
			KEY_9:
				set_selected_slot(8)

func set_selected_slot(slot):
	var last_slot = selected_slot
	selected_slot = slot
	emit_signal("changed_selected_slot", last_slot, selected_slot)