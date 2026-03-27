#pragma once

#include "ability_definition.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/core/gdvirtual.gen.inc"
#include <cstdint>

using namespace godot;

class AbilitySystem;

class AbilityLogic : public Node {
    GDCLASS(AbilityLogic, Node)

private:
    AbilitySystem* _ability_system = nullptr;
    Ref<AbilityDefinition> _definition;
    uint32_t _instance_id = 0;
    uint8_t _state_index = 0;

protected:
    static void _bind_methods();

public:
    AbilityLogic() = default;

    AbilitySystem* get_ability_system() const { return _ability_system; }
    Ref<AbilityDefinition> get_definition() const { return _definition; }
    int get_ability_id() const { return static_cast<int>(_instance_id); }
    int get_state_index() const { return static_cast<int>(_state_index);}

    void transition_to(const StringName& p_state);
    void end();
    void cancel(const StringName& p_reason = StringName());

    virtual bool can_commit();
    virtual void on_commit();
    virtual void on_activate();
    virtual void on_end();
    virtual void on_cancel(const StringName& p_reason);
    virtual void on_state_entered(const StringName& p_state);

    GDVIRTUAL0R(bool, _can_commit)
    GDVIRTUAL0(_on_commit)
    GDVIRTUAL0(_on_activate)
    GDVIRTUAL0(_on_end)
    GDVIRTUAL1(_on_cancel, StringName)
    GDVIRTUAL1(_on_state_entered, StringName)

private:
    friend class AbilitySystem;

    void _initialize(AbilitySystem* p_system, Ref<AbilityDefinition> p_definition, uint32_t p_id);
};
