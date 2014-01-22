uniform sampler2D colorBuffer;
uniform sampler2D depthBuffer;
uniform float uScreenWidth;
uniform float uScreenHeight;
varying vec2 varTexCoords;

vec4 boxBlur() {
	int count = 8;
	vec2 texel = vec2(1.0 / uScreenWidth, 1.0 / uScreenHeight);
	
	vec4 color = vec4(0.0);
	float num = 0.0;
	for (int y = -count; y <= count; y++) {
		for (int x = -count; x <= count; x++) {
			color += texture2D(colorBuffer, varTexCoords + vec2(x, y) * texel);
			num += 1.0;
		}
	}
	return color / num;
}

vec4 radialBlur() {
	float xrange = 0.1;
	float stepping = 0.01;
	float scale = 1.0;
	float maxDist = 0.75;

	vec4 blur = vec4(0.0);
	vec2 coord = varTexCoords;
	// Vector from screen center.
	vec2 dir = coord - vec2(0.5, 0.5);
	float dist = length(dir);
	dir /= dist; // Normalize.
	// Take samples in both directions.
	for (float x = -xrange; x <= xrange; x += stepping) {
		vec2 tex = clamp(coord + dir * x * scale, 0.01, 0.99);
		blur += texture2D(colorBuffer, tex);
	}
	blur /= (2.0 * xrange / stepping);
	// Mix based on distance.
	vec4 color = texture2D(colorBuffer, coord);
	return mix(color, blur, dist / maxDist);
}

vec4 poissonBlur() {
	int num = 64;
	float D = 25.0;
	vec2 texel = vec2(1.0 / uScreenWidth, 1.0 / uScreenHeight);

	vec2 poissonDisk[64];
	poissonDisk[0] = vec2(-0.613392, 0.617481);
	poissonDisk[1] = vec2(0.170019, -0.040254);
	poissonDisk[2] = vec2(-0.299417, 0.791925);
	poissonDisk[3] = vec2(0.645680, 0.493210);
	poissonDisk[4] = vec2(-0.651784, 0.717887);
	poissonDisk[5] = vec2(0.421003, 0.027070);
	poissonDisk[6] = vec2(-0.817194, -0.271096);
	poissonDisk[7] = vec2(-0.705374, -0.668203);
	poissonDisk[8] = vec2(0.977050, -0.108615);
	poissonDisk[9] = vec2(0.063326, 0.142369);
	poissonDisk[10] = vec2(0.203528, 0.214331);
	poissonDisk[11] = vec2(-0.667531, 0.326090);
	poissonDisk[12] = vec2(-0.098422, -0.295755);
	poissonDisk[13] = vec2(-0.885922, 0.215369);
	poissonDisk[14] = vec2(0.566637, 0.605213);
	poissonDisk[15] = vec2(0.039766, -0.396100);
	poissonDisk[16] = vec2(0.751946, 0.453352);
	poissonDisk[17] = vec2(0.078707, -0.715323);
	poissonDisk[18] = vec2(-0.075838, -0.529344);
	poissonDisk[19] = vec2(0.724479, -0.580798);
	poissonDisk[20] = vec2(0.222999, -0.215125);
	poissonDisk[21] = vec2(-0.467574, -0.405438);
	poissonDisk[22] = vec2(-0.248268, -0.814753);
	poissonDisk[23] = vec2(0.354411, -0.887570);
	poissonDisk[24] = vec2(0.175817, 0.382366);
	poissonDisk[25] = vec2(0.487472, -0.063082);
	poissonDisk[26] = vec2(-0.084078, 0.898312);
	poissonDisk[27] = vec2(0.488876, -0.783441);
	poissonDisk[28] = vec2(0.470016, 0.217933);
	poissonDisk[29] = vec2(-0.696890, -0.549791);
	poissonDisk[30] = vec2(-0.149693, 0.605762);
	poissonDisk[31] = vec2(0.034211, 0.979980);
	poissonDisk[32] = vec2(0.503098, -0.308878);
	poissonDisk[33] = vec2(-0.016205, -0.872921);
	poissonDisk[34] = vec2(0.385784, -0.393902);
	poissonDisk[35] = vec2(-0.146886, -0.859249);
	poissonDisk[36] = vec2(0.643361, 0.164098);
	poissonDisk[37] = vec2(0.634388, -0.049471);
	poissonDisk[38] = vec2(-0.688894, 0.007843);
	poissonDisk[39] = vec2(0.464034, -0.188818);
	poissonDisk[40] = vec2(-0.440840, 0.137486);
	poissonDisk[41] = vec2(0.364483, 0.511704);
	poissonDisk[42] = vec2(0.034028, 0.325968);
	poissonDisk[43] = vec2(0.099094, -0.308023);
	poissonDisk[44] = vec2(0.693960, -0.366253);
	poissonDisk[45] = vec2(0.678884, -0.204688);
	poissonDisk[46] = vec2(0.001801, 0.780328);
	poissonDisk[47] = vec2(0.145177, -0.898984);
	poissonDisk[48] = vec2(0.062655, -0.611866);
	poissonDisk[49] = vec2(0.315226, -0.604297);
	poissonDisk[50] = vec2(-0.780145, 0.486251);
	poissonDisk[51] = vec2(-0.371868, 0.882138);
	poissonDisk[52] = vec2(0.200476, 0.494430);
	poissonDisk[53] = vec2(-0.494552, -0.711051);
	poissonDisk[54] = vec2(0.612476, 0.705252);
	poissonDisk[55] = vec2(-0.578845, -0.768792);
	poissonDisk[56] = vec2(-0.772454, -0.090976);
	poissonDisk[57] = vec2(0.504440, 0.372295);
	poissonDisk[58] = vec2(0.155736, 0.065157);
	poissonDisk[59] = vec2(0.391522, 0.849605);
	poissonDisk[60] = vec2(-0.620106, -0.328104);
	poissonDisk[61] = vec2(0.789239, -0.419965);
	poissonDisk[62] = vec2(-0.545396, 0.538133);
	poissonDisk[63] = vec2(-0.178564, -0.596057);

	vec4 color = vec4(0.0);
	for (int i = 0; i < num; i++) {
		vec2 offset = poissonDisk[i] * texel * D;
		color += texture2D(colorBuffer, varTexCoords + offset);
	}
	return color / float(num);
}

vec4 gaussianBlur() {
	int dim = 5;
	float sum = 256.0;
	vec2 texel = vec2(1.0 / uScreenWidth, 1.0 / uScreenHeight);
	float strength = 2.0;
	
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
	
	vec4 color = vec4(0.0);
	float half_dim = float(dim) * 0.5;
	for (int y = 0; y < dim; y++) {
		for (int x = 0; x < dim; x++) {
			vec2 offset = vec2(-half_dim + float(x), -half_dim + float(y)) * texel * strength;
			vec2 coord = varTexCoords + offset;
			color += texture2D(colorBuffer, coord) * gauss[y * 5 + x];
		}
	}
	return color / sum;
}

vec4 dof(vec4 color) {
	vec4 col = texture2D(colorBuffer, varTexCoords);
	float depth = pow(texture2D(depthBuffer, varTexCoords).r, 16.0);
	return mix(col, color, depth);
}

vec4 godRay() {
    float samples = 128.0;
    float intensity = 0.125;
	float decay = 0.96875;
    vec2 texCoords = varTexCoords;
    vec2 dir = vec2(0.5) - texCoords;
    dir /= samples;
    vec3 col = texture2D(colorBuffer, texCoords).rgb;
    
    for (float i = 0.0; i < samples; i++)
    {
        col += texture2D(colorBuffer, texCoords).rgb * intensity;
        intensity *= decay;
        texCoords += dir;
    }
    
    return vec4(col, 1.0);
}

void main(void)
{
	//gl_FragColor = dof(vec4(0.0, 0.0, 0.0, 1.0));
	gl_FragColor = dof(gaussianBlur());

	/* Unrelated effect, just for style: blueish
	gl_FragColor.r *= 3.0 / 4.0;
	gl_FragColor.g *= 3.0 / 4.0;
	gl_FragColor.b *= 6.0 / 4.0;*/
}
