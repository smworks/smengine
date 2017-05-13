uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
uniform float uScreenHeight;
varying vec2 varTexCoords;

vec3 verticalBlur() {
	float v = 1.0 / uScreenHeight;
	vec4 sum = vec4(0.0);
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y - 4.0 * v)) * 0.051;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y - 3.0 * v)) * 0.0918;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y - 2.0 * v)) * 0.12245;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y - 1.0 * v)) * 0.1531;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y)) * 0.1633;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y + 1.0 * v)) * 0.1531;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y + 2.0 * v)) * 0.12245;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y + 3.0 * v)) * 0.0918;
	sum += texture2D(colorBuffer, vec2(varTexCoords.x, varTexCoords.y + 4.0 * v)) * 0.051;
	return sum.rgb;
}

void main() {
	gl_FragColor = vec4(verticalBlur(), 1.0);
}