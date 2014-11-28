uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

mat2 k=mat2(vec2(0.707,0.707),vec2(0.707,-0.707));
float v=0.0;
float l=0.0;

void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = ( gl_FragCoord.xy / resolution.xy ) -0.5;
	position*=5.;
	for(float i=0.;i<10.;i++){
		//v=sin(position.y)+sin(position.x);
		v+=length(vec2(cos(position.y+sin(uTimer+position.y)),-sin(position.x+uTimer*1.2)));
		position.x+=cos(position.y+sin(uTimer+position.y));
		position.y-=sin(position.x+uTimer*1.2);
		position.x*=2.;
		position.y*=2.;
		//position=mat2(vec2(sin(position.y),cos(position.x)),vec2(cos(position.y),-sin(position.x)))*position;
		l=i;
		//if(abs(v)>7.0) break;
	}
	
	gl_FragColor = vec4( vec3(pow(v/5.-2.,2.0)), 1.0 );
}