uniform sampler2D texture_0;
uniform vec4 uForeground;
varying vec2 varTexCoords;

void main(void) {
	vec4 col = texture2D(texture_0, varTexCoords);
	gl_FragColor = vec4(uForeground.rgb, col.a * uForeground.a);
}