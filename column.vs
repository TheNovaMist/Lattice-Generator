#version 330 core
layout (location = 0) in vec3 position;

out vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform vec3 setColor;

void main()
{
    color = setColor;
    gl_Position = view * model * vec4(position, 1.0f);
} 
