uniform mat4 uWVP;
attribute vec4 attrPos;
attribute vec2 attrUV;
varying vec2 varTexCoords;
void main() {
	varTexCoords = attrUV;
	gl_Position = uWVP * attrPos;
}