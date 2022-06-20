precision mediump float;
varying vec4 fColor;
varying vec2 fTexCoord;

uniform sampler2D sampler2d;
uniform sampler2D toBlend;

void main() {

	 gl_FragColor = texture2D(sampler2d, fTexCoord) + texture2D(toBlend, fTexCoord);

}
