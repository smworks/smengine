uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
varying vec2 varTexCoords;

void main()
{
	vec4 col = texture2D(colorBuffer, varTexCoords);
	col += vec4(-0.3);
	gl_FragColor = col;
}
