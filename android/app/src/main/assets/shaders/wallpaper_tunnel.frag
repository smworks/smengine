/* lame-ass tunnel by kusma */

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = (gl_FragCoord.xy - resolution * 0.5) / resolution.yy;
	float th = atan(position.y, position.x) / (2.0 * 3.1415926) + 0.5;
	float dd = length(position);
	float d = 0.15 / dd + uTimer;

	vec3 uv = vec3(th + d, th - d, th + sin(d) * 0.1);
	float a = 0.5 + cos(uv.x * 3.1415926 * 2.0) * 0.5;
	float b = 0.5 + cos(uv.y * 3.1415926 * 2.0) * 0.5;
	float c = 0.5 + cos(uv.z * 3.1415926 * 6.0) * 0.5;
	vec3 color = mix(vec3(1.0, 01.8, 01.9), vec3(01.1, 01.1, 01.2), pow(a, 0.2)) * 0.75;
	color += mix(vec3(0.08, 0.09, 1.0), vec3(00.1, 01.1, 01.2),  pow(b, 0.1)) * 0.75;
	color += mix(vec3(0.09, 0.08, 1.0), vec3(01.1, 01.2, 01.2),  pow(c, 0.1)) * 0.75;
	gl_FragColor = vec4(color * clamp(dd, 0.0, 1.0), 1.0);
}