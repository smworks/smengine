uniform mat4 uWVP;
attribute vec4 attrPos;
void main(void)
{
   gl_Position = uWVP * attrPos;
}