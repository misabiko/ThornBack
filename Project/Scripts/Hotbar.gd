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


func _on_Inventory_changed_selected_slot(last_slot, new_slot):
	slots[last_slot].color = Color(0, 0, 0, 0.5)
	slots[new_slot].color = Color(0.5, 0.5, 0.5, 0.5)
