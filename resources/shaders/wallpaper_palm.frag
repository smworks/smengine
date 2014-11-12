uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

void main( void ) {

	vec2 uv = gl_FragCoord.xy/vec2(uScreenWidth, uScreenHeight);
	vec2 q = uv - vec2(.3,.7);
	float r = 0.2 + .1*cos(atan(q.y,q.x)*10. + 15.*q.x*(0.5+0.5*sin(uTimer)) + 1.);
	vec3 col = mix(vec3(1.,.14,.1), vec3(1.,.8,.0), sqrt(uv.y));
	
	col *= smoothstep(r, r+.04, length(q));
	r = .01;
	r += .003*cos(120.*q.y);
	r += exp(-50.*uv.y);
	col *= 1.- (1.-smoothstep(r,r+.01, abs(q.x - .2*sin(2.*q.y))))*(1.- smoothstep(0.,0.01,q.y));
	
	gl_FragColor = vec4(col,1.);
}
