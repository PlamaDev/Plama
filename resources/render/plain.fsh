#version 130

in vec4 vecPos_;
in vec4 vecNormal_;
uniform vec3 vecLight;
uniform vec3 vecView;
uniform vec3 vecSpec;
uniform float fDiff;
uniform float fAmbient;
uniform mat4 matColor;
out vec4 ret;

void main() {
    vec4 vecLight4 = vec4(normalize(vecLight), 1.0f);
    vec4 dirNormal = normalize(vecNormal_);
    vec4 dirlight = normalize(vecLight4 - vecPos_);
    float diff = max(fDiff * dot(dirlight, dirNormal), 0.0f);
    vec4 color = min(fAmbient + diff, 1.0f) * matColor * vecPos_;
    vec4 dirReflect = reflect(-dirlight, dirNormal);
    vec4 dirView = normalize(vec4(vecView, 1.0f) - vecPos_);
    float fSpec = max(dot(dirReflect, dirView), 0);
    ret = color + pow(fSpec, 4.0f) * vec4(vecSpec, 1.0f);
}
