uniform float uScreenWidth;
uniform float uScreenHeight;
uniform float uTimer;


int siBinar(int a,int b)
{ 
	if (a < 0) a = -1-a;
	if (b < 0) b = -1-b;
	float result = (1.-abs(1.-mod(float(a),3.)))*(1.-abs(1.-mod(float(b),3.)));
	for (int i = 0;i < 7;i++) {
		a /= 3;
		b /= 3;
		result += (1.-abs(1.-mod(float(a),3.)))*(1.-abs(1.-mod(float(b),3.)))*2;
	}
	return result;
}

void main() 
{
	vec2 p = gl_FragCoord.xy - vec2(uScreenWidth, uScreenHeight)  * 0.5;
	p *= pow(3.,1.-fract(uTimer));
	float color = 0.0;
	float bin = siBinar(int(floor(p.x)),int(floor(p.y)));
	if (bin <= 1.0) {
		color = 1.0;
	}
	gl_FragColor = vec4(color, color, color, 1.0);

}