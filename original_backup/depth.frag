precision mediump float;
varying vec4 fColor;
varying vec2 fTexCoord;

uniform sampler2D sampler2d;
uniform sampler2D bgDepthTexture;

uniform float windowW;
uniform float windowH;

void main()
{

	float bgU = gl_FragCoord.x/windowW;
	float bgV = gl_FragCoord.y/windowH;

	float bgDepth = texture2D(bgDepthTexture, vec2(bgU,bgV)).r;

	if(bgDepth >= gl_FragCoord.z)
		discard;
	else
		gl_FragColor = vec4(1.0, 0.0, 1.0, 1.0);

}
