uniform mat4 uWVP;
uniform mat3 uN;
attribute vec4 attrPos;
attribute vec3 attrNorm;
varying vec3 varEyeNormal;

void main(void)
{
	varEyeNormal = uN * attrNorm;
	gl_Position = uWVP * attrPos;
}