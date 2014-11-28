uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
uniform float uScreenWidth;
varying vec2 varTexCoords;

vec3 horizontalBlur() {
	float h = 1.0 / uScreenWidth;
	vec4 sum = vec4(0.0);
	sum += texture2D(colorBuffer, vec2(varTexCoords.x - 4.0 * h, varTexCoords.y)) * 0.051;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x - 3.0 * h, varTexCoords.y)) * 0.0918;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x - 2.0 * h, varTexCoords.y)) * 0.12245;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x - 1.0 * h, varTexCoords.y)) * 0.1531;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y)) * 0.1633;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x + 1.0 * h, varTexCoords.y)) * 0.1531;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x + 2.0 * h, varTexCoords.y)) * 0.12245;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x + 3.0 * h, varTexCoords.y)) * 0.0918;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x + 4.0 * h, varTexCoords.y)) * 0.051;
	return sum.rgb;
}

void main() {
	gl_FragColor = vec4(horizontalBlur(), 1.0);
}