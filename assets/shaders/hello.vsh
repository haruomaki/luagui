#version 330 core

in vec3 position;
in vec4 color;
out vec4 vColor;

void main(void){
    gl_Position = vec4(position, 1.0);
    vColor = color;
}
