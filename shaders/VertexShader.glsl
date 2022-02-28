#version 330

layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_tex;
layout(location = 3) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float CurrentTime;
uniform vec3 Color;

// Output
out vec3 frag_color;
out vec3 frag_normal;


void main()
{
    frag_color = Color;
    frag_normal = v_normal;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);

}
