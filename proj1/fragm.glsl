#version 330 core

in vec3 ourColor;
in vec3 ourNormal;
in vec3 ourFragPos;
in vec2 ourTex;

out vec4 fragColor;

uniform vec3 eye;
uniform vec3 lightPos;
uniform sampler2D map;

void main() {
   vec4 color = texture(map, ourTex);
    fragColor = color;
}
