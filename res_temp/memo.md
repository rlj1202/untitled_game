

```
vertex {
    vec3 pos;
    vec2 tex_coord;
    vec3 normal;
    vec3 color;
    vec4 or_something;
}

mesh(vao) {
    buffer {
        vec3 pos;
        vec2 tex_coord;

        data = [ P, T, P, T, P, T, ... ];
    }
    buffer {
        vec3 normal;
        vec3 color;
        vec4 or_something;

        data = [ N, N, N, N, ..., C, C, C, C, ..., O, O, O, O, ... ];
    }
    index_buffer {
        0, 1, 2,
        0, 2, 3,
        ...
    }
}

model {
    data = [
        { vertices, indices, texture },
        { vertices, indices, texture },
        ...
    ];

    => convert into set of meshes
}

shader {
    layout (location = 0) in vec3 a_pos;
    layout (location = 1) in vec2 a_tex_coords;
}

```