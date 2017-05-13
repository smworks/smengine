uniform float uScreenWidth;
uniform float uScreenHeight;
uniform float     uTimer;

vec3 GrassBlade (vec3 col, vec2 pos, vec3 grass, float soft) {
	float thickness = pow(1.5-pos.y, 3.0) * 0.02;
	float core = thickness - soft;
	
	pos.y *= 1.3 + 0.3 * sin(pos.x * 4.5324) + 0.3 * sin(pos.x * 7.5245);
	pos.x += sin(pos.x * 1.3423) * 0.5;
	pos.x = mod(pos.x, 0.1);
	vec2 a = vec2(0.05, 0.0);
	vec2 l = vec2(0.05, 0.2);
	vec2 aP = pos-a;
	vec2 al = l-a;
	vec3 aP3 = vec3(aP, 0.0);
	vec3 al3 = vec3(al, 0.0);
	float q = length(cross(aP3,al3))/length(al3);
	
	float d = q;
	if ( dot(al, aP) <= 0.0 ) { // before start
               d = distance(pos, a);
	}
        else if ( dot(al, al) <= dot(al, aP) ) { // after end
               d = distance(pos, l);
	}
	float alpha = (1.0 - ((d - core) / soft) ) * (step(d, thickness));
	alpha = max(0.0, min(1.0, alpha));
	col = (col*(1.0-alpha)) + (grass*alpha);
	return col;
}

void main( void ) {
	vec2 position = ( gl_FragCoord.xy / vec2(uScreenWidth, uScreenHeight) );
	position.x *= uScreenWidth/uScreenHeight;
	vec3 grass = vec3(0.01, 0.25, 0.0);
	vec3 fog = vec3(0.5, 0.55, 0.6);
	vec3 sky = vec3(0.4, 0.5, 0.65);
	
	float skyfog = position.y * 3.5 - 1.6;
	if (position.y < 0.3) {
		gl_FragColor.rgb = grass;
	} else {
		gl_FragColor.rgb = sky*skyfog + fog * (1.0 - skyfog);
	}
	gl_FragColor.a = 1.0;
	for (float n=0.0; n<20.0; n+= 1.0) {
		float foga = n/25.0;
		gl_FragColor.rgb = GrassBlade(gl_FragColor.rgb,
					      vec2(position.x * (21.0-n) * 0.15 + sin(sin(n)*94.3456)*37.245 + uTimer * (0.1 + 0.03*n),
						   position.y * (21.0-n) * 0.1 + 0.35 - ((21.0-n)*0.07)),
					      grass*foga + fog*(1.0-foga),
					      abs(n-17.0) * 0.01);
	}
}