#version 430 core

in vec3 position;
in float color;

uniform mat4 MVP;
out vec3 fcolor;

void main()
{
	gl_Position = MVP * vec4(position, 1.0);
//	if (color < 0.0)fcolor = vec3(0.0, 0.0, 1.0);
//	else if(color > 0.0) fcolor = vec3(1.0, 0.0, 0.0);
//	else fcolor = vec3(0.0, 0.0, 0.0);

	fcolor = vec3(color);
	//if (color <= 0.0)fcolor = vec3(0.0, 0.0, 1.0);
	//else fcolor = vec3(1.0, 0.0, 0.0);
}