uniform vec3 uAmbient;
uniform float uTransparency;
uniform sampler2D mainTexture;
uniform float uMainTexture;
varying vec2 varTexCoords;

void main(void)
{
	if (uMainTexture > 0.5) {
		gl_FragColor = texture2D(mainTexture, varTexCoords);
	} else {
		gl_FragColor = vec4(uAmbient, uTransparency);
	}
}