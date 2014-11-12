uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

float hash2(vec2 uv) {
	return fract(sin(uv.x * 15.78 + uv.y * 35.14) * 43758.23);
}

void main( void ) {
	vec2 res = vec2(uScreenWidth/uScreenHeight,1);
	vec2 p = ( gl_FragCoord.xy / uScreenHeight) - (res / 2.0);
	vec3 color = vec3(0);
	color += 0.1 + hash2(p*uTimer)*0.15;	
	gl_FragColor = vec4(vec3( color ), 1.0);
}