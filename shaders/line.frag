#version 330 core

in vec3 Color;
out vec4 FragColor;

uniform float alpha;

void main()
{
    FragColor = vec4(Color, alpha);
}
