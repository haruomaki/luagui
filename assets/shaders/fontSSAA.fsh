#version 330 core

in vec2 texCoord;
uniform sampler2D texture;

void main(void){
    // gl_FragColor = texture2D(texture, texCoord);
    gl_FragColor = vec4(1, 1, 0, 1);
}
