/** WATCH THE CLOUDS FOR AT LEAST 10 SECONDS **/

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

float hash( float p ) {
	return fract( sin(p) * 45678.23 );	
}

float hash( vec2 p ) {
	return fract( sin(p.x * 15.38 + p.y * 38.34) * 45678.23 );
}

vec2 hash2( vec2 p ) {
	mat2 m = mat2(15.38, 38.23, 93.23, 199.32);
	return fract( sin(p*m) * 45678.23 );	
}

float noise( vec2 p ) {
	vec2 g = floor(p);
	vec2 f = fract(p);	
	float lb = hash( g + vec2(0.0, 0.0) );
	float rb = hash( g + vec2(1.0, 0.0) );
	float lt = hash( g + vec2(0.0, 1.0) );
	float rt = hash( g + vec2(1.0, 1.0) );
	float b = mix( lb, rb, f.x );  // mix == blend == lerp
	float t = mix( lt, rt, f.x );
	float res = mix( b, t, f.y );
	return res;
}

float fbm( vec2 p ){
	float r = 0.0;
	r += noise(p) / 2.0;
	r += noise(p * 2.0) / 4.0;
	r += noise(p * 4.0) / 8.0;
	r += noise(p * 8.0) / 16.0;
	r += noise(p * 16.0) / 32.0;
	r += noise(p * 32.0) / 64.0;
	return r;
}



void main( void ) {
	vec2 p = gl_FragCoord.xy / vec2(uScreenWidth, uScreenHeight);
	p.y = 1./p.y;
	p.x = (p.x - .5)*p.y;

	
	//float np = noise(p*15.0);
	float np = fbm(p*30.0);
	
	//float d = distance( vec2(0.5, 0.5), p);
	//float d = distance( vec2(0.5, 0.5), p + np*0.1 );
	//float d = distance( vec2(0.5, 0.5), p ) + np * 0.1;
	//float d = distance( vec2(0.5, 0.5), p * np*0.1);
	float d = length(p)*.3 + np*.1;
	
	float c = sin( d * 3.141592 * 10.0 - uTimer * .3 );
	//float c = np;
	
	
	//gl_FragColor = vec4( sin(uTimer*p.x), 1.0, 1.0, 1.0 );
	vec3 sky = clamp(vec3(.7,.6,.8)+p.y*.115, 0., 1.1);
	
	gl_FragColor = vec4( mix(sky, vec3(1.1-c*.1,1,1), c), 1.0 );

}