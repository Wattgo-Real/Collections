Shader "HLSL/Transparent"
{
    Properties
    {
        _MainTex("Texture", 2D) = "white" {}
        _BaseColor("BaseColor", color) = (1,1,1,1)
        _Cutoff("Cutoff", float) = 1
        [HDR]_BurnColor("BurnColor",color)=(2.5,1,1,1)
    }
    SubShader
    {
        Tags { 
        "RenderType"="TransparentCutout" 
        "RenderPipeline" = "UniversalRenderPipline" 
        "Queue"="AlphaTest" }
        //"IgnoreProjector"="True" 
        //https://blog.csdn.net/niuge8905/article/details/77427895

        HLSLINCLUDE

        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"

        CBUFFER_START(UnityPerMaterial)
        float4 _MainTex_ST;
        float _Cutoff;
        half4 _BaseColor;
        real4 _BurnColor;
        CBUFFER_END

        TEXTURE2D(_MainTex);
        SAMPLER(sampler_MainTex);

        struct a2v
        {
            float4 positionOS : POSITION;
            float4 normalOS : NORMAL;
            float2 texcoord : TEXCOORD;
        };

        struct v2f
        {
            float4 positionCS:SV_POSITION;
            float2 texcoord:TEXCOORD;
        };

        ENDHLSL

        Pass
        {
        Tags{"LightMode" = "UniversalForward"}
        //Blend SrcAlpha OneMinusSrcAlpha 
        //https://blog.csdn.net/ecidevilin/article/details/52864349
        //ZWrite Off
        //https://codertw.com/%E7%A8%8B%E5%BC%8F%E8%AA%9E%E8%A8%80/415492/

            HLSLPROGRAM
            #pragma vertex vert
            #pragma fragment frag

            v2f vert(a2v i)
            {
                v2f o;
                o.positionCS = TransformObjectToHClip(i.positionOS.xyz);
                o.texcoord = TRANSFORM_TEX(i.texcoord,_MainTex); 
                return o;
            }

            real4 frag(v2f i) : SV_Target
            {
                half4 tex=SAMPLE_TEXTURE2D(_MainTex,sampler_MainTex,i.texcoord)*_BaseColor;
                clip(step(_Cutoff,tex.r)-0.01);
                //這裡減去0.01是因為clip對0是還會保留 所以要減去0.01讓本身為0的部分被拋棄
                tex = lerp(tex,_BurnColor,step(tex.r,saturate(_Cutoff+0.1)));
                ////lerp一下灼燒色和原色 +0.1是控制灼燒區域範圍
                return tex;
            }
            ENDHLSL
        }
    }
}
