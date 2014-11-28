uniform float uTimer;

float rand(vec2 co){
	return fract(sin(dot(co, vec2(12.9898, 78.233))) * 100.0);
}

void main () {
	vec2 p = gl_FragCoord.xy * 0.05; // Multiply screen from smaller value to work with regions bigger than one pixel.
	float brightness = rand(floor(p) + uTimer * 0.00001); // Brightness intensity.
	brightness *= 1.0 - length(fract(p) - vec2(0.5, 0.5)); // Distribute quare light intensity.
	gl_FragColor = vec4(0.0, brightness, brightness * 2.0, 1.0);
}