uniform samplerCube cubeMap_0;
varying vec3 varCubeTexCoords;

void main(void)
{
	vec4 col = textureCube(cubeMap_0, varCubeTexCoords);
	gl_FragColor = col;
}
