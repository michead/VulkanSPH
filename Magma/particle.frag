#version 400
#extension GL_ARB_separate_shader_objects  : enable
#extension GL_ARB_shading_language_420pack : enable

#define FLUID_COLOR   vec4(0, 0, 1, 1)
#define AMBIENT_TERM  vec4(0.01, 0.01, 0.3, 1)

layout(location = 0) out vec4 color;

struct Light {
  vec3 pos;
  ke   color;
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
  ndcPos.xy = (2.0 * gl_FragCoord.xy) / (viewport.zw) - 1.0;
  ndcPos.z  = (2.0 * gl_FragCoord.z - gl_DepthRange.near - gl_DepthRange.far) / (gl_DepthRange.far - gl_DepthRange.near);

  vec4 clipPos;
  clipPos.w   = mvp[3][2] / (ndcPos.z - (mvp[2][2] / mvp[2][3]));
  clipPos.xyz = ndcPos * clipPos.w;
  vec4 eyePos = invMvp * clipPos;

  vec4 pxPos = vec4(eyePos.xyz + normal * particleSize * 0.5, 1);
  vec4 csPos = mvp * pxPos;
  gl_FragDepth = csPos.z /= csPos.w;

  color = ambientColor;

  for (uint i = 0; i < lightCount; i++) {
    vec3 lightDir = normalize(lightPos - eyePos.xyz);
    color += max(0, dot(normal, lightDir)) * fluidDiffuse;
  }
}
