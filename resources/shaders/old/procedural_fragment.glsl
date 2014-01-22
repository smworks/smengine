#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform sampler2D texture_0;
uniform int uTimer;
varying vec2 varTexCoords;

void main(void)
{

    vec2 z, c;
	float scale = 2.5; // / (float(uTimer) * 0.2);
	vec2 center = vec2(0.0, 0.0);
	int iter = 1000;
	
    c.x = 1.3333 * (varTexCoords.x - 0.5) * scale - center.x;
    c.y = (varTexCoords.y - 0.5) * scale - center.y;

    int i;
    z = c;
    for(i=0; i<iter; i++) {
        float x = (z.x * z.x - z.y * z.y) + c.x;
        float y = (z.y * z.x + z.x * z.y) + c.y;

        if((x * x + y * y) > 4.0) break;
        z.x = x;
        z.y = y;
    }

	float comp = float(i) / 100.0;
    gl_FragColor = vec4(comp, comp, comp, comp + 0.2);
}