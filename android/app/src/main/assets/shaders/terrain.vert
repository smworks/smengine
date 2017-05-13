uniform mat4 uWVP;
uniform mat3 uN;
uniform vec3 uEyePos;
attribute vec4 attrPos;
attribute vec3 attrNorm;
attribute vec2 attrUV;
varying vec3 varEyeNormal;
varying vec2 varTexCoords;

void main(void)
{
	varEyeNormal = uN * attrNorm;
	varTexCoords = attrUV;
	gl_Position = uWVP * attrPos;
}