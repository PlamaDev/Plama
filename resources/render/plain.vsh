#version 130

in vec3 vecPos;
in vec3 vecNormal;
out vec4 vecPos_;
out vec4 vecNormal_;
uniform mat4 matTrans;
uniform mat4 matModel;
uniform mat4 matNormal;

void main() {
//    vec4 posModel = matModel * vec4(vecPos, 1.0f);
//    vec4 normTran = matNormal * vec4(vecNormal, 0.0f);
//    vecNormal_ = normalize(normTran.xyz);
//    vec4 colorBase = matColor * posModel;
//    vec3 dirLight = normalize(vecLight - posModel.xyz);
//    float fDiffuse = max(dot(dirLight, vecNormal_), 0.0f);
//    vecCol = (fAmbient + fDiffuse) * colorBase;
//    gl_Position = matTrans * posModel;
//    vecPos_ = posModel.xyz;

    vecPos_ = matModel * vec4(vecPos, 1.0f);
    vecNormal_ = matNormal * vec4(vecNormal, 0.0f);

    //vecCol = vecNormal_;
    //vecCol = (fAmbient + diff) * matColor * posWorld;

    gl_Position = matTrans * vecPos_;
}
