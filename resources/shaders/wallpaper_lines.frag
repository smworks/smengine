// By Michal Cwiek
// cwiek.michal at g mail dot com

uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

const float AMPLITUDE_1 = 0.05;
const float AMPLITUDE_2 = 0.1;
const float AMPLITUDE_3 = 0.15;

const vec3 wave4_1 = vec3(0.02, 0.05, 0.03);
const vec3 wave4_2 = vec3(0.02, 0.05, 0.03);
const vec3 wave4_3 = vec3(0.02, 0.05, 0.03);
	
const float WAVE_OFFSET_SMALL  = 5.0;
const float WAVE_OFFSET_MEDIUM = 15.0;

// Default noise
float rand(vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main( void ) 
{
	vec2 p = ( gl_FragCoord.yx / vec2(uScreenWidth, uScreenHeight)) * 2.0 - 0.3;
	vec3 c = vec3( 0.0 );
	float waveShineFactor = mix( 0.10, 0.4, 0.3 * sin(uTimer) + 0.5);
	float starShineFactor = mix( 0.10, 0.4, 1.5 * sin(atan(uTimer * 0.2)) + 0.9);
	c += vec3(0.05, 0.05, 0.15) * (  waveShineFactor        * abs( 1.0 / sin( p.x + 0.04  + sin( p.y + uTimer + WAVE_OFFSET_SMALL )    * AMPLITUDE_2 ) ));
	c += vec3(0.05, 0.05, 0.15) * (  waveShineFactor * 0.4  * abs( 1.0 / sin( p.x + 0.07  + sin( p.y + uTimer + WAVE_OFFSET_SMALL )    * AMPLITUDE_2 - 0.01 ) ));
	c += vec3(0.05, 0.05, 0.15) * (  waveShineFactor * 0.3  * abs( 1.0 / sin( p.x + 0.11  + sin( p.y + uTimer + WAVE_OFFSET_SMALL )    * AMPLITUDE_2 - 0.02 ) ));
	c += vec3(0.05, 0.05, 0.15) * (  waveShineFactor * 0.2  * abs( 1.0 / sin( p.x + 0.14  + sin( p.y + uTimer + WAVE_OFFSET_SMALL )    * AMPLITUDE_2 - 0.03 ) ));
	c += vec3(0.05, 0.05, 0.15) * (  waveShineFactor * 0.1  * abs( 1.0 / sin( p.x + 0.15  + sin( p.y + uTimer + WAVE_OFFSET_SMALL )    * AMPLITUDE_2 - 0.04 ) ));
	c += wave4_1 * (  waveShineFactor        * abs( 1.0 / sin( p.x         + sin( p.y + sin(uTimer)    + WAVE_OFFSET_MEDIUM )      * AMPLITUDE_3 ) ));
	c += wave4_2 * (  waveShineFactor        * abs( 1.0 / sin( p.x         + sin( p.y + sin(uTimer/2.) + WAVE_OFFSET_MEDIUM )      * AMPLITUDE_3 ) ));
	c += wave4_3 * (  waveShineFactor        * abs( 1.0 / sin( p.x         + sin( p.y + sin(uTimer/4.) + WAVE_OFFSET_MEDIUM )      * AMPLITUDE_3 ) ));
	vec4 color = vec4(c,1.0);
	gl_FragColor = color;

}