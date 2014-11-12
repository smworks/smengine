uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

#define pi 3.141592
#define num_balls 7.

void main( void ) 
{
	float time = uTimer * 0.2;
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 position = ( gl_FragCoord.xy -  resolution.xy*.5 ) / resolution.x;

	// 256 angle steps
	float angle = atan(position.y,position.x)/(2.*3.14159265359);
	angle -= floor(angle);
	float rad = length(position);
	
	vec4 color = vec4(0.0);
	for (int i = 0; i < 16; i++) {
		float angleFract = fract(angle*256.);
		float angleRnd = floor(angle*256.)+1.;
		float angleRnd1 = fract(angleRnd*fract(angleRnd*.7235)*45.1);
		float angleRnd2 = fract(angleRnd*fract(angleRnd*.82657)*13.724);
		float t = time+angleRnd1*10.;
		float radDist = sqrt(angleRnd2+float(i));
		
		float adist = radDist/rad*.1;
		float dist = (t*.1+adist);
		dist = abs(fract(dist)-.5);
		float c = sin(float(i) / 4.0) + 1.0;
		color += (max(0.,.5-dist*40./adist)*(.5-abs(angleFract-.5))*5./adist/radDist) * vec4(c*angleRnd1,c*angleRnd2,c*(angleRnd1+angleRnd2),1.0);
		
		angle = fract(angle+.61);
	}
	
	gl_FragColor = color;
}