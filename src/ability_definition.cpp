#include "ability_definition.h"

using namespace godot;

void AbilityDefinition::_bind_methods() {
    ClassDB::bind_method(D_METHOD("get_name_tag"), &AbilityDefinition::get_name_tag);
    ClassDB::bind_method(D_METHOD("set_name_tag", "tag"), &AbilityDefinition::set_name_tag);

    ClassDB::bind_method(D_METHOD("get_level"), &AbilityDefinition::get_level);
    ClassDB::bind_method(D_METHOD("set_level", "level"), &AbilityDefinition::set_level);

    ClassDB::bind_method(D_METHOD("get_logic_script"), &AbilityDefinition::get_logic_script);
    ClassDB::bind_method(D_METHOD("set_logic_script", "script"), &AbilityDefinition::set_logic_script);

    ClassDB::bind_method(D_METHOD("get_granted_tags"),  &AbilityDefinition::get_granted_tags);
    ClassDB::bind_method(D_METHOD("set_granted_tags",  "tags"), &AbilityDefinition::set_granted_tags);

    ClassDB::bind_method(D_METHOD("get_required_tags"), &AbilityDefinition::get_required_tags);
    ClassDB::bind_method(D_METHOD("set_required_tags", "tags"), &AbilityDefinition::set_required_tags);
    
    ClassDB::bind_method(D_METHOD("get_blocked_tags"),  &AbilityDefinition::get_blocked_tags);
    ClassDB::bind_method(D_METHOD("set_blocked_tags",  "tags"), &AbilityDefinition::set_blocked_tags);

    ClassDB::bind_method(D_METHOD("get_canceled_tags"), &AbilityDefinition::get_canceled_tags);
    ClassDB::bind_method(D_METHOD("set_canceled_tags", "tags"), &AbilityDefinition::set_canceled_tags);

    ClassDB::bind_method(D_METHOD("get_trigger_tags"),  &AbilityDefinition::get_trigger_tags);
    ClassDB::bind_method(D_METHOD("set_trigger_tags",  "tags"), &AbilityDefinition::set_trigger_tags);

    ClassDB::bind_method(D_METHOD("get_states"), &AbilityDefinition::get_states);
    ClassDB::bind_method(D_METHOD("set_states", "states"), &AbilityDefinition::set_states);

    ClassDB::bind_method(D_METHOD("get_state_index", "state"), &AbilityDefinition::get_state_index);

    ADD_PROPERTY(PropertyInfo(Variant::STRING_NAME, "ability_tag",
        PROPERTY_HINT_PLACEHOLDER_TEXT, "tag"),
        "set_name_tag", "get_name_tag");

    ADD_PROPERTY(PropertyInfo(Variant::INT, "level"),
        "set_level", "get_level");
    
    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "logic_script",
        PROPERTY_HINT_RESOURCE_TYPE, "GDScript"),
        "set_logic_script", "get_logic_script");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "granted_tags",
        PROPERTY_HINT_TYPE_STRING, "StringName/0:tag"),
        "set_granted_tags", "get_granted_tags");    
    
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "required_tags",
        PROPERTY_HINT_TYPE_STRING, "StringName/0:tag"),
        "set_required_tags", "get_required_tags");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "blocked_tags",
        PROPERTY_HINT_TYPE_STRING, "StringName/0:tag"),
        "set_blocked_tags", "get_blocked_tags");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "canceled_tags",
        PROPERTY_HINT_TYPE_STRING, "StringName/0:tag"),
        "set_canceled_tags", "get_canceled_tags");

    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "trigger_tags",
        PROPERTY_HINT_ARRAY_TYPE, "StringName/0:tag"),
        "set_trigger_tags", "get_trigger_tags");
    
    ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "states",
        PROPERTY_HINT_ARRAY_TYPE, "StringName"),
        "set_states", "get_states");
}