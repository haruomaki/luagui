varying vec2 vuv;
uniform sampler2D texture;
uniform bool is_tex;
uniform vec4 baseColor;
varying vec4 vColor;

void main(void){
    if (is_tex) {
        gl_FragColor = texture2D(texture, vuv);
    } else {
        gl_FragColor = vColor * baseColor;
    }
}
