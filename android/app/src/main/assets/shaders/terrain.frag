// Material data.
uniform vec3 uAmbient;
uniform vec3 uDiffuse;

uniform vec3 uLightPos[8];
uniform vec3 uEyePos;
uniform sampler2D terrain_blend;
uniform sampler2D grass;
uniform sampler2D rock;
uniform sampler2D dirt;

varying vec2 varTexCoords;
varying vec3 varEyeNormal;

void main(void)
{
	vec3 N = normalize(varEyeNormal);
	vec3 L = normalize(uLightPos[0]);
	float df = max(0.0, dot(N, L));
	vec3 col = uAmbient + uDiffuse * df;
	// Take color information from textures and tile them.
	vec2 tiledCoords = fract(varTexCoords * 40.0);
	vec4 colGrass = texture2D(grass, tiledCoords);
	vec4 colDirt = texture2D(dirt, tiledCoords);
	vec4 colRock = texture2D(rock, tiledCoords);
	// Take color information from not tiled blend map.
	vec4 colBlend = texture2D(terrain_blend, varTexCoords);
	// Find the inverse of all the blend weights.
	float inverse = 1.0 / (colBlend.r + colBlend.g + colBlend.b);
	// Scale colors by its corresponding weight.
	colGrass *= colBlend.r * inverse;
	colDirt *= colBlend.g * inverse;
	colRock *= colBlend.b * inverse;
	// Calculate final color.
	gl_FragColor = vec4(col, 1.0) * (colGrass + colDirt + colRock);
}
