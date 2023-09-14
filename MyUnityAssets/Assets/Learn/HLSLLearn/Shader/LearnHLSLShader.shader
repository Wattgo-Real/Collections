Shader "Custom/LearnHLSLShader"
{
    // URP中的首選代碼是HLSL   也有支援一些CG語言，另外也可以在Shader添加CGPROGRAM/ENDCGPROGRAM塊，
    // unity會自動include內置渲染管線build-in shader 但如果再include SRP shader，可能會出錯，也無法支持SRP Batcher
    // https://www.zhihu.com/question/360021447

    // 如果要從內置渲染升級到URP的話 https://teodutra.com/unity/shaders/urp/graphics/2020/05/18/From-Built-in-to-URP/

    Properties
    {
        //[MainColor] 可有可無??，告訴為主要的顏色
        [MainColor] _BaseColor("Base Color", Color) = (1,1,1,1)
        [MainTexture] _BaseMap("BaseMap", 2D) = "white" {}
    }

    SubShader
    {
        Tags{"RenderType" = "Opaque" "RenderPipeline" = "UniversalPipeline"}
        
        //全Pass都會有在HLSLINCLUDE和ENDHLSL中的所有東西
        HLSLINCLUDE

        // 這個core.hlsl文件包含了常用的HLSL宏定義以及函數，也包括了對其他常用HLSL文件的引用 下面的TransformObjectToHClip()就是這個的
        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"

        // 如果下面沒有CBUFFER的話，Unity會不兼容
        CBUFFER_START(UnityPerMaterial)
        //_BaseMap要加上_ST 並且是float4，看Material的inspector就知道了
        float4 _BaseMap_ST;
        half4 _BaseColor;
        CBUFFER_END

        ENDHLSL

        Pass
        {
            Tags { "LightMode"="UniversalForward" }

            // 開始指令
            HLSLPROGRAM

            // 定義點和面的名字 更多 https://docs.unity3d.com/Manual/SL-PragmaDirectives.html
            #pragma vertex vert
            #pragma fragment frag

            // 接收陰影定義 陰影相關
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS_CASCADE
            #pragma multi_compile _ _SHADOWS_SOFT

            #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"

            struct Attributes
            {
                // 物體的空間 OS = onject space
                float4 positionOS : POSITION;
                // 法線位置
                //half3 normal : NORMAL;
                float2 uv           : TEXCOORD0;
                
            };
            // 上面struct最後面都要加上;

            struct Varyings
            {
                float2 uv           : TEXCOORD0;
                //WS（世界空間），VS（視口空間），CS（除除空間），NDC（NDC空間）
                float3 positionWS   : TEXCOORD1;
                // 這個結構必須包含SV_POSITION, HCS = Homogeneous Clipping Space
                float4 positionHCS : SV_POSITION;
            };

            TEXTURE2D(_BaseMap);
            SAMPLER(sampler_BaseMap);

            Varyings vert(Attributes IN)
            {
                Varyings OUT;

                // GetVertexPositionInputs computes position in different spaces (ViewSpace, WorldSpace, Homogeneous Clip Space)
                VertexPositionInputs positionInputs = GetVertexPositionInputs(IN.positionOS.xyz);
                //TransformObjectToHClip，從object space 到homogenous space??，變換頂點位置
                //OUT.positionHCS = TransformObjectToHClip(IN.positionOS.xyz);
                OUT.positionHCS = positionInputs.positionCS;
                OUT.positionWS = positionInputs.positionWS;
                //將模型的UV和_BaseMap做運算
                OUT.uv = TRANSFORM_TEX(IN.uv, _BaseMap);
                return OUT;
            }
            //以下為固定語意 fixed4被取消，作為替代的是half4
            half4 frag(Varyings IN):SV_Target
            {
                //將_BaseMap貼在模型的UV上並且顏色都乘_BaseColor
                //return SAMPLE_TEXTURE2D(_BaseMap, sampler_BaseMap, IN.uv) * _BaseColor;
                float4 shadowCoord = TransformWorldToShadowCoord(IN.positionWS);
                Light mainLight = GetMainLight(shadowCoord);
                half4 color = SAMPLE_TEXTURE2D(_BaseMap, sampler_BaseMap, IN.uv) * _BaseColor;
                color *= mainLight.shadowAttenuation;
                return color;
            }

            //停止指令
            ENDHLSL
        }
        UsePass "Universal Render Pipeline/Lit/ShadowCaster"
    }
}
