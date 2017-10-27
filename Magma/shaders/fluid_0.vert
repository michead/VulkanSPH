#version 450
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

layout(binding = 0) uniform globals {
  mat4  view;
  mat4  proj;
  float particleSize;
} uniforms;

layout(location = 0) in vec4 pos;

out gl_PerVertex{
  vec4  gl_Position;
  float gl_PointSize;
};

void main() {
  gl_PointSize = uniforms.particleSize;
  gl_Position  = uniforms.proj * uniforms.view * pos;
}
