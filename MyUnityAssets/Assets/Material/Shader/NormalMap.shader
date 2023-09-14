Shader "HLSL/NormalMap"
{
    Properties
    {
        _MainTex ("Texture", 2D) = "white" {}
        _BaseColor("BaseColor", color) = (1,1,1,1)
        [Normal]_NormalTex("Normal",2D)="bump"{}
        _NormalScale("NormalScale",Range(0,1))=1
        _SpecularRange("SpecularRange",Range(10,300))=10
        [HDR]_SpecularColor("SpecularColor",Color)=(1,1,1,1)
    }
    SubShader
    {
        Tags {"RenderPipeLine"="UniversalRenderPipeline" }

        HLSLINCLUDE

        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"

        CBUFFER_START(UnityPerMaterial)
        float4 _NormalTex_ST;
        float4  _MainTex_ST;
        half4 _BaseColor;
        real _NormalScale;
        float4 _SpecularColor;
        float _SpecularRange;
        CBUFFER_END

        TEXTURE2D(_MainTex);
        TEXTURE2D(_NormalTex);
        SAMPLER(sampler_MainTex);
        SAMPLER(sampler_NormalTex);

        struct a2v
        {
            float4 positionOS : POSITION;
            float3 normalOS : NORMAL;
            float2 texcoord : TEXCOORD;
            float4 tangentOS : TANGENT;
        };

        struct v2f
        {
            float4 positionCS : SV_POSITION;
            float4 texcoord : TEXCOORD0;
            float4 BtangentWS : TEXCOORD1;
            float4 tangentWS : TANGENT;
            float4 normalWS : NORMAL;
        };

        ENDHLSL

        Pass
        {
            Tags{"LightMode" = "UniversalForward"}

            HLSLPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            v2f vert (a2v i)
            {
                v2f o;
                o.positionCS = TransformObjectToHClip(i.positionOS.xyz);
                o.texcoord.xy = TRANSFORM_TEX(i.texcoord, _MainTex);
                o.texcoord.zw = TRANSFORM_TEX(i.texcoord, _NormalTex);
                o.tangentWS.xyz = normalize(TransformObjectToWorldDir(i.tangentOS.xyz));
                //TransformObjectToWorldDir把法線和切線變換到世界空間，並計算出世界空間下的副法線，基於unity_WorldTransformParams.w和vertexTangent.w來判斷副法線是否需要取反。
                o.normalWS.xyz = TransformObjectToWorldNormal(i.normalOS.xyz,true);
                o.BtangentWS.xyz = cross(o.normalWS.xyz,o.tangentWS.xyz)*i.tangentOS.w*unity_WorldTransformParams.w;
                //這裡乘一個unity_WorldTransformParams.w是為判斷是否使用了奇數相反的縮放
                //而對於紋理UV來說，OpenGL 從左到右(0,1)，從下到上（0，1）DirectX 從左到右(0,1)
                //從下到上（1，0）所以，如果紋理被鏡像，則需要w來存儲這個值，所以這就是為甚麼要乘i.tangentOS.w
                float3 positionWS = TransformObjectToWorld(i.positionOS.xyz);
                o.tangentWS.w = positionWS.x;
                o.BtangentWS.w = positionWS.y;
                o.normalWS.w = positionWS.z;
                return o;
            }

            real4 frag (v2f i) : SV_Target
            {
                float3 WSpos = float3(i.tangentWS.w,i.BtangentWS.w,i.normalWS.w);
                //不想設太多struct v2f所使用的方法，反正w軸很少會用到
                float3x3 T2W = {i.tangentWS.xyz,i.BtangentWS.xyz,i.normalWS.xyz}; 
                //三個彼此互相垂直的方向，能經由矩陣方式來轉動，跟TRS的轉動方法一樣
                real4 nortex = SAMPLE_TEXTURE2D(_NormalTex,sampler_NormalTex,i.texcoord.zw);
                float3 normalTS = UnpackNormalScale(nortex,_NormalScale); 
                //UnpackNormalScale與UnpackNormal功能完全相同，額外提供了Scale參數供縮放法線值至-scale~scale值域
                normalTS.z = pow((1-pow(normalTS.x,2)-pow(normalTS.y,2)),0.5); 
                //平面轉立體所以需要找到z,一般來說沒有經過normal轉換的地方都是為(0.5,0.5,1)(因為(0,0,1)*0.5=0.5)
                float3 norWS = mul(normalTS,T2W); //norWS重新改變法向量的方向
                Light mylight = GetMainLight();
                float halflambot = saturate(dot(norWS,normalize(mylight.direction))*0.5+0.5); 
                real4 diff = SAMPLE_TEXTURE2D(_MainTex,sampler_MainTex,i.texcoord.xy)*halflambot*_BaseColor*real4(mylight.color,1);//半漫反射
                float spe = pow(saturate(dot(normalize(normalize(mylight.direction)+normalize(_WorldSpaceCameraPos-WSpos)),norWS)),_SpecularRange);//高光
                return saturate(spe*_SpecularColor)+diff;

            }
            ENDHLSL
        }
    }
}