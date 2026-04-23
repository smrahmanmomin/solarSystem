#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    TexCoord = aTexCoord;
    // Remove translation from view matrix so skybox always surrounds camera
    mat4 rotView = mat4(mat3(view));
    vec4 pos = projection * rotView * vec4(aPos, 1.0);
    // Set z = w so depth is always 1.0 (behind everything)
    gl_Position = pos.xyww;
}
