Shader "HLSL/Lambert"
{
    Properties
    {
        _Gradual ("Gradual", 2D) = "white" {}
        _MainTex ("Texture", 2D) = "white" {}
        _BaseColor("BaseColor", color) = (1,1,1,1)
        _SpecularRange("SpecularRange",Range(10,300))=10
        _SpecularColor("SpecularColor",Color)=(1,1,1,1)
    }
    SubShader
    {
        Tags {"RenderType"="Opaque" "RenderPipeLine"="UniversalRenderPipeline" }

        HLSLINCLUDE

        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"

        CBUFFER_START(UnityPerMaterial)
        float4 _MainTex_ST;
        float4 _Gradual_ST; 
        half4 _BaseColor;
        float4 _SpecularColor;
        float _SpecularRange;
        CBUFFER_END

        TEXTURE2D(_MainTex);
        SAMPLER(sampler_MainTex);

        sampler2D _Gradual;
        //因為Gradual並不需要對應紋理座標，所以sampler2D就行，

        struct a2v
        {
            float4 positionOS : POSITION;
            float3 normalOS : NORMAL;
            float2 texcoord : TEXCOORD;
        };

        struct v2f
        {
            float4 positionCS : SV_POSITION;
            float2 texcoord : TEXCOORD;
            float3 viewDirWS : TEXCOORD1;
            float3 normalWS : NORMAL;
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
                o.texcoord = TRANSFORM_TEX(i.texcoord, _MainTex);
                o.viewDirWS = normalize(_WorldSpaceCameraPos.xyz-TransformObjectToWorld(i.positionOS.xyz));
                o.normalWS = TransformObjectToWorldNormal(i.normalOS.xyz,true); //normal定式寫法
                return o;
            }

            real4 frag (v2f i) : SV_Target
            {
                real4 tex = SAMPLE_TEXTURE2D(_MainTex,sampler_MainTex,i.texcoord)*_BaseColor; //表面顏色
                Light light = GetMainLight();
                real4 lightColor = real4(light.color,1); //光的顏色
                float3 lightNormal = normalize(light.direction); //光的方向
                real4 hightlight = pow(saturate(dot(normalize(i.viewDirWS+lightNormal),i.normalWS)),_SpecularRange)*_SpecularColor;
                //float lightDot = saturate(dot(lightNormal,i.normalWS)); //漫反射 //saturate 0以下就會返回0 1以上就會返回1
                float lightDot = dot(lightNormal,i.normalWS)*0.5+0.5; //半漫反射
                //return tex*lightDot*lightColor+hightlight; //漫反射
                return tex2D(_Gradual,float2(lightDot,0.5))*_BaseColor; //漸層
                //tex2D -> _Gradual的UV與(lightDot,0.5)對應(UV軸對應XY軸)，並回傳_Gradual中的顏色
            }
            ENDHLSL
        }
    }
}
