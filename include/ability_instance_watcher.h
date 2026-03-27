#pragma once


#include "ability_definition.h"
#include "tag_container.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/string_name.hpp"

using namespace godot;

class AbilitySystem;

class AbilityInstanceWatcher : public RefCounted {
    GDCLASS(AbilityInstanceWatcher, RefCounted)

private:
    AbilitySystem* _system = nullptr;
    TagContainer* _tags = nullptr;
    Ref<AbilityDefinition> _definition;
    uint32_t _instance_id = 0;

protected:
    static void _bind_methods() {};

public:
    AbilityInstanceWatcher() = default;
    AbilityInstanceWatcher(
        AbilitySystem* p_system, 
        TagContainer* p_tags, 
        Ref<AbilityDefinition> p_definition, 
        uint32_t p_instance_id
    );
    ~AbilityInstanceWatcher();

    uint32_t get_ability_instance_id() const { return _instance_id; }

private:
    void _on_canceled_tag_changed(const StringName& p_tag, int32_t p_count); 
};