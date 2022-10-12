#pragma once
/*
    #version:1# (machine generated, don't edit!)

    Generated by sokol-shdc (https://github.com/floooh/sokol-tools)

    Cmdline: sokol-shdc --input lib/sloth/sloth_sokol_shader.glsl --output lib/sloth/sloth_sokol_shader.glsl.h --slang glsl330:hlsl5:metal_macos

    Overview:

        Shader program 'sloth_viz':
            Get shader desc: sloth_viz_shader_desc(sg_query_backend());
            Vertex shader: sloth_viz_vs
                Attribute slots:
                    ATTR_sloth_viz_vs_position = 0
                    ATTR_sloth_viz_vs_uv = 1
                    ATTR_sloth_viz_vs_color = 2
                Uniform block 'sloth_viz_vs_params':
                    C struct: sloth_viz_vs_params_t
                    Bind slot: SLOT_sloth_viz_vs_params = 0
            Fragment shader: sloth_viz_fs
                Image 'tex':
                    Type: SG_IMAGETYPE_2D
                    Component Type: SG_SAMPLERTYPE_FLOAT
                    Bind slot: SLOT_tex = 0


    Shader descriptor structs:

        sg_shader sloth_viz = sg_make_shader(sloth_viz_shader_desc(sg_query_backend()));

    Vertex attribute locations for vertex shader 'sloth_viz_vs':

        sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
            .layout = {
                .attrs = {
                    [ATTR_sloth_viz_vs_position] = { ... },
                    [ATTR_sloth_viz_vs_uv] = { ... },
                    [ATTR_sloth_viz_vs_color] = { ... },
                },
            },
            ...});

    Image bind slots, use as index in sg_bindings.vs_images[] or .fs_images[]

        SLOT_tex = 0;

    Bind slot and C-struct for uniform block 'sloth_viz_vs_params':

        sloth_viz_vs_params_t sloth_viz_vs_params = {
            .mvp = ...;
        };
        sg_apply_uniforms(SG_SHADERSTAGE_[VS|FS], SLOT_sloth_viz_vs_params, &SG_RANGE(sloth_viz_vs_params));

*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stddef.h>
#if !defined(SOKOL_SHDC_ALIGN)
#if defined(_MSC_VER)
#define SOKOL_SHDC_ALIGN(a) __declspec(align(a))
#else
#define SOKOL_SHDC_ALIGN(a) __attribute__((aligned(a)))
#endif
#endif
#define ATTR_sloth_viz_vs_position (0)
#define ATTR_sloth_viz_vs_uv (1)
#define ATTR_sloth_viz_vs_color (2)
#define SLOT_tex (0)
#define SLOT_sloth_viz_vs_params (0)
#pragma pack(push,1)
SOKOL_SHDC_ALIGN(16) typedef struct sloth_viz_vs_params_t {
  Sloth_R32 mvp[4][4];
} sloth_viz_vs_params_t;
#pragma pack(pop)
/*
    #version 330
    
    uniform vec4 sloth_viz_vs_params[4];
    layout(location = 0) in vec3 position;
    out vec2 o_uv;
    layout(location = 1) in vec2 uv;
    out vec4 o_color;
    layout(location = 2) in vec4 color;
    
    void main()
    {
        gl_Position = mat4(sloth_viz_vs_params[0], sloth_viz_vs_params[1], sloth_viz_vs_params[2], sloth_viz_vs_params[3]) * vec4(position, 1.0);
        o_uv = uv;
        o_color = color;
    }
    
*/
static const char sloth_viz_vs_source_glsl330[389] = {
  0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
  0x69,0x66,0x6f,0x72,0x6d,0x20,0x76,0x65,0x63,0x34,0x20,0x73,0x6c,0x6f,0x74,0x68,
  0x5f,0x76,0x69,0x7a,0x5f,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x34,
  0x5d,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,
  0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x33,0x20,
  0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,0x65,
  0x63,0x32,0x20,0x6f,0x5f,0x75,0x76,0x3b,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,
  0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x31,0x29,0x20,0x69,0x6e,
  0x20,0x76,0x65,0x63,0x32,0x20,0x75,0x76,0x3b,0x0a,0x6f,0x75,0x74,0x20,0x76,0x65,
  0x63,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x6c,0x61,0x79,0x6f,
  0x75,0x74,0x28,0x6c,0x6f,0x63,0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x32,0x29,
  0x20,0x69,0x6e,0x20,0x76,0x65,0x63,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,
  0x0a,0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,
  0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3d,
  0x20,0x6d,0x61,0x74,0x34,0x28,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,
  0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x30,0x5d,0x2c,0x20,0x73,0x6c,
  0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,
  0x73,0x5b,0x31,0x5d,0x2c,0x20,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,
  0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x5b,0x32,0x5d,0x2c,0x20,0x73,0x6c,
  0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,
  0x73,0x5b,0x33,0x5d,0x29,0x20,0x2a,0x20,0x76,0x65,0x63,0x34,0x28,0x70,0x6f,0x73,
  0x69,0x74,0x69,0x6f,0x6e,0x2c,0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x20,0x20,0x20,
  0x20,0x6f,0x5f,0x75,0x76,0x20,0x3d,0x20,0x75,0x76,0x3b,0x0a,0x20,0x20,0x20,0x20,
  0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,
  0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #version 330
    
    uniform sampler2D tex;
    
    layout(location = 0) out vec4 frag_color;
    in vec2 o_uv;
    in vec4 o_color;
    
    void main()
    {
        frag_color = texture(tex, o_uv) * o_color;
    }
    
*/
static const char sloth_viz_fs_source_glsl330[177] = {
  0x23,0x76,0x65,0x72,0x73,0x69,0x6f,0x6e,0x20,0x33,0x33,0x30,0x0a,0x0a,0x75,0x6e,
  0x69,0x66,0x6f,0x72,0x6d,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x32,0x44,0x20,
  0x74,0x65,0x78,0x3b,0x0a,0x0a,0x6c,0x61,0x79,0x6f,0x75,0x74,0x28,0x6c,0x6f,0x63,
  0x61,0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x30,0x29,0x20,0x6f,0x75,0x74,0x20,0x76,
  0x65,0x63,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,
  0x69,0x6e,0x20,0x76,0x65,0x63,0x32,0x20,0x6f,0x5f,0x75,0x76,0x3b,0x0a,0x69,0x6e,
  0x20,0x76,0x65,0x63,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,
  0x76,0x6f,0x69,0x64,0x20,0x6d,0x61,0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x20,0x20,
  0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x74,
  0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x74,0x65,0x78,0x2c,0x20,0x6f,0x5f,0x75,0x76,
  0x29,0x20,0x2a,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,
  0x00,
};
/*
    cbuffer sloth_viz_vs_params : register(b0)
    {
        row_major float4x4 _21_mvp : packoffset(c0);
    };
    
    
    static float4 gl_Position;
    static float3 position;
    static float2 o_uv;
    static float2 uv;
    static float4 o_color;
    static float4 color;
    
    struct SPIRV_Cross_Input
    {
        float3 position : TEXCOORD0;
        float2 uv : TEXCOORD1;
        float4 color : TEXCOORD2;
    };
    
    struct SPIRV_Cross_Output
    {
        float2 o_uv : TEXCOORD0;
        float4 o_color : TEXCOORD1;
        float4 gl_Position : SV_Position;
    };
    
    #line 23 "lib/sloth/sloth_sokol_shader.glsl"
    void vert_main()
    {
    #line 23 "lib/sloth/sloth_sokol_shader.glsl"
        gl_Position = mul(float4(position, 1.0f), _21_mvp);
    #line 24 "lib/sloth/sloth_sokol_shader.glsl"
        o_uv = uv;
    #line 25 "lib/sloth/sloth_sokol_shader.glsl"
        o_color = color;
    }
    
    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        position = stage_input.position;
        uv = stage_input.uv;
        color = stage_input.color;
        vert_main();
        SPIRV_Cross_Output stage_output;
        stage_output.gl_Position = gl_Position;
        stage_output.o_uv = o_uv;
        stage_output.o_color = o_color;
        return stage_output;
    }
*/
static const char sloth_viz_vs_source_hlsl5[1121] = {
  0x63,0x62,0x75,0x66,0x66,0x65,0x72,0x20,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x76,0x69,
  0x7a,0x5f,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,0x6d,0x73,0x20,0x3a,0x20,0x72,0x65,
  0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x62,0x30,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,
  0x20,0x72,0x6f,0x77,0x5f,0x6d,0x61,0x6a,0x6f,0x72,0x20,0x66,0x6c,0x6f,0x61,0x74,
  0x34,0x78,0x34,0x20,0x5f,0x32,0x31,0x5f,0x6d,0x76,0x70,0x20,0x3a,0x20,0x70,0x61,
  0x63,0x6b,0x6f,0x66,0x66,0x73,0x65,0x74,0x28,0x63,0x30,0x29,0x3b,0x0a,0x7d,0x3b,
  0x0a,0x0a,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
  0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x73,0x74,
  0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,0x6f,0x73,0x69,
  0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,
  0x61,0x74,0x32,0x20,0x6f,0x5f,0x75,0x76,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,
  0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,0x76,0x3b,0x0a,0x73,0x74,0x61,0x74,
  0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,
  0x72,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
  0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,
  0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,
  0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,
  0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,
  0x52,0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
  0x75,0x76,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,
  0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,0x6f,0x6c,0x6f,0x72,
  0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x32,0x3b,0x0a,0x7d,0x3b,
  0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,
  0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,
  0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x6f,0x5f,0x75,0x76,0x20,0x3a,0x20,
  0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,
  0x6c,0x6f,0x61,0x74,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,
  0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,
  0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,
  0x6e,0x20,0x3a,0x20,0x53,0x56,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,
  0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x32,0x33,0x20,0x22,0x6c,
  0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,
  0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,
  0x22,0x0a,0x76,0x6f,0x69,0x64,0x20,0x76,0x65,0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,
  0x28,0x29,0x0a,0x7b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x32,0x33,0x20,0x22,0x6c,
  0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,
  0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,
  0x22,0x0a,0x20,0x20,0x20,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,
  0x6e,0x20,0x3d,0x20,0x6d,0x75,0x6c,0x28,0x66,0x6c,0x6f,0x61,0x74,0x34,0x28,0x70,
  0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x2c,0x20,0x31,0x2e,0x30,0x66,0x29,0x2c,0x20,
  0x5f,0x32,0x31,0x5f,0x6d,0x76,0x70,0x29,0x3b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,
  0x32,0x34,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,
  0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,
  0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,0x5f,0x75,0x76,0x20,
  0x3d,0x20,0x75,0x76,0x3b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x32,0x35,0x20,0x22,
  0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,
  0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,
  0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,
  0x20,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,0x0a,0x0a,0x53,0x50,0x49,0x52,0x56,
  0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x6d,0x61,
  0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,
  0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,
  0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,
  0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x70,
  0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x75,0x76,0x20,
  0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x75,0x76,
  0x3b,0x0a,0x20,0x20,0x20,0x20,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x73,0x74,
  0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x2e,0x63,0x6f,0x6c,0x6f,0x72,0x3b,
  0x0a,0x20,0x20,0x20,0x20,0x76,0x65,0x72,0x74,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,
  0x3b,0x0a,0x20,0x20,0x20,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,
  0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,
  0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,
  0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,
  0x69,0x6f,0x6e,0x20,0x3d,0x20,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,
  0x6e,0x3b,0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,
  0x70,0x75,0x74,0x2e,0x6f,0x5f,0x75,0x76,0x20,0x3d,0x20,0x6f,0x5f,0x75,0x76,0x3b,
  0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,
  0x74,0x2e,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x6f,0x5f,0x63,0x6f,
  0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,0x6e,0x20,
  0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,0x0a,0x7d,0x0a,
  0x00,
};
/*
    Texture2D<float4> tex : register(t0);
    SamplerState _tex_sampler : register(s0);
    
    static float4 frag_color;
    static float2 o_uv;
    static float4 o_color;
    
    struct SPIRV_Cross_Input
    {
        float2 o_uv : TEXCOORD0;
        float4 o_color : TEXCOORD1;
    };
    
    struct SPIRV_Cross_Output
    {
        float4 frag_color : SV_Target0;
    };
    
    #line 15 "lib/sloth/sloth_sokol_shader.glsl"
    void frag_main()
    {
    #line 15 "lib/sloth/sloth_sokol_shader.glsl"
        frag_color = tex.Sample(_tex_sampler, o_uv) * o_color;
    }
    
    SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
    {
        o_uv = stage_input.o_uv;
        o_color = stage_input.o_color;
        frag_main();
        SPIRV_Cross_Output stage_output;
        stage_output.frag_color = frag_color;
        return stage_output;
    }
*/
static const char sloth_viz_fs_source_hlsl5[727] = {
  0x54,0x65,0x78,0x74,0x75,0x72,0x65,0x32,0x44,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x34,
  0x3e,0x20,0x74,0x65,0x78,0x20,0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,
  0x28,0x74,0x30,0x29,0x3b,0x0a,0x53,0x61,0x6d,0x70,0x6c,0x65,0x72,0x53,0x74,0x61,
  0x74,0x65,0x20,0x5f,0x74,0x65,0x78,0x5f,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x20,
  0x3a,0x20,0x72,0x65,0x67,0x69,0x73,0x74,0x65,0x72,0x28,0x73,0x30,0x29,0x3b,0x0a,
  0x0a,0x73,0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,
  0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x73,0x74,0x61,0x74,0x69,
  0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x6f,0x5f,0x75,0x76,0x3b,0x0a,0x73,
  0x74,0x61,0x74,0x69,0x63,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x6f,0x5f,0x63,
  0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,
  0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x0a,
  0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x6f,0x5f,0x75,
  0x76,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x30,0x3b,0x0a,0x20,
  0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,
  0x72,0x20,0x3a,0x20,0x54,0x45,0x58,0x43,0x4f,0x4f,0x52,0x44,0x31,0x3b,0x0a,0x7d,
  0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x53,0x50,0x49,0x52,0x56,0x5f,
  0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,0x75,0x74,0x0a,0x7b,0x0a,0x20,
  0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,
  0x6f,0x6c,0x6f,0x72,0x20,0x3a,0x20,0x53,0x56,0x5f,0x54,0x61,0x72,0x67,0x65,0x74,
  0x30,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x31,0x35,0x20,
  0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,
  0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,
  0x73,0x6c,0x22,0x0a,0x76,0x6f,0x69,0x64,0x20,0x66,0x72,0x61,0x67,0x5f,0x6d,0x61,
  0x69,0x6e,0x28,0x29,0x0a,0x7b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x31,0x35,0x20,
  0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,
  0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,
  0x73,0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,
  0x6f,0x72,0x20,0x3d,0x20,0x74,0x65,0x78,0x2e,0x53,0x61,0x6d,0x70,0x6c,0x65,0x28,
  0x5f,0x74,0x65,0x78,0x5f,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x2c,0x20,0x6f,0x5f,
  0x75,0x76,0x29,0x20,0x2a,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x7d,
  0x0a,0x0a,0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,
  0x74,0x70,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x28,0x53,0x50,0x49,0x52,0x56,0x5f,
  0x43,0x72,0x6f,0x73,0x73,0x5f,0x49,0x6e,0x70,0x75,0x74,0x20,0x73,0x74,0x61,0x67,
  0x65,0x5f,0x69,0x6e,0x70,0x75,0x74,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x6f,
  0x5f,0x75,0x76,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,
  0x74,0x2e,0x6f,0x5f,0x75,0x76,0x3b,0x0a,0x20,0x20,0x20,0x20,0x6f,0x5f,0x63,0x6f,
  0x6c,0x6f,0x72,0x20,0x3d,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x70,0x75,
  0x74,0x2e,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,
  0x72,0x61,0x67,0x5f,0x6d,0x61,0x69,0x6e,0x28,0x29,0x3b,0x0a,0x20,0x20,0x20,0x20,
  0x53,0x50,0x49,0x52,0x56,0x5f,0x43,0x72,0x6f,0x73,0x73,0x5f,0x4f,0x75,0x74,0x70,
  0x75,0x74,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,0x74,0x3b,
  0x0a,0x20,0x20,0x20,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,0x75,
  0x74,0x2e,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x66,
  0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,
  0x65,0x74,0x75,0x72,0x6e,0x20,0x73,0x74,0x61,0x67,0x65,0x5f,0x6f,0x75,0x74,0x70,
  0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct sloth_viz_vs_params
    {
        float4x4 mvp;
    };
    
    struct main0_out
    {
        float2 o_uv [[user(locn0)]];
        float4 o_color [[user(locn1)]];
        float4 gl_Position [[position]];
    };
    
    struct main0_in
    {
        float3 position [[attribute(0)]];
        float2 uv [[attribute(1)]];
        float4 color [[attribute(2)]];
    };
    
    #line 23 "lib/sloth/sloth_sokol_shader.glsl"
    vertex main0_out main0(main0_in in [[stage_in]], constant sloth_viz_vs_params& _21 [[buffer(0)]])
    {
        main0_out out = {};
    #line 23 "lib/sloth/sloth_sokol_shader.glsl"
        out.gl_Position = _21.mvp * float4(in.position, 1.0);
    #line 24 "lib/sloth/sloth_sokol_shader.glsl"
        out.o_uv = in.uv;
    #line 25 "lib/sloth/sloth_sokol_shader.glsl"
        out.o_color = in.color;
        return out;
    }
    
*/
static const char sloth_viz_vs_source_metal_macos[811] = {
  0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
  0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
  0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
  0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
  0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x73,
  0x6c,0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,0x76,0x73,0x5f,0x70,0x61,0x72,0x61,
  0x6d,0x73,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x78,
  0x34,0x20,0x6d,0x76,0x70,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,
  0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,
  0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x6f,0x5f,0x75,0x76,0x20,0x5b,0x5b,
  0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x20,
  0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,
  0x72,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,0x31,0x29,0x5d,
  0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x67,0x6c,
  0x5f,0x50,0x6f,0x73,0x69,0x74,0x69,0x6f,0x6e,0x20,0x5b,0x5b,0x70,0x6f,0x73,0x69,
  0x74,0x69,0x6f,0x6e,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,
  0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,
  0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x33,0x20,0x70,0x6f,0x73,0x69,0x74,0x69,0x6f,
  0x6e,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x28,0x30,0x29,
  0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,0x75,
  0x76,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,0x28,0x31,0x29,
  0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x20,0x63,
  0x6f,0x6c,0x6f,0x72,0x20,0x5b,0x5b,0x61,0x74,0x74,0x72,0x69,0x62,0x75,0x74,0x65,
  0x28,0x32,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,0x69,0x6e,0x65,
  0x20,0x32,0x33,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,
  0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,
  0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x76,0x65,0x72,0x74,0x65,0x78,0x20,0x6d,
  0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x28,0x6d,
  0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,0x5b,0x5b,0x73,0x74,0x61,
  0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x2c,0x20,0x63,0x6f,0x6e,0x73,0x74,0x61,0x6e,
  0x74,0x20,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x76,0x69,0x7a,0x5f,0x76,0x73,0x5f,0x70,
  0x61,0x72,0x61,0x6d,0x73,0x26,0x20,0x5f,0x32,0x31,0x20,0x5b,0x5b,0x62,0x75,0x66,
  0x66,0x65,0x72,0x28,0x30,0x29,0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,
  0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,0x3d,0x20,
  0x7b,0x7d,0x3b,0x0a,0x23,0x6c,0x69,0x6e,0x65,0x20,0x32,0x33,0x20,0x22,0x6c,0x69,
  0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,
  0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,
  0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,0x67,0x6c,0x5f,0x50,0x6f,0x73,0x69,
  0x74,0x69,0x6f,0x6e,0x20,0x3d,0x20,0x5f,0x32,0x31,0x2e,0x6d,0x76,0x70,0x20,0x2a,
  0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,0x28,0x69,0x6e,0x2e,0x70,0x6f,0x73,0x69,0x74,
  0x69,0x6f,0x6e,0x2c,0x20,0x31,0x2e,0x30,0x29,0x3b,0x0a,0x23,0x6c,0x69,0x6e,0x65,
  0x20,0x32,0x34,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x2f,0x73,
  0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,0x61,0x64,0x65,
  0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,0x75,0x74,0x2e,
  0x6f,0x5f,0x75,0x76,0x20,0x3d,0x20,0x69,0x6e,0x2e,0x75,0x76,0x3b,0x0a,0x23,0x6c,
  0x69,0x6e,0x65,0x20,0x32,0x35,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,
  0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,
  0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,
  0x75,0x74,0x2e,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,0x69,0x6e,0x2e,
  0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,0x72,0x65,0x74,0x75,0x72,
  0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
};
/*
    #include <metal_stdlib>
    #include <simd/simd.h>
    
    using namespace metal;
    
    struct main0_out
    {
        float4 frag_color [[color(0)]];
    };
    
    struct main0_in
    {
        float2 o_uv [[user(locn0)]];
        float4 o_color [[user(locn1)]];
    };
    
    #line 15 "lib/sloth/sloth_sokol_shader.glsl"
    fragment main0_out main0(main0_in in [[stage_in]], texture2d<float> tex [[texture(0)]], sampler texSmplr [[sampler(0)]])
    {
        main0_out out = {};
    #line 15 "lib/sloth/sloth_sokol_shader.glsl"
        out.frag_color = tex.sample(texSmplr, in.o_uv) * in.o_color;
        return out;
    }
    
*/
static const char sloth_viz_fs_source_metal_macos[544] = {
  0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,0x20,0x3c,0x6d,0x65,0x74,0x61,0x6c,0x5f,
  0x73,0x74,0x64,0x6c,0x69,0x62,0x3e,0x0a,0x23,0x69,0x6e,0x63,0x6c,0x75,0x64,0x65,
  0x20,0x3c,0x73,0x69,0x6d,0x64,0x2f,0x73,0x69,0x6d,0x64,0x2e,0x68,0x3e,0x0a,0x0a,
  0x75,0x73,0x69,0x6e,0x67,0x20,0x6e,0x61,0x6d,0x65,0x73,0x70,0x61,0x63,0x65,0x20,
  0x6d,0x65,0x74,0x61,0x6c,0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,
  0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,
  0x6c,0x6f,0x61,0x74,0x34,0x20,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,
  0x20,0x5b,0x5b,0x63,0x6f,0x6c,0x6f,0x72,0x28,0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,
  0x3b,0x0a,0x0a,0x73,0x74,0x72,0x75,0x63,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,
  0x69,0x6e,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x32,0x20,
  0x6f,0x5f,0x75,0x76,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,0x6c,0x6f,0x63,0x6e,
  0x30,0x29,0x5d,0x5d,0x3b,0x0a,0x20,0x20,0x20,0x20,0x66,0x6c,0x6f,0x61,0x74,0x34,
  0x20,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x5b,0x5b,0x75,0x73,0x65,0x72,0x28,
  0x6c,0x6f,0x63,0x6e,0x31,0x29,0x5d,0x5d,0x3b,0x0a,0x7d,0x3b,0x0a,0x0a,0x23,0x6c,
  0x69,0x6e,0x65,0x20,0x31,0x35,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,
  0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,
  0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x66,0x72,0x61,0x67,0x6d,
  0x65,0x6e,0x74,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x6f,0x75,0x74,0x20,0x6d,0x61,
  0x69,0x6e,0x30,0x28,0x6d,0x61,0x69,0x6e,0x30,0x5f,0x69,0x6e,0x20,0x69,0x6e,0x20,
  0x5b,0x5b,0x73,0x74,0x61,0x67,0x65,0x5f,0x69,0x6e,0x5d,0x5d,0x2c,0x20,0x74,0x65,
  0x78,0x74,0x75,0x72,0x65,0x32,0x64,0x3c,0x66,0x6c,0x6f,0x61,0x74,0x3e,0x20,0x74,
  0x65,0x78,0x20,0x5b,0x5b,0x74,0x65,0x78,0x74,0x75,0x72,0x65,0x28,0x30,0x29,0x5d,
  0x5d,0x2c,0x20,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x20,0x74,0x65,0x78,0x53,0x6d,
  0x70,0x6c,0x72,0x20,0x5b,0x5b,0x73,0x61,0x6d,0x70,0x6c,0x65,0x72,0x28,0x30,0x29,
  0x5d,0x5d,0x29,0x0a,0x7b,0x0a,0x20,0x20,0x20,0x20,0x6d,0x61,0x69,0x6e,0x30,0x5f,
  0x6f,0x75,0x74,0x20,0x6f,0x75,0x74,0x20,0x3d,0x20,0x7b,0x7d,0x3b,0x0a,0x23,0x6c,
  0x69,0x6e,0x65,0x20,0x31,0x35,0x20,0x22,0x6c,0x69,0x62,0x2f,0x73,0x6c,0x6f,0x74,
  0x68,0x2f,0x73,0x6c,0x6f,0x74,0x68,0x5f,0x73,0x6f,0x6b,0x6f,0x6c,0x5f,0x73,0x68,
  0x61,0x64,0x65,0x72,0x2e,0x67,0x6c,0x73,0x6c,0x22,0x0a,0x20,0x20,0x20,0x20,0x6f,
  0x75,0x74,0x2e,0x66,0x72,0x61,0x67,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x20,0x3d,0x20,
  0x74,0x65,0x78,0x2e,0x73,0x61,0x6d,0x70,0x6c,0x65,0x28,0x74,0x65,0x78,0x53,0x6d,
  0x70,0x6c,0x72,0x2c,0x20,0x69,0x6e,0x2e,0x6f,0x5f,0x75,0x76,0x29,0x20,0x2a,0x20,
  0x69,0x6e,0x2e,0x6f,0x5f,0x63,0x6f,0x6c,0x6f,0x72,0x3b,0x0a,0x20,0x20,0x20,0x20,
  0x72,0x65,0x74,0x75,0x72,0x6e,0x20,0x6f,0x75,0x74,0x3b,0x0a,0x7d,0x0a,0x0a,0x00,
  
};
#if !defined(SOKOL_GFX_INCLUDED)
#error "Please include sokol_gfx.h before sloth_sokol_shader.glsl.h"
#endif
static inline const sg_shader_desc* sloth_viz_shader_desc(sg_backend backend) {
  if (backend == SG_BACKEND_GLCORE33) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].name = "position";
      desc.attrs[1].name = "uv";
      desc.attrs[2].name = "color";
      desc.vs.source = sloth_viz_vs_source_glsl330;
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.vs.uniform_blocks[0].uniforms[0].name = "sloth_viz_vs_params";
      desc.vs.uniform_blocks[0].uniforms[0].type = SG_UNIFORMTYPE_FLOAT4;
      desc.vs.uniform_blocks[0].uniforms[0].array_count = 4;
      desc.fs.source = sloth_viz_fs_source_glsl330;
      desc.fs.entry = "main";
      desc.fs.images[0].name = "tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "sloth_viz_shader";
    }
    return &desc;
  }
  if (backend == SG_BACKEND_D3D11) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.attrs[0].sem_name = "TEXCOORD";
      desc.attrs[0].sem_index = 0;
      desc.attrs[1].sem_name = "TEXCOORD";
      desc.attrs[1].sem_index = 1;
      desc.attrs[2].sem_name = "TEXCOORD";
      desc.attrs[2].sem_index = 2;
      desc.vs.source = sloth_viz_vs_source_hlsl5;
      desc.vs.d3d11_target = "vs_5_0";
      desc.vs.entry = "main";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.fs.source = sloth_viz_fs_source_hlsl5;
      desc.fs.d3d11_target = "ps_5_0";
      desc.fs.entry = "main";
      desc.fs.images[0].name = "tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "sloth_viz_shader";
    }
    return &desc;
  }
  if (backend == SG_BACKEND_METAL_MACOS) {
    static sg_shader_desc desc;
    static bool valid;
    if (!valid) {
      valid = true;
      desc.vs.source = sloth_viz_vs_source_metal_macos;
      desc.vs.entry = "main0";
      desc.vs.uniform_blocks[0].size = 64;
      desc.vs.uniform_blocks[0].layout = SG_UNIFORMLAYOUT_STD140;
      desc.fs.source = sloth_viz_fs_source_metal_macos;
      desc.fs.entry = "main0";
      desc.fs.images[0].name = "tex";
      desc.fs.images[0].image_type = SG_IMAGETYPE_2D;
      desc.fs.images[0].sampler_type = SG_SAMPLERTYPE_FLOAT;
      desc.label = "sloth_viz_shader";
    }
    return &desc;
  }
  return 0;
}