Shader "Custom/LearnHLSLShader1"
{
    // 首先給個中文百科https://www.twblogs.net/a/5c9663aebd9eee4a0d0912b0
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
        // 更正 CBUFFER_START和CBUFFER_END,對於變量是單個材質獨有的時候建議放在這裡面，以提高性能
        // CBUFFER(常量緩衝區)的空間較小，不適合存放紋理貼圖這種大量數據的數據類型，適合存放float，half之類的不佔空間的數據
        // 更詳細的說明 https://blogs.unity3d.com/2019/02/28/srp-batcher-speed-up-your-rendering/
        CBUFFER_START(UnityPerMaterial)
        //只要與圖片相關都要加上_ST 並且是float4，看Material的inspector就知道為甚麼是float4了
        float4 _BaseMap_ST;
        half4 _BaseColor;
        CBUFFER_END

        ENDHLSL

        Pass
        {
            Tags { "LightMode"="UniversalForward" }
            //"LightMode" ="ShadowCaster" ,"UniversalForward" ,"ForwardBase" ,"ForwardAdd"
            //ShadowCaster = 目前已知能接受點光源
            //UniversalForward = 只會有一個方向的光源(待確認)
            //ForwardBase = 用於正向渲染中，該通過會計算、最重要的平行光、光逐目標/SH光源圖和Lightmaps光源貼圖
            //ForwardAdd = 用於前向渲染。該Pass會計算一個單獨的像素光源，每個Pass對應一個光源
            //https://blog.csdn.net/qq_39574690/article/details/99766585 or https://codeleading.com/article/99201426472/

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
                float4 normalOS:NORMAL;
                float2 uv           : TEXCOORD0;
                
            };
            // 上面struct最後面都要加上;

            struct Varyings
            {
                float2 uv           : TEXCOORD0;
                //WS（世界空間），VS（視口空間），CS（除除空間），NDC（NDC空間）
                float3 positionWS   : TEXCOORD1;
                // 這個結構必須包含SV_POSITION, HCS = Homogeneous Clipping Space
                float4 positionCS : SV_POSITION;
            };

            //貼圖的採用輸出函數採用DXD11 HLSL下的 SAMPLE_TEXTURE2D(textureName, samplerName, coord2)
            //具有三個變量，分別是TEXTURE2D (_MainTex)的變量和SAMPLER(sampler_MainTex)的變量和uv，用來代替原本DXD9的
            TEXTURE2D(_BaseMap);
            SAMPLER(sampler_BaseMap);

            Varyings vert(Attributes IN)
            {
                Varyings OUT;

                // GetVertexPositionInputs computes position in different spaces (ViewSpace, WorldSpace, Homogeneous Clip Space)
                VertexPositionInputs positionInputs = GetVertexPositionInputs(IN.positionOS.xyz);
                //TransformObjectToHClip，從object space 到homogenous space??，變換頂點位置
                //OUT.positionCS = TransformObjectToHClip(IN.positionOS.xyz);
                OUT.positionCS = positionInputs.positionCS;
                OUT.positionWS = positionInputs.positionWS;
                OUT.uv = TRANSFORM_TEX(IN.uv, _BaseMap);
                //X 將模型的UV和_BaseMap做運算
                //將模型偏移的Offset和Tiling進行計算，計算出實際用的定點uv。
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
