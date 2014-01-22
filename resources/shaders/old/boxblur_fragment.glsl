#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform sampler2D sscene;
varying vec2 varTexCoords;
int count = 8;
float pixel = 1.0 / 512.0;

void main(void)
{
	if (varTexCoords.x > 0.5) {
		vec4 color = vec4(0.0);
		float num = 0.0;
		for (int y = -count; y <= count; y++) {
			for (int x = -count; x <= count; x++) {
				color += texture2D(sscene, varTexCoords + vec2(x, y) * pixel);
				num += 1.0;
			}
		}
		gl_FragColor = color / num;
		//vec4 col = texture2D(sscene, varTexCoords);
		//float val = 1.0f - pow(gl_FragCoord.z, 60);
		//gl_FragColor = vec4(vec3(val), 1.0) * col;
	}
	else {
		gl_FragColor = texture2D(sscene, varTexCoords);
	}
}