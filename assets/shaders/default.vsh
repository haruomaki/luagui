#version 330 core

in vec3 position;
in vec2 uv;
in vec4 color;
in mat4 instanceModelMatrix;
out vec2 vuv;
out vec4 vColor;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main(void){
    gl_Position = projectionMatrix * viewMatrix * instanceModelMatrix * vec4(position, 1.0);
    vuv = uv;
    vColor = color;
}
