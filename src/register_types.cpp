#include "register_types.h"

#include <godot/gdnative_interface.h>

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void register_slicer_types() {
    ClassDB::register_class<Slicer>();
    ClassDB::register_class<SlicedMesh>();
}

void unregister_slicer_types() {
}

extern "C" {

// Initialization.

GDNativeBool GDN_EXPORT slicer_library_init(const GDNativeInterface *p_interface, const GDNativeExtensionClassLibraryPtr p_library, GDNativeInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_interface, p_library, r_initialization);

	init_obj.register_scene_initializer(register_slicer_types);
	init_obj.register_scene_terminator(unregister_slicer_types);

	return init_obj.init();
}
}
