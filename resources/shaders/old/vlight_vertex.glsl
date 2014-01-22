// Material data.
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uSpecIntensity;
uniform float uTransparency;

uniform mat4 uWVP;
uniform mat3 uN;
uniform vec3 uLightPos;
uniform vec3 uEyePos;
attribute vec4 attrPos;
attribute vec3 attrNorm;
varying vec4 varColor;

void main(void)
{
	vec3 N = uN * attrNorm;
	vec3 L = normalize(uLightPos);
	vec3 H = normalize(L + normalize(uEyePos));
	float df = max(0.0, dot(N, L));
	float sf = max(0.0, dot(N, H));
	sf = pow(sf, uSpecIntensity);
	
	vec3 col = uAmbient + uDiffuse * df + uSpecular * sf;
	varColor = vec4(col, uTransparency);
	gl_Position = uWVP * attrPos;
}