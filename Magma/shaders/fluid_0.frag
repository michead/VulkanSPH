#version 450
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

#define MAX_NUM_LIGHTS    32
#define FLUID_COLOR       vec4(0, 0, 1, 1)
#define AMBIENT_TERM      vec4(0.01, 0.01, 0.3, 1)
#define DEPTH_RANGE_NEAR  0.0
#define DEPTH_RANGE_FAR   1.0

layout(location = 0) out vec4 color;

layout(binding = 1, std140) uniform globals {
  vec4  fluidDiffuse;
  vec4  ambientColor;
  mat4  proj;
  mat4  invProj;
  ivec4 viewport;
  float particleSize;
  uint  lightCount;
  vec4  lightPos[MAX_NUM_LIGHTS];
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
  ndcPos.z  = (2.0 * gl_FragCoord.z - DEPTH_RANGE_NEAR - DEPTH_RANGE_FAR) / (DEPTH_RANGE_FAR  - DEPTH_RANGE_NEAR);

  vec4 clipPos;
  clipPos.w   = uniforms.proj[3][2] / (ndcPos.z - (uniforms.proj[2][2] / uniforms.proj[2][3]));
  clipPos.xyz = ndcPos * clipPos.w;
  vec4 eyePos = uniforms.invProj * clipPos;

  vec4 pxPos = vec4(eyePos.xyz + normal * uniforms.particleSize * 0.5, 1);
  vec4 csPos = uniforms.proj * pxPos;
  gl_FragDepth = csPos.z /= csPos.w;

  color = uniforms.ambientColor;

  for (uint i = 0; i < uniforms.lightCount; i++) {
    vec3 lightPos = uniforms.lightPos[i].xyz;
    vec3 lightDir = normalize(lightPos - eyePos.xyz);
    color += max(0, dot(normal, lightDir)) * uniforms.fluidDiffuse;
  }
}
