#include "ability_system.h"

#include "ability_activation_watcher.h"
#include "ability_definition.h"
#include "ability_logic.h"
#include "ability_instance_watcher.h"
#include "godot_cpp/variant/typed_array.hpp"
#include <algorithm>

void AbilitySystem::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_tag_container"), &AbilitySystem::get_tag_container);
    ClassDB::bind_method(D_METHOD("set_tag_container", "container"), &AbilitySystem::set_tag_container);

    ClassDB::bind_method(D_METHOD("get_abilities"), &AbilitySystem::get_abilities);
    ClassDB::bind_method(D_METHOD("set_abilities", "abilities"), &AbilitySystem::set_abilities);

    ClassDB::bind_method(D_METHOD("add_ability", "definition"), &AbilitySystem::add_ability);
    ClassDB::bind_method(D_METHOD("remove_ability", "definition"), &AbilitySystem::remove_ability);

    ClassDB::bind_method(D_METHOD("try_activate", "tag"), &AbilitySystem::try_activate);
    ClassDB::bind_method(D_METHOD("try_activate_by_definition","definition"), &AbilitySystem::try_activate_by_definition);

    ClassDB::bind_method(D_METHOD("end_instance", "instance_id"), &AbilitySystem::end_instance);
    ClassDB::bind_method(D_METHOD("cancel_instance", "instance_id", "reason"), &AbilitySystem::cancel_instance);

    ClassDB::bind_method(D_METHOD("is_ability_active", "tag"), &AbilitySystem::is_ability_active);

    ClassDB::bind_method(D_METHOD("get_active_instance_ids", "tag"), &AbilitySystem::get_active_instance_ids);
    ClassDB::bind_method(D_METHOD("get_instance", "instance_id"), &AbilitySystem::get_instance);

    ClassDB::bind_method(D_METHOD("_initialize"), &AbilitySystem::_initialize);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "tag_container",
        PROPERTY_HINT_NODE_TYPE, "TagContainer"),
        "set_tag_container", "get_tag_container");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "abilities",
        PROPERTY_HINT_TYPE_STRING,
        String::num(Variant::OBJECT) + "/" +
        String::num(PROPERTY_HINT_RESOURCE_TYPE) + ":AbilityDefinition"),
        "set_abilities", "get_abilities");

    ADD_SIGNAL(MethodInfo("ability_activated",
        PropertyInfo(Variant::STRING_NAME, "ability_tag"),
        PropertyInfo(Variant::INT, "instance_id")));

    ADD_SIGNAL(MethodInfo("ability_ended",
        PropertyInfo(Variant::STRING_NAME, "ability_tag"),
        PropertyInfo(Variant::INT, "instance_id")));

    ADD_SIGNAL(MethodInfo("ability_cancelled",
        PropertyInfo(Variant::STRING_NAME, "ability_tag"),
        PropertyInfo(Variant::INT, "instance_id"),
        PropertyInfo(Variant::STRING_NAME, "reason")));

    ADD_SIGNAL(MethodInfo("ability_state_changed",
        PropertyInfo(Variant::INT, "instance_id"),
        PropertyInfo(Variant::INT, "state_index")));

    ADD_SIGNAL(MethodInfo("ability_ready",
        PropertyInfo(Variant::STRING_NAME, "ability_tag")));
}

void AbilitySystem::_ready()
{
    call_deferred("_initialize");
}

void AbilitySystem::_initialize()
{
    ERR_FAIL_NULL_MSG(_tag_container,
        "AbilitySystem: tag_container not assigned.");
 
    for (int i = 0; i < _abilities.size(); i++) {
        Ref<AbilityDefinition> definition = _abilities[i];
        if (definition.is_valid()) _register_ability(definition);
    }
 
    _is_initialized = true;
}

void AbilitySystem::add_ability(const Ref<AbilityDefinition>& p_definition)
{
    ERR_FAIL_COND_MSG(p_definition.is_null(),
        "AbilitySystem::add_ability: null definition.");
        
    ERR_FAIL_COND_MSG(!p_definition->get_logic_script().is_valid(),
        vformat("AbilitySystem::add_ability: '%s' has no logic_script assigned.",
            String(p_definition->get_name_tag())));

    _abilities.push_back(p_definition);
    _register_ability(p_definition);
}

void AbilitySystem::remove_ability(const Ref<AbilityDefinition>& p_definition)
{
    ERR_FAIL_COND_MSG(p_definition.is_null(),
        "AbilitySystem::remove_ability: null definition.");
 
    StringName tag = p_definition->get_name_tag();

    auto id_it = _tag_to_ids.find(tag);
    if (id_it != _tag_to_ids.end() && !id_it->value.empty()) {
        WARN_PRINT(vformat(
            "AbilitySystem::remove_ability: '%s' removed while instances active.",
            String(tag)));
    }
 
    _tag_to_activation_watcher.erase(tag);

    auto preview_it = _tag_to_preview.find(tag);
    if (preview_it != _tag_to_preview.end()) {
        _tag_to_preview.erase(tag);
    }

    _tag_to_definition.erase(tag);
 
    for (int i = 0; i < _abilities.size(); i++) {
        if (_abilities[i] == p_definition) {
            _abilities.remove_at(i);
            break;
        }
    }   
}

uint32_t AbilitySystem::try_activate(const StringName& p_tag)
{
    auto it = _tag_to_definition.find(p_tag);
    if (it == _tag_to_definition.end()) return 0;
    
    return _try_activate(it->value, false);
}

uint32_t AbilitySystem::try_activate_by_definition(const Ref<AbilityDefinition>& p_definition)
{
    return _try_activate(p_definition, false);
}

uint32_t AbilitySystem::try_activate_by_trigger(const Ref<AbilityDefinition>& p_definition)
{
    return _try_activate(p_definition, true);
}

void AbilitySystem::end_instance(uint32_t p_instance_id)
{
    _finish_instance(p_instance_id, false, StringName());
}

void AbilitySystem::cancel_instance(uint32_t p_instance_id, const StringName& p_reason)
{
    _finish_instance(p_instance_id, true, p_reason);
}

bool AbilitySystem::is_ability_active(const StringName& p_tag)
{
    auto it = _tag_to_ids.find(p_tag);
    return it != _tag_to_ids.end() && !it->value.empty();
}

Array AbilitySystem::get_active_instance_ids(const StringName& p_tag) const
{
    Array result;
    auto it = _tag_to_ids.find(p_tag);
    if (it == _tag_to_ids.end()) return result;

    for (uint32_t id : it->value) {
        result.push_back(static_cast<int>(id));
    }

    return result;
}

AbilityLogic* AbilitySystem::get_instance(uint32_t p_instance_id) const
{
    auto it = _instance_id_to_instance.find(p_instance_id);
    return it != _instance_id_to_instance.end() ? it->second : nullptr;
}

void AbilitySystem::_register_ability(const Ref<AbilityDefinition>& p_definition)
{
    StringName tag = p_definition->get_name_tag();
    _tag_to_definition[tag] = p_definition;

    AbilityLogic* preview = _create_preview(p_definition);
    if (preview) _tag_to_preview[tag] = preview;
 
    _tag_to_activation_watcher[tag] = Ref<AbilityActivationWatcher>(
        memnew(
            AbilityActivationWatcher(
                this, 
                _tag_container, 
                p_definition
            )
        )
    );
 
    emit_signal("ability_ready", tag);
}

AbilityLogic* AbilitySystem::_create_preview(const Ref<AbilityDefinition>& p_definition)
{
    Ref<GDScript> script = p_definition->get_logic_script();
    if (!script.is_valid()) return nullptr;

    Object* obj = script->call("new");
    if (!obj) return nullptr;

    AbilityLogic* preview = Object::cast_to<AbilityLogic>(obj);
    if (!preview) { memdelete(obj); return nullptr; }

    preview->_initialize(this, p_definition, 0);
    return preview;
}

void AbilitySystem::_on_instance_end(uint32_t p_instance_id)
{
    _finish_instance(p_instance_id, false, StringName());
}

void AbilitySystem::_on_instance_cancel(uint32_t p_instance_id, const StringName& p_reason)
{
    _finish_instance(p_instance_id, true, p_reason);
}

void AbilitySystem::_finish_instance(uint32_t p_instance_id, bool p_canceled, const StringName& p_reason)
{
    auto it = _instance_id_to_instance.find(p_instance_id);
    ERR_FAIL_COND_MSG(it == _instance_id_to_instance.end(),
        "AbilitySystem: unknown instance_id in finish_instance.");
    
    AbilityLogic* logic = it->second;
    Ref<AbilityDefinition> definition = logic->get_definition();
    StringName tag = definition->get_name_tag();

    _remove_granted_tags(definition);

    _instance_id_to_instance_watcher.erase(p_instance_id);

    if (p_canceled) {
        logic->on_cancel(p_reason);
    } else {
        logic->on_end();
    }

    auto& ids = _tag_to_ids[tag];
    ids.erase(std::remove(ids.begin(), ids.end(), p_instance_id), ids.end());
    if (ids.empty()) _tag_to_ids.erase(tag);

    _instance_id_to_instance.erase(p_instance_id);

    logic->queue_free();

    if (p_canceled) {
        emit_signal("ability_canceled", tag, static_cast<int>(p_instance_id), p_reason);
    } else {
        emit_signal("ability_ended", tag, static_cast<int>(p_instance_id));
    }
}

uint32_t AbilitySystem::_try_activate(const Ref<AbilityDefinition>& p_definition, bool p_enter_pending)
{
    ERR_FAIL_COND_V_MSG(!_is_initialized, 0,
        "AbilitySystem: try_activate called before _ready().");

    ERR_FAIL_COND_V_MSG(p_definition.is_null(), 0,
        "AbilitySystem: null definition.");

    ERR_FAIL_COND_V_MSG(!p_definition->get_logic_script().is_valid(), 0,
        vformat("AbilitySystem: '%s' has no logic_script assigned.",
            String(p_definition->get_name_tag())));
    
    AbilityActivationWatcher* watcher = nullptr;
    auto watcher_it = _tag_to_activation_watcher.find(p_definition->get_name_tag());
    if (watcher_it != _tag_to_activation_watcher.end()) watcher = watcher_it->value.ptr();

    if (!_passes_activation_checks(p_definition)) {
        if (p_enter_pending && watcher) watcher->enter_pending();
        return 0;
    }

    if (watcher) watcher->exit_pending();

    AbilityLogic* preview = nullptr;
    auto preview_it = _tag_to_preview.find(p_definition->get_name_tag());
    if (preview_it != _tag_to_preview.end()) preview = preview_it->value;

    if (preview && !preview->can_commit()) return 0;
 
    Ref<GDScript> script = p_definition->get_logic_script();
    Object* obj = script->call("new");
    ERR_FAIL_COND_V_MSG(!obj, 0,
        vformat("AbilitySystem: failed to instantiate logic_script for '%s'.",
            String(p_definition->get_name_tag())));
 
    AbilityLogic* logic = Object::cast_to<AbilityLogic>(obj);
    ERR_FAIL_COND_V_MSG(!logic, 0,
        vformat("AbilitySystem: logic_script for '%s' does not extend AbilityLogic.",
            String(p_definition->get_name_tag())));

    uint32_t id = _next_id++;

    logic->_initialize(this, p_definition, id);

    logic->on_commit();

    _add_granted_tags(p_definition);

    _instance_id_to_instance_watcher[id] = Ref<AbilityInstanceWatcher>(
        memnew(
            AbilityInstanceWatcher(
                this, 
                _tag_container, 
                p_definition, 
                id
            )
        )
    );
    
    logic->set_name(String(p_definition->get_name_tag()));
    add_child(logic);
 
    _instance_id_to_instance[id] = logic;
    _tag_to_ids[p_definition->get_name_tag()].push_back(id);

    if (p_definition->get_states().size() > 0) {
        logic->_state_index = 0;
        StringName initial = StringName(p_definition->get_states()[0]);
        logic->on_state_entered(initial);
        emit_signal("ability_state_changed", static_cast<int>(id), 0);
    }
 
    logic->on_activate();

    emit_signal("ability_activated",
        p_definition->get_name_tag(), static_cast<int>(id));
 
    return id;
}

void AbilitySystem::_add_granted_tags(const Ref<AbilityDefinition>& p_definition)
{
    if (!_tag_container) return;
    _tag_container->add_tags(p_definition->get_granted_tags());
}

void AbilitySystem::_remove_granted_tags(const Ref<AbilityDefinition>& p_definition)
{
    if (!_tag_container) return;
    _tag_container->remove_tags(p_definition->get_granted_tags());
}

bool AbilitySystem::_passes_activation_checks(const Ref<AbilityDefinition>& p_definition) const
{
    if (!_tag_container) return true;

    if (!_tag_container->has_all_tags(p_definition->get_required_tags())) return false;

    if (_tag_container->has_any_tags(p_definition->get_blocked_tags())) return false;

    return true;
}
