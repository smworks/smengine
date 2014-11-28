uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

float hash2(vec2 uv) {
	return fract(sin(uv.x * 15.78 + uv.y * 35.14) * 43758.23);
}

void main( void ) {
	vec2 p = gl_FragCoord.xy / vec2(uScreenWidth, uScreenHeight);
	float color = 0.1 + hash2(p*uTimer)*0.5;	
	gl_FragColor = vec4(color, color, color, 1.0);
}
