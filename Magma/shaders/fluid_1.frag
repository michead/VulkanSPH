#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform sampler2D depthTex;
layout(location = 0) in vec2 texCoord;
layout(location = 0) out vec4 color;

void main() {
  float depth = texture(depthTex, texCoord).r;
  color = vec4(vec3(depth), 1);
}
