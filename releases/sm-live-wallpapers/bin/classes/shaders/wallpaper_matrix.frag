uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;


float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float chr(vec2 outer, vec2 inner) {
	//return float(rand(floor(inner * 2.0) + outer) > 0.9);
	
	vec2 seed = floor(inner * 4.0) + outer.y;
	if (rand(vec2(outer.y, 23.0)) > 0.98) {
		seed += floor((uTimer + rand(vec2(outer.y, 41.0))) * 3.0);
	}
	
	return float(rand(seed) > .4);
}

void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = gl_FragCoord.xy / resolution.xy;
	position.y /= resolution.x / resolution.y;

	float rx = gl_FragCoord.x / 0.05;
	float mx = mod(gl_FragCoord.x, 10.0);
	
	if (mx > 7.0) {
		gl_FragColor = vec4(0);
	} else {
        	float x = floor(rx);
		float ry = gl_FragCoord.y + rand(vec2(x, x * 13.0)) * 100000.0 + uTimer * rand(vec2(x, 23.0)) * 120.0;
		float my = mod(ry, 15.0);
		if (my > 12.0) {
			gl_FragColor = vec4(0);
		} else {
		
			float y = floor(ry / 15.0);
			
			float b = chr(vec2(rx, floor((ry) / 15.0)), vec2(mx, my) / 12.0);
			float col = max(mod(-y, 24.0) - 4.0, 0.0) / 20.0;
			vec3 c = col < 0.8 ? vec3(0.0, col / 0.8, 0.0) : mix(vec3(0.0, 1.0, 0.0), vec3(1.0), (col - 0.8) / 0.2);
			
			gl_FragColor = vec4(c * b, 1.0);
		}
	}
}