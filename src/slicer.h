#ifndef SLICER_H
#define SLICER_H

#ifdef WIN32
#include <windows.h>
#endif

//#include <godot-cpp/classes/node3d.hpp>
#include <godot_cpp/classes/node3d.hpp>
//#include <godot-cpp/classes/mesh_instance3d.hpp>
#include <godot_cpp/classes/mesh_instance3d.hpp>
#include "sliced_mesh.h"

using namespace godot;

/**
 * Helper for cutting a convex mesh along a plane and returning
 * two new meshes representing both sides of the cut
*/
class Slicer : public RefCounted {
    GDCLASS(Slicer, RefCounted);

protected:
    static void _bind_methods();

public:
    /**
     * Slice the passed in mesh along the passed in plane, setting the interrior cut surface to the passed in material
    */
    Ref<SlicedMesh> slice_by_plane(const Ref<ArrayMesh> mesh, const Plane plane, const Ref<Material> cross_section_material);

    /**
     * Generates a plane based on the given position and normal and perform a cut along that plane
    */
    Ref<SlicedMesh> slice_mesh(const Ref<Mesh> mesh, const Vector3 position, const Vector3 normal, const Ref<Material> cross_section_material);

    /**
     * Generates a plane based on the given position and normal and offsets it by the given Transform before applying the slice
    */
    Ref<SlicedMesh> slice(const Ref<Mesh> mesh, const Transform3D mesh_transform, const Vector3 position, const Vector3 normal, const Ref<Material> cross_section_material);
};

#endif // SLICER_H
