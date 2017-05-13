#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

// Material data.
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uSpecIntensity;
uniform float uTransparency;
uniform vec3 uEyePos;

uniform sampler2D brick_wall;
uniform sampler2D brick_wall_n;
varying vec2 varTexCoords;

//////////////////////////////////////////////////////////////////////
uniform vec3 uLightPos[8];
uniform int uLightCount;
varying vec4 varPos;
//--------------------------------------------------------------------
// Name: lightIntensity()
// Desc: calculate light intensity.
//--------------------------------------------------------------------
float lightIntensity() {
	float intensity = 0.0;
	for (int i = 0; i < uLightCount; i++) {
		float dist = length(uLightPos[i] - varPos.xyz);
		intensity += 1.0 / dist;
	}
	return intensity;
}
//////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------
// Name: main
// Desc: applies effects.
//--------------------------------------------------------------------
void main(void)
{	
	vec2 texCoords = vec2(varTexCoords.x * 300.0, varTexCoords.y * 2.0);
	vec3 col = texture2D(brick_wall, texCoords).xyz * lightIntensity();
	//vec3 normal = texture2D(brick_wall_n, texCoords).xyz * 2.0 - 1.0;
	gl_FragColor = vec4(col, 1.0);
}