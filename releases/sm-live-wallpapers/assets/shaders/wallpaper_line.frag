// By Michal Cwiek
// cwiek.michal at g mail dot com

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

#define PI 0.01

void main( void ) {
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 p = ( gl_FragCoord.xy / resolution.xy ) - 0.5;
	
	float sx = 0.25*sin( 12.0 * p.x - uTimer);
	
	float dy = 5./ ( 30. * abs(p.y - sx));
	
	gl_FragColor = vec4( dy-1.35, 0.5 * dy, (p.x + 0.5) * dy, 1.0 );
}
