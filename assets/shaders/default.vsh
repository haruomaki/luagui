#version 330 core

in vec3 position;
in vec2 uv;
in vec4 color;
out vec2 vuv;
out vec4 vColor;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;

void main(void){
    gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0);
    vuv = uv;
    vColor = color;
}
