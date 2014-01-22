#ifdef GL_FRAGMENT_PRECISION_HIGH
   // Default precision
   precision highp float;
#else
   precision mediump float;
#endif

uniform samplerCube cubeMap_0;
varying vec3 varEyeNormal;
varying vec3 varView;

void main(void)
{
	vec3 R = reflect(varView, varEyeNormal);
	gl_FragColor = textureCube(cubeMap_0, R);
}