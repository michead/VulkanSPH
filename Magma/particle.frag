#version 400
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_NUM_LIGHTS  32
#define FLUID_COLOR     vec4(0, 0, 1, 1)
#define AMBIENT_TERM    vec4(0.01, 0.01, 0.3, 1)

layout(location = 0) out vec4 color;

struct Light {
  vec3 pos;
  vec4 ke;
};

layout(binding = 0) uniform globals {
  float particleSize;
  vec4  fluidDiffuse;
  vec4  ambientColor;
  mat4  mvp;
  mat4  invMvp;
  ivec4 viewport;
  uint  lightCount;
  Light lights[MAX_NUM_LIGHTS];
} uniforms;

void main() {
  vec3 normal;
  normal.xy = gl_PointCoord * 2.0 - 1.0;
  
  float r2 = dot(normal.xy, normal.xy);
  if (r2 > 1) {
    discard;
  }
  
  normal.z = -sqrt(1 - r2);

  vec3 ndcPos;
  ndcPos.xy = (2.0 * gl_FragCoord.xy) / (uniforms.viewport.zw) - 1.0;
  ndcPos.z  = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);

  vec4 clipPos;
  clipPos.w   = uniforms.mvp[3][2] / (ndcPos.z - (uniforms.mvp[2][2] / uniforms.mvp[2][3]));
  clipPos.xyz = ndcPos * clipPos.w;
  vec4 eyePos = uniforms.invMvp * clipPos;

  vec4 pxPos = vec4(eyePos.xyz + normal * uniforms.particleSize * 0.5, 1);
  vec4 csPos = uniforms.mvp * pxPos;
  gl_FragDepth = csPos.z /= csPos.w;

  color = uniforms.ambientColor;

  for (uint i = 0; i < uniforms.lightCount; i++) {
    Light light = uniforms.lights[i];
    vec3 lightDir = normalize(light.pos - eyePos.xyz);
    color += max(0, dot(normal, lightDir)) * uniforms.fluidDiffuse;
  }
}
