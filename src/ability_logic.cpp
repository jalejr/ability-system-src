#include "ability_logic.h"
#include "ability_definition.h"
#include "ability_system.h"

using namespace godot;

void AbilityLogic::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_ability_system"), &AbilityLogic::get_ability_system);
    ClassDB::bind_method(D_METHOD("transition_to", "state"), &AbilityLogic::transition_to);
    ClassDB::bind_method(D_METHOD("end"), &AbilityLogic::end);
    ClassDB::bind_method(D_METHOD("cancel", "reason"), &AbilityLogic::cancel);
 
    ClassDB::bind_method(D_METHOD("can_commit"), &AbilityLogic::can_commit);
    ClassDB::bind_method(D_METHOD("on_commit"), &AbilityLogic::on_commit);
    ClassDB::bind_method(D_METHOD("on_activate"), &AbilityLogic::on_activate);
    ClassDB::bind_method(D_METHOD("on_end"), &AbilityLogic::on_end);
    ClassDB::bind_method(D_METHOD("on_cancel", "reason"), &AbilityLogic::on_cancel);
    ClassDB::bind_method(D_METHOD("on_state_entered", "state"), &AbilityLogic::on_state_entered);
 
    GDVIRTUAL_BIND(_can_commit)
    GDVIRTUAL_BIND(_on_commit)
    GDVIRTUAL_BIND(_on_activate)
    GDVIRTUAL_BIND(_on_end)
    GDVIRTUAL_BIND(_on_cancel, "reason")
    GDVIRTUAL_BIND(_on_state_entered, "state")
}

bool AbilityLogic::can_commit() {
    bool result = true;
    GDVIRTUAL_CALL(_can_commit, result);
    return result;
}

void AbilityLogic::on_commit() {
    GDVIRTUAL_CALL(_on_commit);
}

void AbilityLogic::on_activate() {
    GDVIRTUAL_CALL(_on_activate);
}

void AbilityLogic::on_end() {
    GDVIRTUAL_CALL(_on_end);
}

void AbilityLogic::on_cancel(const StringName& p_reason) {
    if (!GDVIRTUAL_CALL(_on_cancel, p_reason)) on_end();
}

void AbilityLogic::on_state_entered(const StringName& p_state) {
    GDVIRTUAL_CALL(_on_state_entered, p_state);
}

void AbilityLogic::transition_to(const StringName& p_state) {
    ERR_FAIL_NULL_MSG(_ability_system,
        "AbilityLogic::transition_to: no AbilitySystem set.");

    ERR_FAIL_NULL_MSG(_definition.ptr(),
        "AbilityLogic::transition_to: no definition set.");

    uint8_t idx = _definition->get_state_index(p_state);

    ERR_FAIL_COND_MSG(idx == 255,
        vformat("AbilityLogic::transition_to: state '%s' not found.",
            String(p_state)));
 
    _state_index = idx;
    _ability_system->emit_signal(
        "ability_state_changed",
        static_cast<int>(_instance_id), 
        static_cast<int>(_state_index)
    );

    on_state_entered(p_state);
}

void AbilityLogic::end() {
    ERR_FAIL_NULL_MSG(_ability_system,
        "AbilityLogic::end: no AbilitySystem set.");

    _ability_system->_on_instance_end(_instance_id);
}

void AbilityLogic::cancel(const StringName& p_reason) {
    ERR_FAIL_NULL_MSG(_ability_system,
        "AbilityLogic::cancel: no AbilitySystem set.");

    _ability_system->_on_instance_cancel(_instance_id, p_reason);
}

void AbilityLogic::_initialize(
    AbilitySystem* p_system, 
    Ref<AbilityDefinition> p_definition, 
    uint32_t p_id
) {
    _ability_system = p_system;
    _definition = p_definition;
    _instance_id = p_id;
    _state_index = 0;
}