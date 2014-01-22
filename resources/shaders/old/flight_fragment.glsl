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

uniform vec3 uLightPos[8];
uniform vec3 uEyePos;
varying vec3 varEyeNormal;

void main(void)
{
	vec3 N = normalize(varEyeNormal);
	vec3 L = normalize(uLightPos[0]);
	vec3 H = normalize(L + normalize(uEyePos));
	
	float df = max(0.0, dot(N, L));
	float sf = max(0.0, dot(N, H));
	sf = pow(sf, uSpecIntensity);
	
	vec3 col = uAmbient + uDiffuse * df + uSpecular * sf;

	gl_FragColor = vec4(col, uTransparency);
}