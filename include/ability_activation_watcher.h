#pragma once

#include "ability_definition.h"
#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "tag_container.h"

using namespace godot;

class AbilitySystem;

class AbilityActivationWatcher : public RefCounted {
    GDCLASS(AbilityActivationWatcher, RefCounted)

private:
    AbilitySystem* _system = nullptr;
    TagContainer* _tags = nullptr;
    Ref<AbilityDefinition> _definition;
    bool _is_pending = false;

protected:
    static void _bind_methods() {};

public:
    AbilityActivationWatcher() = default;
    AbilityActivationWatcher(
        AbilitySystem* p_system,
        TagContainer* p_tags,
        Ref<AbilityDefinition> p_definition
    );
    ~AbilityActivationWatcher();

    void enter_pending();
    void exit_pending();
    bool is_pending() const { return _is_pending; }

    Ref<AbilityDefinition> get_definition() const { return _definition; }

private:
    void _on_tag_changed(const StringName& p_tag, int32_t p_count);

    void _connect_trigger_tags();
    void _disconnect_trigger_tags();

    void _connect_pending_tags();
    void _disconnect_pending_tags();
};