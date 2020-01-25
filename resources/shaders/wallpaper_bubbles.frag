uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;
varying vec2 varTexCoords;

// rakesh@picovico.com : www.picovico.com

uniform vec2 resolution;
const float fRadius = 0.2;

void main(void)
{
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
	vec2 uv = -1.0 + 2.0*gl_FragCoord.xy / resolution.xy;
	uv.x *=  resolution.x / resolution.y;
	
	vec3 color = vec3(0.0);

    	// bubbles
	for( int i=0; i<64; i++ )
	{
        	// bubble seeds
		float pha = sin(float(i)*8.13+1.0)*0.5 + 0.5;
		float siz = pow( sin(float(i)*1.74+5.0)*0.5 + 0.5, 4.0 );
		float pox = sin(float(i)*3.55+4.1) * resolution.x / resolution.y;
		
        	// buble size, position and color
		float rad = fRadius;
		vec2  pos = vec2( pox+sin(uTimer/30.+pha+siz), -1.0-rad + (2.0+2.0*rad)
						 *mod(pha+0.1*(uTimer/5.)*(0.2+0.8*siz),1.0)) * vec2(1.0, 1.0);
		float dis = length( uv - pos );
		vec3  col = mix( vec3(0.2, 0.6, 0.0), vec3(0.0,0.2,0.8), 0.5+0.5*sin(float(i)*sin(uTimer*pox*0.03)+1.9));
		
        	// render
		color += col.xyz *(1.- smoothstep( rad*(0.65+0.20*sin(pox*uTimer)), rad, dis )) * (1.0 - cos(pox*uTimer));
	}

	gl_FragColor = vec4(color,2.0);
}