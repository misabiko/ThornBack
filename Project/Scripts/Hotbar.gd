extends MarginContainer

onready var slots = [
	$HBoxContainer/Slot,
	$HBoxContainer/Slot2,
	$HBoxContainer/Slot3,
	$HBoxContainer/Slot4,
	$HBoxContainer/Slot5,
	$HBoxContainer/Slot6,
	$HBoxContainer/Slot7,
	$HBoxContainer/Slot8,
	$HBoxContainer/Slot9
]

func _on_Inventory_slot_updated(slot, id, count):
	slots[slot].get_child(0).text = String(id)
	slots[slot].get_child(1).get_child(0).text = String(count)
