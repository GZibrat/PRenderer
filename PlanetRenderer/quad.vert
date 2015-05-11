#version 450

in vec2 position;
in vec3 uv;

out vec3 fuv;

void main()
{
	fuv = uv;
	gl_Position = vec4(position, 0.0, 1.0);
}