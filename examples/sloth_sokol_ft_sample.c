#include <stdio.h>

#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "./libs/sokol/sokol_app.h"
#include "./libs/sokol/sokol_gfx.h"
#include "./libs/sokol/sokol_glue.h"

#include "libs/freetype/freetype2/ft2build.h"
#include FT_FREETYPE_H

#define SLOTH_IMPLEMENTATION 1
#define SLOTH_FREETYPE_ATLAS 1
#define SLOTH_SOKOL_RENDERER 1
#include "../src/sloth.h"

Sloth_Ctx sloth = {0};
Sloth_Font_ID sloth_font = {0};
Sloth_Frame_Desc sloth_frame_desc = {0};

sg_pass_action default_pass = {
  .colors[0] = {
    .action = SG_ACTION_CLEAR,
    .value = { 0.2f, 0.2f, 0.2f, 1 },
  }
};

void init_cb() {
  sg_setup(&(sg_desc){ .context = sapp_sgcontext() });
  
  sloth_sokol_init(&sloth);
  sloth_ft2_init(&sloth);
  sloth_ctx_init(&sloth);
  
  // Load Fonts We Need
  sloth_font = sloth_font_load(&sloth, "../data/font.ttf", 14);
  sloth_font_register_family(&sloth, sloth_font, 0, 0);
}

void frame_cb() {
  sloth_sokol_frame_prepare(&sloth, &sloth_frame_desc);
  
  Sloth_Widget_Desc rd = {
    .layout = {
      .width = SLOTH_SIZE_PIXELS(sapp_widthf()),
      .height = SLOTH_SIZE_PIXELS(sapp_heightf()),
      .margin = {
        .left = SLOTH_SIZE_PIXELS(16),
        .top = SLOTH_SIZE_PIXELS(16),
      },
    },
  };
  Sloth_Widget_Result root = sloth_push_widget(&sloth, rd, "##root");
  {
    sloth_cmp_text_f(&sloth, 0, 0xFFFFFFFF, "Hi There!");
    if (sloth_cmp_button_f(&sloth, 0x000000FF, 0xFFFFFFFF, 0, "Test"))
    {
      printf("Button pressed\n");
    }
  }
  sloth_pop_widget_safe(&sloth, root);
  
  // advance the frame
  sloth_frame_advance(&sloth);
  
  if (sloth_frame_desc.mouse_button_l & Sloth_MouseState_IsDown) {
    sloth_frame_desc.mouse_button_l |= Sloth_MouseState_WasDown;
  } else if (sloth_frame_desc.mouse_button_l == Sloth_MouseState_WasDown) {
    sloth_frame_desc.mouse_button_l = 0;
  }
  
  // Render everything to the screen
  sg_begin_default_pass(&default_pass, sapp_width(), sapp_height());
  for (Sloth_U32 i = 0; i < sloth.glyph_atlases_cap; i++)
  {
    sloth_render_sokol(&sloth, i);
  }
  sg_end_pass();
  sg_commit();
}

void event_cb(const sapp_event* event) {
  sloth_sokol_event(&sloth_frame_desc, event);
}

sapp_desc sokol_main(int argc, char* argv[]) {
  return (sapp_desc) {
    .width = 640,
    .height = 480,
    .init_cb = init_cb,
    .frame_cb = frame_cb,
    .event_cb = event_cb,
    .high_dpi = true,
  };
}