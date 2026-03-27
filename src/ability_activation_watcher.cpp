#include "ability_activation_watcher.h"
#include "ability_definition.h"
#include "ability_system.h"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/string_name.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "tag_container.h"

using namespace godot;

AbilityActivationWatcher::AbilityActivationWatcher(
    AbilitySystem* p_system,
    TagContainer* p_tags,
    Ref<AbilityDefinition> p_definition
) : _system(p_system),
    _tags(p_tags),
    _definition(p_definition)
{
    _connect_trigger_tags();
}

AbilityActivationWatcher::~AbilityActivationWatcher() {
    _disconnect_trigger_tags();

    if (_is_pending) _disconnect_pending_tags();
}

void AbilityActivationWatcher::enter_pending() {
    if (_is_pending) return;

    _is_pending = true;
    _connect_pending_tags();
}

void AbilityActivationWatcher::exit_pending() {
    if (!_is_pending) return;
    _is_pending = false;
    _disconnect_pending_tags();
}

void AbilityActivationWatcher::_on_tag_changed(const StringName& p_tag, int32_t p_count) {
    _system->try_activate_by_trigger(_definition);
}

void AbilityActivationWatcher::_connect_trigger_tags() {
    if (!_tags) return;

    Callable callable = callable_mp(this, &AbilityActivationWatcher::_on_tag_changed);
    const TypedArray<StringName>& trigger_tags = _definition->get_trigger_tags();
    for (int i = 0; i < trigger_tags.size(); i++) {
        _tags->watch_tag(StringName(trigger_tags[i]), callable);
    }
}

void AbilityActivationWatcher::_disconnect_trigger_tags() {
    if (!_tags) return;

    Callable cb = callable_mp(this, &AbilityActivationWatcher::_on_tag_changed);
    const TypedArray<StringName>& trigger_tags = _definition->get_trigger_tags();
    for (int i = 0; i < trigger_tags.size(); i++)
        _tags->unwatch_tag(StringName(trigger_tags[i]), cb);
}

void AbilityActivationWatcher::_connect_pending_tags() {
    if (!_tags) return;

    Callable callable = callable_mp(this, &AbilityActivationWatcher::_on_tag_changed);

    const TypedArray<StringName>& required_tags = _definition->get_required_tags();
    for (int i = 0; i < required_tags.size(); i++) {
        _tags->watch_tag(StringName(required_tags[i]), callable);
    }

    const TypedArray<StringName>& blocked_tags = _definition->get_blocked_tags();
    for (int i = 0; i < blocked_tags.size(); i++) {
        _tags->watch_tag(StringName(blocked_tags[i]), callable);
    }
}

void AbilityActivationWatcher::_disconnect_pending_tags() {
    if (!_tags) return;

    Callable callable = callable_mp(this, &AbilityActivationWatcher::_on_tag_changed);

    const TypedArray<StringName>& required_tags = _definition->get_required_tags();
    for (int i = 0; i < required_tags.size(); i++) {
        _tags->unwatch_tag(StringName(required_tags[i]), callable);
    }

    const TypedArray<StringName>& blocked_tags = _definition->get_blocked_tags();
    for (int i = 0; i < blocked_tags.size(); i++) {
        _tags->unwatch_tag(StringName(blocked_tags[i]), callable);
    }
}