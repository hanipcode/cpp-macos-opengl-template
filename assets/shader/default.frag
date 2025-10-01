#version 330 core

out vec4 FragColor;

in vec3 color;
in vec2 texCoord;

uniform sampler2D tex0;
uniform vec2 magnifyCenter;
uniform float magnifyRadius;
uniform float magnifyZoom;

void main() {
  float dist = distance(texCoord, magnifyCenter);

  vec2 uv = texCoord;
  if (dist < magnifyRadius) {
    uv = (texCoord - magnifyCenter) / magnifyZoom + magnifyCenter;
  }
  FragColor = texture(tex0, uv);
}
