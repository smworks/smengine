uniform mat4 uWVP;
attribute vec4 attrPos;
varying vec3 varCubeTexCoords;
void main(void)
{
	varCubeTexCoords = attrPos.xyz;
	gl_Position = uWVP * attrPos;
}