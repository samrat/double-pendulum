#version 330

uniform float color;
out vec4 outColor;

void main() {
  outColor = vec4(color, color, 1.0, 1.0);
}
