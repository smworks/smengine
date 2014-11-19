uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

#define AMPLITUDE 0.1
#define WAVE_1 vec3(0.05, 0.05, 0.15)
#define WAVE_2 vec3(0.02, 0.03, 0.05)

void main()
{
	vec2 p = gl_FragCoord.yx / vec2(uScreenWidth, uScreenHeight) - uScreenHeight / uScreenWidth * 0.5;
	vec3 c = vec3(0.0);
	float shineFactor = mix(0.10, 0.4, 0.3 * sin(uTimer) + 0.5) * 2.0;
	float height = sin(p.y + uTimer) * AMPLITUDE;
	c += WAVE_2 * (shineFactor * abs(1.0 / sin(p.x + height)));
	c += WAVE_2 * (shineFactor * abs(1.0 / sin(p.x - height)));
	gl_FragColor = vec4(c, 1.0);

}