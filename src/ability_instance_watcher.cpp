#include "ability_instance_watcher.h"
#include "ability_definition.h"
#include "ability_system.h"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/callable_method_pointer.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "tag_container.h"

using namespace godot;

AbilityInstanceWatcher::AbilityInstanceWatcher(
    AbilitySystem* p_system,
    TagContainer* p_tags,
    Ref<AbilityDefinition> p_definition,
    uint32_t p_instance_id
) : _system(p_system),
    _tags(p_tags),
    _definition(p_definition),
    _instance_id(p_instance_id)
{
    if (!_tags) return;

    Callable callable = callable_mp(this, &AbilityInstanceWatcher::_on_canceled_tag_changed);
    const TypedArray<StringName>& canceled_tags = _definition->get_canceled_tags();
    for (int i = 0; i < canceled_tags.size(); i++) {
        _tags->watch_tag(StringName(canceled_tags[i]), callable);
    }
}

AbilityInstanceWatcher::~AbilityInstanceWatcher() {
    if (!_tags) return;

    Callable callable = callable_mp(this, &AbilityInstanceWatcher::_on_canceled_tag_changed);
    const TypedArray<StringName>& canceled_tags = _definition->get_canceled_tags();
    for (int i = 0; i < canceled_tags.size(); i++) {
        _tags->unwatch_tag(StringName(canceled_tags[i]), callable);
    }
}

void AbilityInstanceWatcher::_on_canceled_tag_changed(const StringName& p_tag, int32_t p_count) {
    if (p_count > 0) {
        _system->cancel_instance(_instance_id, p_tag);
    }
}