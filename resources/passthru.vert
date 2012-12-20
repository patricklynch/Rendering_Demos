uniform mat4		modelViewMatrix;
uniform mat4		transformMatrix;
uniform mat4		projectionMatrix;
uniform float		nInvert;

varying vec3		worldPos;
varying vec3		normal;

void main(void)
{
	gl_TexCoord[0]	= gl_MultiTexCoord0;
	
	vec4 pos		= transformMatrix * gl_Vertex;
	gl_Position		= projectionMatrix * modelViewMatrix * pos;
	worldPos		= pos.xyz;
	normal			= vec4( transformMatrix * vec4( gl_Normal, 0.0 ) ).xyz * nInvert;
}
