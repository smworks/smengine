uniform vec3 uAmbient;
uniform vec3 uDiffuse;

//---------------------------------------------------------
// Calculates fog factor for specified pixel.
// Value 1.442695 is the result of log2.
//---------------------------------------------------------
/*float fogFactor() {
	float z = gl_FragCoord.z / gl_FragCoord.w;
	float fogFactor = exp2(
		-uFogDensity * uFogDensity * z * z * 1.442695);
	return clamp(fogFactor, 0.0, 1.0);
}*/

float linearDepth() {
	float n = 0.1; // camera z near
	float f = 300.0; // camera z far
	float zoverw =gl_FragCoord.z;
	return (2.0 * n) / (f + n - zoverw * (f - n));
}

void main(void)
{
	float depth = 1.0 - linearDepth();
	gl_FragColor = vec4(depth, 0, 0, 1.0);
}