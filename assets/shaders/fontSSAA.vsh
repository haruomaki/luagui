#version 330 core

in vec2 position;
out vec2 texCoord;

void main(void){
    texCoord = (position + vec2(1, 1)) / 2;
    gl_Position = vec4(position, 0.0, 1.0);
}
