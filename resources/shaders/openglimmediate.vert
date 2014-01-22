uniform mat4 uWVP;
attribute vec4 attrPos;
attribute vec4 attrCol;
attribute vec2 attrUV;
varying vec2 varTexCoords;
varying vec4 varColor;
void main(void)
{
	varColor = attrCol;
	gl_Position = uWVP * attrPos;
	varTexCoords = attrUV;
}