#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec2 fragTexCoord;

layout(input_attachment_index = 1, binding = 0) uniform subpassInput depthTex;

layout(location = 0) out vec4 color;

void main() {
  float depth = subpassLoad(depthTex).r;
  color = vec4(vec3(depth), 1);
}
