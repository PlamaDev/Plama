#version 130

in vec3 vecPnt;
in vec3 vecColor;
flat out vec4 vecColorF;
uniform mat4 matTrans;
uniform mat4 matModel;

void main() {
    vec4 vecPntW = matModel * vec4(vecPnt, 1.0f);
    gl_Position = matTrans * vecPntW;
    vecColorF = vec4(vecColor, 1.0f);
}
