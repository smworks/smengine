#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform sampler2D sscene;
varying vec2 varTexCoords;
float xrange = 0.1;
float stepping = 0.01;
float scale = 1.0;
float maxDist = 0.75;

void main(void)
{
	vec4 blur = vec4(0.0);
	vec2 coord = varTexCoords;
	// Vector from screen center.
	vec2 dir = coord - vec2(0.5, 0.5);
	float dist = length(dir);
	dir /= dist; // Normalize.
	// Take samples in both directions.
	for (float x = -xrange; x <= xrange; x += stepping) {
		vec2 tex = clamp(coord + dir * x * scale, 0.01, 0.99);
		blur += texture2D(sscene, tex);
	}
	blur /= (2.0 * xrange / stepping);
	// Mix based on distance.
	vec4 color = texture2D(sscene, coord);
	gl_FragColor = mix(color, blur, dist / maxDist);
}