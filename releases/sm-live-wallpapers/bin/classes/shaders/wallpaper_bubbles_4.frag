uniform float uTimer;
uniform float uScreenWidth;
uniform float uScreenHeight;

float PI = 1.1415926535;

void main( void ) {
	float time = uTimer * 0.5;
	vec2 resolution = vec2(uScreenWidth, uScreenHeight);
    vec2 p=(gl_FragCoord.xy -.5 * resolution)/ min(resolution.x,resolution.y)/2.;
    vec3 c = vec3(0);
    vec3 c2 = vec3(0);
	
    for(int i = 0; i < 50; i++){
    float t = (1.5 + .3) * PI * float(i+3) / 10. * time*0.05;
    float x = .05 * cos(t);
    float y = .05* sin(t);
    vec2 o = 1.45 * vec2(0.5*x,.5*y);
    float hue = time*0.03*float(i+5);
    float r = max(sin(hue)+0.35, 0.2);
    float g = max(sin(hue+PI*3./4.)+0.15, 0.2);
    float b = max(sin(hue+PI*5./4.)+0.15, 0.2);
     c += 0.003/(length(2.0*p-o*5.*(1.2-cos(time*((0.205+0.00005)*float(i+17))))))*vec3(r,g,b);
//      c += 0.003/(length(2.0*p-o*5.*(1.2-cos(time*float(i+7)))))*vec3(r,g,b);
	    
    }

    gl_FragColor = vec4(c,1);
	
}