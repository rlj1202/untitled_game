

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


```

/assets
  /textures
    /blocks
      /
    /minecraft_atlas.png
    /minecraft_atlas.xml
  /audios
    /some_sound.mp3

textures
  minecraft_atlas
  grass (refer: minecraft_atlas, pos: 0, 0, size: 16, 16)
  cobble_stone (refer: minecraft_atlas, ...)
  wood_top (refer: minecraft_atlas, ...)
  wood_side (refer: minecraft_atlas, ...)
  ...

audios
  some_audio.mp3
  some_audio2.mp3
  ...

/res/textures/atlas.texatlas.xml
/res/textures/atlas.tex.png
/res/audios/some_audio.audio.mp3
/res/models/some_model.mesh.obj
/res/something/some_file.some_type.tar.gz

/path/to/asset/name.type.internaltype

```




```
/res
  /textures
    /some_texture.png
    /some_atlas
      /texture.png
      /asset.xml
  /audios
    /some_audio.mp3
  /models
    /some_model.obj
  /shaders
    /terrain_shader
      /asset.xml
      /vertex_shader.glsl
      /fragment_shader.glsl
```

in asset.xml file:

```xml
<?xml version="1.0" encoding="UTF-8" ?>
<Asset>
  <Type>texture_atlas</Type>
  
  <Path>./texture.png</Path>
  <SubTextures>
    <SubTexture>
      <Name>grass</Name>
      <Pos>0 0</Pos>
      <Size>16 16</Size>
    </SubTexture>
    ...
  </SubTextures>
</Asset>
```


```xml
<?xml version="1.0" encoding="UTF-8" ?>
<Asset>
  <Type>shader_program</Type>
  
  <Shader>
    <Path>./vert.glsl</Path>
    <ShaderType>Vertex</ShaderType>
  </Shader>
  <Shader>
    <Path>./frag.glsl</Path>
    <ShaderType>Fragment</ShaderType>
  </Shader>
</Asset>
```
