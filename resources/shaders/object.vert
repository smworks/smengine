uniform mat4 uWVP;
uniform mat4 uW;
uniform mat3 uN;
attribute vec4 attrPos;
attribute vec3 attrNorm;
varying vec3 varEyeNormal;
attribute vec2 attrUV;
varying vec2 varTexCoords;
varying vec4 varPos;

void main(void)
{
	varPos = uW * attrPos;
	varEyeNormal = uN * attrNorm;
	varTexCoords = attrUV;
	gl_Position = uWVP * attrPos;
}