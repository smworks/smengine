#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

varying vec4 varColor;

void main(void)
{
	
	//vec4 col = texture2D(texture_0, varTexCoords);
	//col.r += uLightDir.x;
	//col.rgb = vec3(pow(gl_FragCoord.z, 64));
	gl_FragColor = varColor;
	
}