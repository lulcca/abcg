#version 300 es

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTextCoord;

out vec2 textCoord;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main() {
  textCoord = aTextCoord;
  gl_Position = proj * view * model * vec4(aPos, 1.0);
}