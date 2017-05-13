uniform mat4 uWVP;
attribute vec4 attrPos;
attribute vec2 attrUV;
varying vec2 varTexCoords;
void main(void)
{
   gl_Position = uWVP * attrPos;
   varTexCoords = attrUV;
}