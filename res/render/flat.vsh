#version 130

in vec3 vecPos;
in vec3 vecPnt;
in vec3 vecNormal;
in vec3 vecColor;
flat out vec4 vecColorF;
uniform mat4 matTrans;
uniform mat4 matModel;
uniform mat4 matNormal;
uniform vec3 vecLight;
uniform vec3 vecView;

void main() {
    vec4 vecPntW = matModel * vec4(vecPnt, 1.0f);
    vec4 vecPosW = matModel * vec4(vecPos, 1.0f);
    vec4 vecNormalT = matNormal * vec4(vecNormal, 0.0f);
    vec4 vecNormalW = normalize(vec4(vecNormalT.xyz, 0.0f));
    vec3 vecLightIn = normalize(vecLight - vecPosW.xyz);
    vec3 vecColorLight = vec3(1, 1, 1);
    vec3 vecViewIn = normalize(vecView - vecPosW.xyz);
    vec3 vecReflect = reflect(-vecLightIn, vecNormalW.xyz);
    float spec = pow(max(dot(vecViewIn, vecReflect), 0.0), 8);
    float base = 0.55 + max(0.3 * dot(vecNormalW.xyz, vecLightIn), 0.0f);
    vecColorF = vec4(base*vecColor + 0.3*spec*vecColorLight, 1.0f);
    gl_Position = matTrans * vecPntW;
}
