uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

float PI  = 4.0*atan(1.0);

//vec3 sunLight  = normalize( vec3( 0.0, 0.45, 1.0 ) );
vec3 sunLight  = normalize( vec3( 0.0, (sin(uTimer / 2.0 ) + 0.5) / 2.0, 1.0 ) );
vec3 cameraPos = vec3(0,0,0);
vec3 camTar = vec3(0,0,1);
vec3 sunColour = vec3(1.0, .75, .6);
//vec3 sunColour = vec3((0.2 * sin(PI * 3.0/18.0 * uTimer )), (0.2 * sin(PI * 6.0/18.0 * uTimer )), (0.2 * sin(PI * 9.0/18.0 * uTimer )));
vec3 skyColor = vec3(.1, .2, .3);
float gTime = 0.0;
float sphereRadius = 6000.0;

//--------------------------------------------------------------------------
// Grab all sky information for a given ray from camera
vec3 GetSky(in vec3 rd)
{
	//Calculate suns strength based on dot product between ray vector and sun position vector
	float sunAmount = max( dot( rd, sunLight), 0.0 );
	float v = pow(1.0-max(rd.y, 0.0), 6.0);
	vec3 sky = mix(skyColor, vec3(.32, .32, .32), v);
	//Draw the sun
	sky = sky + sunColour * min(pow(sunAmount, 800.0)*1.5, .3);
	//Draw solar halo
	sky = sky + sunColour * sunAmount * sunAmount * .25;
	return clamp(sky, 0.0, 1.0);
}

//--------------------------------------------------------------------------
vec3 PostEffects(vec3 rgb, vec2 xy)
{
	// Gamma first...
	rgb = pow(rgb, vec3(0.45));
	
	// Then...
	#define CONTRAST 1.1
	#define SATURATION 1.3
	#define BRIGHTNESS 1.3
	rgb = mix(vec3(.5), mix(vec3(dot(vec3(.2125, .7154, .0721), rgb*BRIGHTNESS)), rgb*BRIGHTNESS, SATURATION), CONTRAST);
	// Vignette...
	rgb *= .4+0.5*pow(40.0*xy.x*xy.y*(1.0-xy.x)*(1.0-xy.y), 0.2 );	
	return rgb;
}

//--------------------------------------------------------------------------
void main(void)
{
	float gTime = (uTimer*5.0+2352.0)*.006;
    	vec2 xy = gl_FragCoord.xy / vec2(uScreenWidth, uScreenHeight);
	vec2 uv = (-1.0 + 2.0 * xy) * vec2(uScreenWidth/uScreenHeight,1.0);
	
	#ifdef STEREO
	float isCyan = mod(gl_FragCoord.x + mod(gl_FragCoord.y,2.0),2.0);
	#endif	
	
	float roll = 0.0;
	vec3 cw = normalize(camTar-cameraPos);
	vec3 cp = vec3(sin(roll), cos(roll),0.0);
	vec3 cu = cross(cw,cp);
	vec3 cv = cross(cu,cw);
	vec3 dir = normalize(uv.x*cu + uv.y*cv + 1.3*cw);
	mat3 camMat = mat3(cu, cv, cw);

	#ifdef STEREO
	cameraPos += .85*cu*isCyan; // move camera to the right - the rd vector is still good
	#endif

	vec3 col;
	float distance;
	float type;

	col = GetSky(dir);
	
	// Generate the lens flares...
	
	// bri is the brightness of sun at the centre of the camera direction.
	// Yeah, the lens flares is not exactly subtle, but it was good fun making it.
	float bri = dot(cw, sunLight)*.75;
	if (bri > 0.0)
	{
		vec2 sunPos = vec2( dot( sunLight, cu ), dot( sunLight, cv ) );
		vec2 uvT = uv-sunPos;
		uvT = uvT*(length(uvT));
		bri = pow(bri, 6.0)*.8;

		// glare = the red shifted blob...
		float glare1 = max(dot(normalize(vec3(dir.x, dir.y+.3, dir.z)),sunLight),0.0)*1.4;
		// glare2 is the yellow ring...
		float glare2 = max(1.0 - length( uvT + sunPos * .5) * 4.0, 0.0);
		uvT = mix (uvT, uv, -2.3);
		// glare3 is a purple splodge...
		float glare3 = max(1.0-length(uvT+sunPos*5.0)*1.2, 0.0);

		//col += bri * vec3(1.0, .0, .0)  * pow(glare1, 12.5)*.05;
		//col += bri * vec3(1.0, 1.0, 0.2) * pow(glare2, 2.0)*2.5;
		//col += bri * sunColour * pow(glare3, 2.0)*3.0;
	}
	col = PostEffects(col, xy);	
	
	#ifdef STEREO	
	col *= vec3( isCyan, 1.0-isCyan, 1.0-isCyan );	
	#endif
	
	gl_FragColor=vec4(col,1.0);
}

//--------------------------------------------------------------------------