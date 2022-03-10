#ifndef FACE_FILLER_H
#define FACE_FILLER_H

#include "slicer_face.h"

// This just mimics logic found in TriangleMesh#Create
_FORCE_INLINE_ Vector3 snap_vertex(Vector3 v) {
    return v.snapped(Vector3(0.0001, 0.0001, 0.0001));
}

/**
 * Responsible for serializing data from vertex arrays, as they are
 * given from the visual server, into an array of SlicerFaces while
 * maintaining info about things such as normals and uvs etc.
*/
struct FaceFiller {
    SlicerFace *faces_writer;
    const Vector3 *vertices_reader;

    bool has_normals;
    const Vector3 *normals_reader;

    bool has_tangents;
    const real_t *tangents_reader;

    bool has_colors;
    const Color *colors_reader;

    bool has_bones;
    const real_t *bones_reader;

    bool has_weights;
    const real_t *weights_reader;

    bool has_uvs;
    const Vector2 *uvs_reader;

    bool has_uv2s;
    const Vector2 *uv2s_reader;

    // Yuck. What an eye sore this constructor is
    FaceFiller(Vector<SlicerFace> &faces, const Array &surface_arrays) {
        faces_writer = faces.ptrw();

        PackedVector3Array vertices = surface_arrays[Mesh::ARRAY_VERTEX];
        vertices_reader = vertices.ptr();

        PackedVector3Array normals = surface_arrays[Mesh::ARRAY_NORMAL];
        normals_reader = normals.ptr();
        has_normals = normals.size() > 0 && normals.size() == vertices.size();

        #ifdef REAL_T_IS_DOUBLE
        PackedFloat64Array tangents;
        #else
        PackedFloat32Array tangents;
        #endif
        tangents = surface_arrays[Mesh::ARRAY_TANGENT];
        tangents_reader = tangents.ptr();
        has_tangents = tangents.size() > 0 && tangents.size() == vertices.size() * 4;
        
        if (!surface_arrays[Mesh::ARRAY_COLOR]) {
        has_colors = false;
        } else {
        PackedColorArray colors = surface_arrays[Mesh::ARRAY_COLOR];
        colors_reader = colors.ptr();
        has_colors = colors.size() > 0 && colors.size() == vertices.size();
        }

        if (!surface_arrays[Mesh::ARRAY_BONES]) {
        has_bones = false;
        } else {
        #ifdef REAL_T_IS_DOUBLE
        PackedFloat64Array bones;
        #else
        PackedFloat32Array bones;
        #endif
        bones = surface_arrays[Mesh::ARRAY_BONES];
        bones_reader = bones.ptr();
        has_bones = bones.size() > 0 && bones.size() == vertices.size() * 4;
        }

        if (!surface_arrays[Mesh::ARRAY_WEIGHTS]) {
        has_weights = false;
        } else {
        #ifdef REAL_T_IS_DOUBLE
        PackedFloat64Array weights;
        #else
        PackedFloat32Array weights;
        #endif
        weights = surface_arrays[Mesh::ARRAY_WEIGHTS];
        weights_reader = weights.ptr();
        has_weights = weights.size() > 0 && weights.size() == vertices.size() * 4;
        }

        PackedVector2Array uvs = surface_arrays[Mesh::ARRAY_TEX_UV];
        uvs_reader = uvs.ptr();
        has_uvs = uvs.size() > 0 && uvs.size() == vertices.size();

        if (!surface_arrays[Mesh::ARRAY_TEX_UV2]) {
        has_uv2s = false;
        } else {
        PackedVector2Array uv2s = surface_arrays[Mesh::ARRAY_TEX_UV2];
        uv2s_reader = uv2s.ptr();
        has_uv2s = uv2s.size() > 0 && uv2s.size() == vertices.size();
        }
    }

    /**
     * Takes data from the vertex array using the lookup_idx and puts it into
     * our face vector using set_idx
    */ 
    _FORCE_INLINE_ void fill(int set_idx, int lookup_idx) {
        // Having this function work vertex by vertex makes the code a bit nicer,
        // especially with having to support indexed and non-indexed vertexes,
        // but just performance-wise I hate it. There's no reason, besides uglier
        // and more complicated code, why we can't be doing these has_* checks on
        // a per face basis. Maybe even a per mesh basis. Admitidly it'll probably
        // all come out in the wash, but it bothers me conceptually. Let's put in
        // a TODO about it. Maybe there's something incredibly clever we can do with
        // macros that *won't* make me want to tear out what's left of my hair.
        int face_idx = set_idx / 3;
        int set_offset = set_idx % 3;

        if (set_offset == 0) {
            faces_writer[face_idx].has_normals = has_normals;
            faces_writer[face_idx].has_tangents = has_tangents;
            faces_writer[face_idx].has_colors = has_colors;
            faces_writer[face_idx].has_bones = has_bones;
            faces_writer[face_idx].has_weights = has_weights;
            faces_writer[face_idx].has_uvs = has_uvs;
            faces_writer[face_idx].has_uv2s = has_uv2s;
        }

        faces_writer[face_idx].vertex[set_offset] = snap_vertex(vertices_reader[lookup_idx]);

        if (has_normals) {
            faces_writer[face_idx].normal[set_offset] = normals_reader[lookup_idx];
        }

        if (has_tangents) {
            faces_writer[face_idx].tangent[set_offset] = SlicerVector4(
                tangents_reader[lookup_idx * 4],
                tangents_reader[lookup_idx * 4 + 1],
                tangents_reader[lookup_idx * 4 + 2],
                tangents_reader[lookup_idx * 4 + 3]
            );
        }

        if (has_colors) {
            faces_writer[face_idx].color[set_offset] = colors_reader[lookup_idx];
        }

        if (has_bones) {
            faces_writer[face_idx].bones[set_offset] = SlicerVector4(
                bones_reader[lookup_idx * 4],
                bones_reader[lookup_idx * 4 + 1],
                bones_reader[lookup_idx * 4 + 2],
                bones_reader[lookup_idx * 4 + 3]
            );
        }

        if (has_weights) {
            faces_writer[face_idx].weights[set_offset] = SlicerVector4(
                weights_reader[lookup_idx],
                weights_reader[lookup_idx * 4 + 1],
                weights_reader[lookup_idx * 4 + 2],
                weights_reader[lookup_idx * 4 + 3]
            );
        }

        if (has_uvs) {
            faces_writer[face_idx].uv[set_offset] = uvs_reader[lookup_idx];
        }

        if (has_uv2s) {
            faces_writer[face_idx].uv2[set_offset] = uv2s_reader[lookup_idx];
        }
    }
};

#endif // FACE_FILLER_H
