//thanks for bumping this :))
// I think I made a mistake when I wrote the fiery part, it should look more like this.

uniform float uTimer;
varying vec2 varTexCoords;

float Hash( vec2 p) {
	vec3 p2 = vec3(p.xy,1.0);
	return fract(sin(dot(p2,vec3(37.1,61.7, 12.4)))*3758.5453123);
}

float noise(in vec2 p)
{
    vec2 i = floor(p);
	vec2 f = fract(p);
	f *= f * (3.0-2.0*f);

    return mix(mix(Hash(i + vec2(0.,0.)), Hash(i + vec2(1.,0.)),f.x),
			mix(Hash(i + vec2(0.,1.)), Hash(i + vec2(1.,1.)),f.x),
			f.y);


}

float fbm(vec2 p) {
	float v = 0.0;
	v += noise(p*1.)*.5;
	v += noise(p*2.)*.25;
	v += noise(p*4.)*.125;
	return v;
}

void main(void) {

	vec2 pos = varTexCoords;
	
	vec2 opos=pos;

	float flag=1.0;
	vec3 color = vec3(1.0);
	pos.x = pos.x+fract(uTimer*.1)*1.2;
	float d = pos.x+pos.y*0.5-0.3;
	d+=0.5*fbm(opos*15.1);


	if (d >0.35)
		color = clamp((color-(d-0.35)*10.),0.0,1.0);
	if (d >0.47 && d < 0.99)
	{
		float b = 1.-abs(d-.48)*33.3;
		color += flag * b*1.5*(0.0+noise(100.*pos+vec2(-uTimer*3.,0.)))*vec3(1.5,0.5,0.0);
		color += b*0.5*(0.0+noise(100.*pos+vec2(-uTimer*3.,0.)))*vec3(1.5,0.5,0.0);
	}
	
	gl_FragColor = vec4(color, 1.0);
}
