#pragma once

#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/gd_script.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/templates/hash_map.hpp"

using namespace godot;

class AbilityDefinition : public Resource {
    GDCLASS(AbilityDefinition, Resource)

private:
    StringName _name_tag;
    uint32_t _level = 1;
    Ref<GDScript> _logic_script;

    TypedArray<StringName> _granted_tags;
    TypedArray<StringName> _required_tags;
    TypedArray<StringName> _blocked_tags;
    TypedArray<StringName> _canceled_tags;
    TypedArray<StringName> _trigger_tags;

    TypedArray<StringName> _states;
    HashMap<StringName, uint8_t> _state_to_states_index;

protected:
    static void _bind_methods();

public:
    AbilityDefinition() = default;

    StringName get_name_tag() const { return _name_tag; }
    void set_name_tag(const StringName& p_tag) { _name_tag = p_tag; }

    uint32_t get_level() const { return _level; }
    void set_level(uint32_t p_level) { _level = p_level; }

    Ref<GDScript> get_logic_script() const { return _logic_script; }
    void set_logic_script(const Ref<GDScript>& p_script) { _logic_script = p_script; }

    TypedArray<StringName> get_trigger_tags() const { return _trigger_tags; }
    void set_trigger_tags(const TypedArray<StringName>& p_tags) { _trigger_tags = p_tags; }

    TypedArray<StringName> get_required_tags() const { return _required_tags; }
    void set_required_tags(const TypedArray<StringName>& p_tags) { _required_tags = p_tags; }

    TypedArray<StringName> get_blocked_tags() const { return _blocked_tags; }
    void set_blocked_tags(const TypedArray<StringName>& p_tags) { _blocked_tags = p_tags; }

    TypedArray<StringName> get_canceled_tags() const { return _canceled_tags; }
    void set_canceled_tags(const TypedArray<StringName>& p_tags) { _canceled_tags = p_tags; }

    TypedArray<StringName> get_granted_tags() const { return _granted_tags; }
    void set_granted_tags(const TypedArray<StringName>& p_tags) { _granted_tags = p_tags; }

    TypedArray<StringName> get_states() const { return _states; }
    void set_states(const TypedArray<StringName>& p_states) {
        _states = p_states;
        _state_to_states_index.clear();
        for (int i = 0; i < p_states.size(); i++)
            _state_to_states_index[StringName(p_states[i])] = static_cast<uint8_t>(i);
    }

    uint8_t get_state_index(const StringName& p_state) const {
        auto it = _state_to_states_index.find(p_state);
        return it != _state_to_states_index.end() ? it->value : 255;
    }
};