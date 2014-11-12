uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;


void main()
{
	float time = uTimer * 0.2;
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	float v,t=v=.0005;
	vec3 cen=gl_FragCoord.xyz;
	cen.xy=((cen.xy/resolution.xy)-0.5)*resolution.xy;
	cen.xy+= 1000.0;
	for (float s=.0; s<2.; s+=.01) {
		vec3 p=s*(cen+vec3(.6,.6,.6))*t+vec3(.1,.2,fract(s+floor(time*0.)*.01));
		for (int i=0; i<8; i++) p=abs(p)/dot(p,p)-.8;
		v+=dot(p,p)*t;
	}
	gl_FragColor=vec4(v, v, v, 1.0);
}