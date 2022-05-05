# Godot-Slicer

A port of [Ezy-Slicer](https://github.com/DavidArayan/ezy-slice) for the [Godot game engine](https://godotengine.org/). Updated for Godot 4.0 ([original by CJ DiMaggio](https://github.com/cj-dimaggio/godot-slicer)).

## Important: Slicer is currently broken because of engine bug [godot-cpp#751](https://github.com/godotengine/godot-cpp/issues/751).

## About
Built as a Godot extension in C++, Slicer is a port of [David Arayan's Ezy-Slicer](https://github.com/DavidArayan/ezy-slice) Unity plugin (who deserves all credit). It allows for the dynamic slicing of convex meshes along a plane. Built against Godot version 4.0 alpha.

## Installing
The new extension system doesn't have official documentation yet, but [this blog post](https://godotengine.org/article/introducing-gd-extensions) is about as close as it gets. Slicer can be built by cloning the repo next to [the compiled C++ bindings](https://github.com/godotengine/godot-cpp):

```bash
git clone https://github.com/pooroligarch/godot-slicer.git slicer/
cd slicer
scons
```

When this is done, copy the dynamic library (`libgdslicer.so`/`.dll`) and the extension manifest (`slicer.gdextension`) to your Godot project. You can modify the manifest if you want to move the library (default path is `bin/win64/` on Windows, `bin/x11/` on Linux).

## Using
After installing the extension a `Slicer` object will now be available. A `Slicer` instance can then be used to trigger slices of `Mesh` geometry like so:

```gdscript
extends RigidDynamicBody3D

class_name Sliceable

@export var cross_section_material : Material
@export var mesh_override : Mesh

func _ready():
	if mesh_override:
		$MeshInstance.mesh = mesh_override

	# Setup the collision shape to be the mesh's shape
	var shape = ConvexPolygonShape3D.new()
	shape.points = $MeshInstance.mesh.surface_get_arrays(0)[Mesh.ARRAY_VERTEX]
	shape.margin = 0.015
	var owner_id = self.create_shape_owner(self)
	self.shape_owner_add_shape(owner_id, shape)

func cut(plane_origin: Vector3, plane_normal: Vector3):
	var slicer = Slicer.new()
	var sliced: SlicedMesh = slicer.slice($MeshInstance.mesh, self.transform, plane_origin, plane_normal, cross_section_material)

    if not sliced:
        print("No slice occurred")
    
    if sliced.upper_mesh:
        print("Instantiate the upper cut mesh somewhere")

    if sliced.lower_mesh:
        print("Instantiate the lower cut mesh somewhere")
```
