#pragma once

#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "ability_definition.h"
#include "ability_activation_watcher.h"
#include "ability_instance_watcher.h"
#include "identity_hash.h"
#include "tag_container.h"
#include <cstdint>
#include <unordered_map>

using namespace godot;

class AbilityLogic;

class AbilitySystem : public Node {
    GDCLASS(AbilitySystem, Node)

private:
    TagContainer* _tag_container = nullptr;
    TypedArray<AbilityDefinition> _abilities;
    uint32_t _next_id = 1;
    bool _is_initialized = false;
    
    HashMap<StringName, Ref<AbilityDefinition>> _tag_to_definition;
    HashMap<StringName, AbilityLogic*> _tag_to_preview;
    HashMap<StringName, std::vector<uint32_t>> _tag_to_ids;
    HashMap<StringName, Ref<AbilityActivationWatcher>> _tag_to_activation_watcher;
    std::unordered_map<uint32_t, AbilityLogic*, IdentityHash> _instance_id_to_instance;
    std::unordered_map<uint32_t, Ref<AbilityInstanceWatcher>, IdentityHash>  _instance_id_to_instance_watcher;

    friend class AbilityLogic;
    
protected:
    static void _bind_methods();

public:
    AbilitySystem() = default;

    void _ready() override;
    void _initialize();

    TagContainer* get_tag_container() const { return _tag_container; }
    void set_tag_container(TagContainer* p_container) { _tag_container = p_container; }

    TypedArray<AbilityDefinition> get_abilities() const { return _abilities; }
    void set_abilities(const TypedArray<AbilityDefinition>& p_abilities) { _abilities = p_abilities; };

    void add_ability(const Ref<AbilityDefinition>& p_definition);
    void remove_ability(const Ref<AbilityDefinition>& p_definition);

    uint32_t try_activate(const StringName& p_tag);
    uint32_t try_activate_by_definition(const Ref<AbilityDefinition>& p_definition);
    uint32_t try_activate_by_trigger(const Ref<AbilityDefinition>& p_definition);

    void end_instance(uint32_t p_instance_id);
    void cancel_instance(uint32_t p_instance_id, const StringName& p_reason = StringName());

    bool is_ability_active(const StringName& p_tag);
    Array get_active_instance_ids(const StringName& p_tag) const;
    AbilityLogic* get_instance(uint32_t p_instance_id) const;

private:
    void _register_ability(const Ref<AbilityDefinition>& p_definition);
    AbilityLogic* _create_preview(const Ref<AbilityDefinition>& p_definition);
    void _on_instance_end(uint32_t p_instance_id);
    void _on_instance_cancel(uint32_t p_instance_id, const StringName& p_reason);
    void _finish_instance(uint32_t p_instance_id, bool p_canceled, const StringName& p_reason);
    uint32_t _try_activate(const Ref<AbilityDefinition>& p_definition, bool p_enter_pending);

    void _add_granted_tags(const Ref<AbilityDefinition>& p_definition);
    void _remove_granted_tags(const Ref<AbilityDefinition>& p_definition);
    
    bool _passes_activation_checks(const Ref<AbilityDefinition>& p_definition) const;
};