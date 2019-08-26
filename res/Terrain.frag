/**
*	@file Terrain.frag
*/
#version 430

layout(location=0) in vec3 inPosition;
layout(location=1) in vec2 inTexCoord;
layout(location=2) in vec3 inNormal;

out vec4 fragColor;

uniform sampler2D texColor;

/*
*	地形用フラグメントシェーダ
*/
void main(){

	//暫定で一つの平行高原を置く
//	vec3 vLight = normalize(vec3(1,-2,-1));
//	float power = max(dot(normalize(inNormal),-vLight),0.0) + 0.2;

//	fragColor = texture(texColor,inTexCoord);
//	fragColor.rgb *= power;
	vec2 texCoord = inTexCoord -0.5;
	float y = (inPosition.y + 5) * 0.05;
	fragColor = vec4(texCoord.x,texCoord.y,y,1);
}