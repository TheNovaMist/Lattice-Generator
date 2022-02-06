#version 330 core
#extension GL_ARB_gpu_shader_fp64 : enable

in vec3 color;
out vec4 Fragcolor;

void main()
{      
    Fragcolor = vec4(color, 0.5f);
}
