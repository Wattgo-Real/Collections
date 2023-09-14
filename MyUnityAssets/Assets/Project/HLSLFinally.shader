Shader "HLSL/shadowCaster"
{
    Properties
    {
        _MainTex("MainTex",2D)="White"{}
        _BaseColor("BaseColor",Color)=(1,1,1,1)
        _Gloss("Gloss",Range(10,300))=50
        _SpecularColor("SpecularColor",Color  )=(1,1,1,1)
        [KeywordEnum(ON,OFF)]_CUT("CUT",float)=1
        _Cutoff("cutoff",Range(0,1))=1
        [KeywordEnum(ON,OFF)]_ADD_LIGHT("AddLight",float)=1
    }

    SubShader
    {
        Tags{"RenderPipeline"="UniversalRenderPipeline"}

        HLSLINCLUDE

        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Core.hlsl"
        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Lighting.hlsl"
        #include "Packages/com.unity.render-pipelines.universal/ShaderLibrary/Shadows.hlsl"
        #pragma  shader_feature_local _CUT_ON 
        #pragma  shader_feature_local _ADD_LIGHT_ON 

        CBUFFER_START(UnityPerMaterial)
        float4 _MainTex_ST;
        half4 _BaseColor;
        float _Cutoff;
        float _Gloss;
        real4 _SpecularColor;
        CBUFFER_END

        TEXTURE2D(_MainTex);
        SAMPLER(sampler_MainTex);

        struct a2v
        {
            float4 positionOS:POSITION;
            float4 normalOS:NORMAL;
            float2 texcoord:TEXCOORD;
            UNITY_VERTEX_INPUT_INSTANCE_ID
        };

        struct v2f
        {
            float4 positionCS:SV_POSITION;
            float2 texcoord:TEXCOORD;
            #ifdef _MAIN_LIGHT_SHADOWS
            float4 shadowcoord:TEXCOORD1;
            #endif
            float3 WS_P:TEXCOORD2;
            float3 WS_N:TEXCOORD4; 
            float3 WS_V:TEXCOORD3;
            UNITY_VERTEX_INPUT_INSTANCE_ID
        };

        struct MeshProperties {
            float4x4 mat;
            float4 color;
        };

        StructuredBuffer<MeshProperties> _Properties;


        ENDHLSL

        pass
        {
        Tags{"LightMode"="UniversalForward" "RenderType"="TransparentCutout" "Queue"="AlphaTest"}

        Cull off

            HLSLPROGRAM
            #pragma vertex VERT
            #pragma fragment FRAG
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS
            #pragma multi_compile _ _MAIN_LIGHT_SHADOWS_CASCADE
            #pragma multi_compile _ _ADDITIONAL_LIGHTS_VERTEX _ADDITIONAL_LIGHTS
            #pragma multi_compile _ _ADDITIONAL_LIGHT_SHADOWS
            #pragma multi_compile _ _SHADOWS_SOFT

            v2f VERT(a2v i, uint id : SV_InstanceID)
            {
                v2f o;
                
                UNITY_SETUP_INSTANCE_ID(input);
                UNITY_TRANSFER_INSTANCE_ID(input, output);
                float4x4 mat = _Properties[id.x].mat;
                float4 OS = mul(mat,i.positionOS.xyzw);
                float4 nOS = mul(mat,float4(i.normalOS.xyz,0));

                o.positionCS=TransformWorldToHClip(OS.xyz);
                o.texcoord=TRANSFORM_TEX(i.texcoord,_MainTex);
                o.WS_P=TransformObjectToWorld(OS.xyz);

                #ifdef _MAIN_LIGHT_SHADOWS
                o.shadowcoord=TransformWorldToShadowCoord(o.WS_P);
                #endif

                o.WS_V=normalize(_WorldSpaceCameraPos-o.WS_P.xyz);
                o.WS_N=normalize(TransformObjectToWorldNormal(nOS.xyz));
                return o;
            }

            half4 FRAG(v2f i):SV_TARGET

            {
                half4 tex=SAMPLE_TEXTURE2D(_MainTex,sampler_MainTex,i.texcoord)*_BaseColor;

                #ifdef _CUT_ON
                clip(tex.a-_Cutoff);
                #endif

                float3 NormalWS=i.WS_N;
                float3 PositionWS=i.WS_P;
                float3 viewDir=i.WS_V;

                #ifdef _MAIN_LIGHT_SHADOWS
                Light mylight=GetMainLight(i.shadowcoord);
                #else
                Light mylight=GetMainLight();
                #endif

                half4 MainColor=(dot(normalize(mylight.direction.xyz),NormalWS)*0.5+0.5)*half4(mylight.color,1);
                MainColor+= pow(max(dot(normalize(viewDir +normalize(mylight.direction.xyz)),NormalWS),0),_Gloss);
                MainColor*=mylight.shadowAttenuation;

                half4 AddColor=half4(0,0,0,1);

                #ifdef _ADD_LIGHT_ON
                int addlightCount=GetAdditionalLightsCount();
                for(int t=0;t<addlightCount;t++)
                {
                Light addlight=GetAdditionalLight(t,PositionWS);
                //额外灯光就只计算一下半兰伯特模型（高光没计算，性能考虑）
                AddColor+=(dot(normalize(addlight.direction),NormalWS)*0.5+0.5)*half4(addlight.color,1)*addlight.shadowAttenuation*addlight.distanceAttenuation;
                }
                #endif

                return tex*(MainColor+AddColor);
            }
            ENDHLSL
        }
        // UsePass "Universal Render Pipeline/Lit/ShadowCaster"

        pass
        {
        //该pass只把主灯光空间的深度图写到了shadowmap里  addlight灯光空间目前没有写进去 导致模型无法投射addlight的阴影 但是整shader可以接受addlight的阴影
        //官方的
        Tags{"LightMode"="ShadowCaster"}

            HLSLPROGRAM

            #pragma vertex vertshadow
            #pragma fragment fragshadow

            v2f vertshadow(a2v i, uint id : SV_InstanceID)
            {
                v2f o;
                UNITY_SETUP_INSTANCE_ID(input);
                UNITY_TRANSFER_INSTANCE_ID(input, output);
                float4x4 mat = _Properties[id.x].mat;
                float4 OS = mul(mat,i.positionOS.xyzw);
                float4 nOS = mul(mat,float4(i.normalOS.xyz,0));

                o.texcoord=TRANSFORM_TEX(i.texcoord,_MainTex);
                float3 WSpos=TransformObjectToWorld(OS.xyz);
                Light MainLight=GetMainLight();
                float3 WSnor=TransformObjectToWorldNormal(nOS.xyz);
                o.positionCS=TransformWorldToHClip(ApplyShadowBias(WSpos,WSnor,MainLight.direction));

                #if UNITY_REVERSED_Z
                o.positionCS.z=min(o.positionCS.z,o.positionCS.w*UNITY_NEAR_CLIP_VALUE);
                #else
                o.positionCS.z=max(o.positionCS.z,o.positionCS.w*UNITY_NEAR_CLIP_VALUE);
                #endif


                //之後處裡
                o.WS_P = TransformObjectToWorld(OS.xyz);
                o.WS_V=normalize(_WorldSpaceCameraPos-o.WS_P.xyz);
                o.WS_N=normalize(TransformObjectToWorldNormal(nOS.xyz));


                return o;
            }

            half4 fragshadow(v2f i):SV_TARGET
            {
                #ifdef _CUT_ON 
                float alpha=SAMPLE_TEXTURE2D(_MainTex,sampler_MainTex,i.texcoord).a;
                clip(alpha-_Cutoff);
                #endif
                return 0;
            }
        ENDHLSL
        }
    }
}