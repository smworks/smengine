uniform sampler2D mainTexture;
varying vec2 varTexCoords;
uniform float uTransparency;

void main(void) {
	vec4 col = texture2D(mainTexture, varTexCoords);
	//col.a = uTransparency * col.a;
	gl_FragColor = col;
}
