/**
*	@file DepthOfField.frag
*/

#version 430

layout(location=0) in vec2 inTexCoord;

out vec4 fragColor;

uniform sampler2D texColorArray[2];	// 0:カラー,1:深度

//ポアソンディスクサンプリングのための座標リスト
const int poissonSampleCount = 12;
const vec2 poissonDisk[poissonSampleCount] = {
  { -0.326, -0.406 }, { -0.840, -0.074 }, { -0.696,  0.457 },
  { -0.203,  0.621 }, {  0.962, -0.195 }, {  0.473, -0.480 },
  {  0.519,  0.767 }, {  0.185, -0.893 }, {  0.507,  0.064 },
  {  0.896,  0.412 }, { -0.322, -0.933 }, { -0.792, -0.598 }
};

/**
*	被写界深度シェーダ
*/
void main(){

	float focalPlane = 0.935;
	float centerZ = texture(texColorArray[1], inTexCoord).r;
	float scale = (centerZ - focalPlane) * 0.1;
	fragColor.rgb = texture(texColorArray[0], inTexCoord).rgb;
	for(int i = 0;i < poissonSampleCount; ++i){
		
		vec2 uv = inTexCoord + poissonDisk[i] * scale;
		fragColor.rgb += texture(texColorArray[0], uv).rgb;
	}
	fragColor.rgb = fragColor.rgb / float(poissonSampleCount + 1);
	fragColor.a = 1.0;

}