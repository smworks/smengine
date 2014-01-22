uniform mat4 uWVP;
uniform mat4 uW;
attribute vec4 attrPos;
attribute vec2 attrUV;
varying vec2 varTexCoords;
varying vec4 varPos;

void main(void)
{
	varPos = uW * attrPos;
	gl_Position = uWVP * attrPos;
	varTexCoords = attrUV;
}