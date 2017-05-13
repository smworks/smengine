uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = ( gl_FragCoord.xy / resolution.xy );
	vec2 sunPos = vec2( cos(uTimer*0.4) * 0.5 + 0.5, sin(uTimer*0.4)*0.40+0.5 );
	vec3 atmo = vec3(0.7, 0.9, 1.2);
	atmo -= 0.6*pow(position.y,1.0);
	atmo -= vec3( pow(1.0-position.y,(32.0-sunPos.y*16.0)), pow(1.0-position.y,(12.0-sunPos.y*6.0)), pow(1.0-position.y,(4.0-2.0)))*(1.6-sunPos.y*1.6);
	atmo += sunPos.y*pow(1.-position.y,4.0);
	atmo *= 0.4+sunPos.y;
	float s = length(position*vec2(1.777,1.0)-sunPos*vec2(1.777,1.0));
	s = 20./s;
	atmo+=atmo*s*.005;
	gl_FragColor = vec4( atmo*(1.0-sunPos.y*0.5), 1.0 );
}