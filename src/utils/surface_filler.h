#ifndef SURFACE_FILLER_H
#define SURFACE_FILLER_H

#include "slicer_face.h"

#include <godot_cpp/classes/array_mesh.hpp>

/**
 * The inverse of FaceFiller, this struct is responsible for taking
 * SlicerFaces and serializing them back into vertex arrays for Godot
 * to read into a mesh surface
*/
struct SurfaceFiller {
    bool has_normals;
    bool has_tangents;
    bool has_colors;
    bool has_bones;
    bool has_weights;
    bool has_uvs;
    bool has_uv2s;

    const SlicerFace *faces_reader;
    Array arrays;

    PackedVector3Array vertices;
    Vector3 *vertices_writer;

    PackedVector3Array normals;
    Vector3 *normals_writer;

    #ifdef REAL_T_IS_DOUBLE // this is why everybody loves C++
    PackedFloat64Array tangents;
    #else
    PackedFloat32Array tangents;
    #endif
    real_t *tangents_writer;

    PackedColorArray colors;
    Color *colors_writer;

    #ifdef REAL_T_IS_DOUBLE
    PackedFloat64Array bones;
    #else
    PackedFloat32Array bones;
    #endif
    real_t *bones_writer;

    #ifdef REAL_T_IS_DOUBLE
    PackedFloat64Array weights;
    #else
    PackedFloat32Array weights;
    #endif
    real_t *weights_writer;

    PackedVector2Array uvs;
    Vector2 *uvs_writer;

    PackedVector2Array uv2s;
    Vector2 *uv2s_writer;

    SurfaceFiller(const Vector<SlicerFace> &faces) {
        SlicerFace first_face = faces[0];

        has_normals = first_face.has_normals;
        has_tangents = first_face.has_tangents;
        has_colors = first_face.has_colors;
        has_bones = first_face.has_bones;
        has_weights = first_face.has_weights;
        has_uvs = first_face.has_uvs;
        has_uv2s = first_face.has_uv2s;

        faces_reader = faces.ptr();

        arrays.resize(Mesh::ARRAY_MAX);

        int array_length = faces.size() * 3;
        vertices.resize(array_length);
        vertices_writer = vertices.ptrw();

        // There's gotta be a less tedious way of doing this
        if (has_normals) {
            normals.resize(array_length);
            normals_writer = normals.ptrw();
        }

        if (has_tangents) {
            tangents.resize(array_length * 4);
            tangents_writer = tangents.ptrw();
        }

        if (has_colors) {
            colors.resize(array_length);
            colors_writer = colors.ptrw();
        }

        if (has_bones) {
            bones.resize(array_length * 4);
            bones_writer = bones.ptrw();
        }

        if (has_weights) {
            weights.resize(array_length * 4);
            weights_writer = weights.ptrw();
        }

        if (has_uvs) {
            uvs.resize(array_length);
            uvs_writer = uvs.ptrw();
        }

        if (has_uv2s) {
            uv2s.resize(array_length);
            uv2s_writer = uv2s.ptrw();
        }
    }

    /**
     * Takes data from the faces using the lookup_idx and stores it
     * to be saved into vertex arrays (see add_to_mesh for how to attach
     * that information into a mesh)
    */
    _FORCE_INLINE_ void fill(int lookup_idx, int set_idx) {
        // TODO - I think the function definition here with lookup_idx and set_idx
        // is reversed from FaceFiller#fill. We should make that more consistant
        //
        // As mentioned in the FaceFiller comments, while having this function work
        // on a vertex by vertex basis helps with cleaner code (especially, in this case,
        // when it comes to reversing the order of cross section verts), its conceptually
        // and perhaps performancely drawnback back by having to do these repeated calculations
        // and boolean checks (I'd hope the force_inline would help with the function invocation
        // cost but even then who knows).
        int face_idx = lookup_idx / 3;
        int idx_offset = lookup_idx % 3;

        SlicerFace face = faces_reader[face_idx];

        vertices_writer[set_idx] = face.vertex[idx_offset];

        if (has_normals) {
            normals_writer[set_idx] = face.normal[idx_offset];
        }

        if (has_tangents) {
            tangents_writer[set_idx * 4] = face.tangent[idx_offset][0];
            tangents_writer[set_idx * 4 + 1] = face.tangent[idx_offset][1];
            tangents_writer[set_idx * 4 + 2] = face.tangent[idx_offset][2];
            tangents_writer[set_idx * 4 + 3] = face.tangent[idx_offset][3];
        }

        if (has_colors) {
            colors_writer[set_idx] = face.color[idx_offset];
        }

        if (has_bones) {
            bones_writer[set_idx * 4] = face.bones[idx_offset][0];
            bones_writer[set_idx * 4 + 1] = face.bones[idx_offset][1];
            bones_writer[set_idx * 4 + 2] = face.bones[idx_offset][2];
            bones_writer[set_idx * 4 + 3] = face.bones[idx_offset][3];
        }

        if (has_weights) {
            weights_writer[set_idx * 4] = face.weights[idx_offset][0];
            weights_writer[set_idx * 4 + 1] = face.weights[idx_offset][1];
            weights_writer[set_idx * 4 + 2] = face.weights[idx_offset][2];
            weights_writer[set_idx * 4 + 3] = face.weights[idx_offset][3];
        }

        if (has_uvs) {
            uvs_writer[set_idx] = face.uv[idx_offset];
        }

        if (has_uv2s) {
            uv2s_writer[set_idx] = face.uv2[idx_offset];
        }
    }

    /**
     * Adds the vertex information read from the "fill" as a new surface
     * of the passed in mesh and sets the passed in material to the new
     * surface
    */
    void add_to_mesh(ArrayMesh &mesh, Ref<Material> material) {
        arrays[Mesh::ARRAY_VERTEX] = vertices;

        if (has_normals)
            arrays[Mesh::ARRAY_NORMAL] = normals;
        
        if (has_tangents)
            arrays[Mesh::ARRAY_TANGENT] = tangents;

        if (has_colors)
            arrays[Mesh::ARRAY_COLOR] = colors;

        if (has_bones)
            arrays[Mesh::ARRAY_BONES] = bones;
        
        if (has_weights)
            arrays[Mesh::ARRAY_WEIGHTS] = weights;
        
        if (has_uvs)
            arrays[Mesh::ARRAY_TEX_UV] = uvs;

        if (has_uv2s)
            arrays[Mesh::ARRAY_TEX_UV2] = uv2s;

        mesh.add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, arrays);
        mesh.surface_set_material(mesh.get_surface_count() - 1, material);
    }
};

#endif // SURFACE_FILLER_H
