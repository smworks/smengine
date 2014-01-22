uniform mat4 uWVP;
uniform float uTimer;
attribute vec4 attrPos;

void main(void) {
	float PI = 3.14159;
	vec4 pos = attrPos;
	pos.y = 0.01 * sin(pos.x + pos.z + uTimer * 0.002);
	gl_Position = uWVP * pos;
}
