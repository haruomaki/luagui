attribute vec3 position;
attribute vec2 uv;
attribute vec4 color;
varying vec2 vuv;
varying vec4 vColor;
uniform mat4 modelViewMatrix;

void main(void){
    gl_Position = vec4(position, 1.0);
    vuv = uv;
    vColor = color;
}
