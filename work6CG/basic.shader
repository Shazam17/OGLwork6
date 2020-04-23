#shader vertex
#version 330 core

layout(location = 0) in vec3 pos;


uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_proj;

void main() {
	gl_Position = u_proj*u_view*u_model*vec4(pos, 1);
}


#shader fragment
#version 330 core

uniform vec3 renderColor;

out vec4 color;

void main() {
	color = vec4(renderColor, 1);
}