uniform sampler2D texture_0;
uniform vec4 uForeground;
uniform vec4 uBackground;
varying vec2 varTexCoords;

void main(void) {
	vec4 col = texture2D(texture_0, varTexCoords);
	
	float foregroundAlpha = col.a;
	float backgroundAlpha = 1.0 - col.a;
	
	float r = uForeground.r * foregroundAlpha + uBackground.r * backgroundAlpha;
	float g = uForeground.g * foregroundAlpha + uBackground.g * backgroundAlpha;
	float b = uForeground.b * foregroundAlpha + uBackground.b * backgroundAlpha;
	float a = uForeground.a * foregroundAlpha + uBackground.a * backgroundAlpha;
	
	gl_FragColor = vec4(r, g, b, a);
}