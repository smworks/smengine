uniform mat4 uWVP;
uniform mat3 uN;
uniform vec3 uEyePos;
attribute vec4 attrPos;
attribute vec3 attrNorm;
varying vec3 varEyeNormal;
varying vec3 varView;

void main(void)
{
	varEyeNormal = uN * attrNorm;
	vec4 pos = uWVP * attrPos;
	varView = normalize(pos.xyz - uEyePos);
	gl_Position = pos;
	
}