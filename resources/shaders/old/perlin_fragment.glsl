#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform vec3 uAmbient;
varying vec2 varTexCoords;

float random(int x, int y)
{
	int seed = 50;
	int n = (x + seed) + (y + seed) * 57;
	n = (n << 13) * n;
	return (1.0 - ((n * (n * n * 19417 + 189851) + 4967243) & 4945007) / 3354521.0);
}

float noise(vec2 v)
{
	int x = floor(v.x);
	int y = floor(v.y);
	float s = frac(v.x);
	float t = frac(v.y);
	float v1 = random(x, y);
	float v2 = random(x + 1, y);
	float v3 = random(x, y + 1);
	float v4 = random(x + 1, y + 1);
	float j1 = mix(v1, v2, s);
	float j2 = mix(v3, v4, s);
	return lerp(j1, j2, t);
}

float noise2D(vec2 v, int oct, float freq, float amp, float pers, float lacu)
{
	float ret = 0.0;
	for (int i = 0; i < oct; i++)
	{
		ret += noise(v * freq)  * amp;
		amp *= pers;
		freq *= lacu;
	}
	return ret;
}

void main(void)
{
	vec2 tiling = vec2(1.0, 1.0);
	int octaves = 10; // Changes color composition.
	float amplitude = 1.0; // Color intensity. Low - blank colors, high - big contrast.
	float frequency = 200.0; // Detail level.
	float lacunarity = 0.35; // Gap between patterns.
	float persistence = 0.98; // Low value makes repeatable pattern visible.
	vec4 color1 = vec4(0);
	
	varTexCoords = frac(varTexCoords * tiling);
	
	float v0 = noise2D(varTexCoords, octaves, frequency, amplitude, persistence, lacunarity);
	float v1 = noise2D(varTexCoords + vec2(1.0, 0.0), octaves, frequency, amplitude, persistence, lacunarity);
	float v2 = noise2D(varTexCoords + vec2(0.0, 1.0), octaves, frequency, amplitude, persistence, lacunarity);
	float v3 = noise2D(varTexCoords + vec2(1.0, 1.0), octaves, frequency, amplitude, persistence, lacunarity);
	
	vec2 grad = 1.0 - varTexCoords;
	float z0 = mix(v0, v1, grad.x);
	float z1 = mix(v2, v3, grad.x);
	float t = mix(z0, z1, grad.y) + 1.0;
	
	gl_FragColor = mix(color1, vec4(uAmbient, 1.0), t * 0.5);
}