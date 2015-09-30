// Material data.
uniform vec3 uAmbient;
uniform vec3 uDiffuse;
uniform vec3 uSpecular;
uniform float uSpecIntensity;
uniform float uTransparency;
// Eye data.
uniform vec3 uEyePos;
varying vec3 varEyeNormal;
// Texture data.
uniform sampler2D mainTexture;
uniform float uMainTexture;
varying vec2 varTexCoords;
// Light data.
uniform vec3 uLightPos[8];
uniform int uLightCount;
varying vec4 varPos;

void main(void) {
	vec3 N = normalize(varEyeNormal);
	vec3 L = normalize(vec3(1.0, 1.0, 1.0));
	vec3 H = normalize(L + normalize(uEyePos));
	
	float df = max(0.0, dot(N, L));
	
	vec4 col = vec4(1.0);
	float sf = max(0.0, dot(N, H));
	sf = pow(sf, uSpecIntensity);
	col.xyz = uAmbient + uDiffuse * df + uSpecular * sf;
	col.w = uTransparency;
	vec4 colTex = texture2D(mainTexture, varTexCoords);
		
	if (uMainTexture > 0.5) {
		gl_FragColor = col * colTex;
	} else {
		gl_FragColor = col;
	}
}
