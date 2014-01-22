#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform sampler2D sscene;
varying vec2 varTexCoords;
int dim = 5;
float sum = 256.0;
float pixel_scale = 1.0/512.0;
float strength = 4.5;

void main(void)
{
	float gauss[25];
	gauss[0] = 1.0;
	gauss[1] = 4.0;
	gauss[2] = 6.0;
	gauss[3] = 4.0;
	gauss[4] = 1.0;
	gauss[5] = 4.0;
	gauss[6] = 16.0;
	gauss[7] = 24.0;
	gauss[8] = 16.0;
	gauss[9] = 4.0;
	gauss[10] = 6.0;
	gauss[11] = 24.0;
	gauss[12] = 36.0;
	gauss[13] = 24.0;
	gauss[14] = 6.0;
	gauss[15] = 4.0;
	gauss[16] = 16.0;
	gauss[17] = 24.0;
	gauss[18] = 16.0;
	gauss[19] = 4.0;
	gauss[20] = 1.0;
	gauss[21] = 4.0;
	gauss[22] = 6.0;
	gauss[23] = 4.0;
	gauss[24] = 1.0;
	
	if (varTexCoords.x > 0.5) {
		vec4 color = vec4(0);
		float half_dim = float(dim) * 0.5;
		for (int y = 0; y < dim; y++) {
			for (int x = 0; x < dim; x++) {
				vec2 offset = vec2(-half_dim + float(x), -half_dim + float(y)) * pixel_scale * strength;
				vec2 coord = varTexCoords + offset;
				color += texture2D(sscene, coord) * gauss[y * 5 + x];
			}
		}
		gl_FragColor = color / sum;
	}
	else {
		gl_FragColor = texture2D(sscene, varTexCoords);
	}
}