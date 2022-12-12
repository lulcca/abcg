#version 300 es

precision mediump float;

out vec4 FragColor;

in vec2 textCoord;
uniform sampler2D texture1;

void main() { FragColor = texture(texture1, textCoord); }