uniform sampler2D mainTexture;
varying vec2 varTexCoords;
void main(void)
{
	vec4 col = texture2D(mainTexture, varTexCoords);
	gl_FragColor = col;
}
