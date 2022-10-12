
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_app.h"

#define STB_SPRINTF_IMPLEMENTATION 1
#include "../stb/stb_sprintf.h"

#define SLOTH_IMPLEMENTATION 1
#include "../sloth/sloth.h"

Sloth_Ctx sloth = {
  .per_frame_memory.name = "PFM",
  .scratch = "scratch",
};

typedef unsigned char u8;
typedef float r32;

void game_init (void* user_data) { }

void game_frame (void* user_data) 
{ 
  sloth_frame_prepare(&sloth);
  
  Sloth_Widget_Desc root_desc = {
    .layout = {
      .width = SLOTH_SIZE_PIXELS(800),
      .height = SLOTH_SIZE_PIXELS(600),
      .margin = {
        .right = SLOTH_SIZE_PIXELS(32),
        .top = SLOTH_SIZE_PIXELS(32),
        .left = SLOTH_SIZE_PIXELS(32),
        .bottom = SLOTH_SIZE_PIXELS(32),
      },
      .position = {
        .kind = Sloth_LayoutPosition_FixedOnScreen,
        .left = SLOTH_SIZE_PIXELS(32),
        .top = SLOTH_SIZE_PIXELS(48),
      },
    },
    .style = {
      .color_bg = 0xFFFFFF00,
      .color_text = 0xFF0000FF,
    },
  };
  
  Sloth_Widget_Desc ed = root_desc;
  ed.layout.width = SLOTH_SIZE_PIXELS(800);
  ed.layout.height = SLOTH_SIZE_PIXELS(250);
  ed.layout.position.kind = Sloth_LayoutPosition_ParentDecides;
  ed.style.color_text = 0x000000FF;
  ed.style.color_bg = 0x0000FF00;
  ed.layout.direction = Sloth_LayoutDirection_LeftToRight;
  
  sloth_push_widget(&sloth, root_desc, "testing some text how it works if its really long");
  {
    
    sloth_push_widget(&sloth, ed, "left to right"); 
    {
      
      char* long_str = "hobbits have been living and farming hte four farthings of the shire for many years, quite content to ignore and be ignored by the concerns of the big folk of the world.##%d";
      Sloth_Widget_Desc inner = root_desc;
      inner.layout.width = SLOTH_SIZE_PIXELS(150);
      inner.layout.height = SLOTH_SIZE_PIXELS(110);
      inner.layout.position.kind = Sloth_LayoutPosition_ParentDecides;
      inner.style.color_bg = 0x88880000;
      inner.style.text_style = Sloth_TextStyle_Align_Left;
      inner.input.flags = Sloth_WidgetInput_TextSelectable;
      sloth_push_widget_f(&sloth, inner, "tesmp##mouse id");
      sloth_pop_widget(&sloth);
      
      sloth.active_text_glyph_family = (Sloth_Font_ID){
        .value = 1,
        .weight_index = 0
      };
      inner.layout.height = SLOTH_SIZE_PIXELS(200);
      inner.style.color_bg = 0x00880000;
      inner.style.text_style = Sloth_TextStyle_Align_Center;
      inner.style.color_outline = 0xFFFFFFFF;
      inner.style.outline_thickness = 4;
      sloth_push_widget_f(&sloth, inner, long_str, 2);
      sloth_pop_widget(&sloth);
      
      sloth.active_text_glyph_family = (Sloth_Font_ID){
        .value = 0,
        .weight_index = 0
      };
      inner.style.color_bg = 0x88000000;
      inner.style.text_style = Sloth_TextStyle_Align_Right;
      inner.style.outline_thickness = 1;
      sloth_push_widget_f(&sloth, inner, long_str, 3);
      sloth_pop_widget(&sloth);
      
      inner.style.color_bg = 0x88000000;
      inner.style.outline_thickness = 0;
      sloth_push_widget_f(&sloth, inner, "Testing 1");
      sloth_pop_widget(&sloth);
      
      inner.style.color_bg = 0x88000000;
      sloth_push_widget_f(&sloth, inner, "Testing 2");
      sloth_pop_widget(&sloth);
    }
    sloth_pop_widget(&sloth);
  }
  
  ed.style.color_bg = 0x00FF0000;
  ed.layout.direction = Sloth_LayoutDirection_RightToLeft;
  sloth_push_widget(&sloth, ed, "right to left"); 
  {
    static r32 test = 0.5f;
    static r32 test_b = 0;
    
    
    Sloth_Widget_Desc inner = root_desc;
    inner.layout.position.kind = Sloth_LayoutPosition_ParentDecides;
    inner.layout.margin.top = SLOTH_SIZE_PIXELS(8);
    inner.layout.width = SLOTH_SIZE_PIXELS(150);
    inner.layout.height = SLOTH_SIZE_PIXELS(50);
    inner.style.color_bg = 0x88880000;        
    sloth_push_widget_f(&sloth, inner, "%f###inner-%d", test_b, 1);
    sloth_pop_widget(&sloth);
    
    inner.layout.position = (Sloth_Layout_Position){
      .kind = Sloth_LayoutPosition_FixedInParent,
      .left = SLOTH_SIZE_PIXELS(32), 
      .top = SLOTH_SIZE_PIXELS(32)
    };
    inner.layout.width = SLOTH_SIZE_PIXELS(150);
    inner.layout.height = SLOTH_SIZE_PIXELS(150);
    inner.style.color_bg = 0xFFFFFF00;
    inner.style.bg_glyph = (Sloth_Glyph_ID){
      .family = 255,
    };
    sloth_push_widget_f(&sloth, inner, "##inner%d", 2);
    sloth_pop_widget(&sloth);
    inner.style.bg_glyph.family = 0;
    
    inner.layout.position = (Sloth_Layout_Position){
      .kind = Sloth_LayoutPosition_FixedOnScreen,
      .left = SLOTH_SIZE_PIXELS(0), 
      .top = SLOTH_SIZE_PIXELS(0)
    };
    inner.style.color_bg = 0x88000000;
    sloth_push_widget_f(&sloth, inner, "I'm fixed on screen##inner-%d", 3);
    sloth_pop_widget(&sloth);
    
    inner.layout.position.kind = Sloth_LayoutPosition_ParentDecides;
    inner.style.color_bg = 0x88000000;
    sloth_push_widget_f(&sloth, inner, "inner-%d", 4);
    sloth_pop_widget(&sloth);
    
    inner.style.color_bg = 0x88000000;
    sloth_push_widget_f(&sloth, inner, "inner-%d", 5);
    sloth_pop_widget(&sloth);
    
  }
  sloth_pop_widget(&sloth);
  
  
  ed.style.color_bg = 0xFFFF0000;
  sloth_push_widget(&sloth, ed, "e2"); 
  sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  
  sloth_frame_advance(&sloth);
}

void game_cleanup (void* user_data) { }

void game_event(const sapp_event* event, void* user_data)
{
  if (event->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
    printf("Mouse Down\n");
  }
}

sapp_desc sokol_main(int argc, char* argv[]) {
  
  float Scale = .5f;
  return (sapp_desc) {
    .init_userdata_cb = game_init,
    .frame_userdata_cb = game_frame,
    .cleanup_userdata_cb = game_cleanup,
    .event_userdata_cb = game_event,
    
    
    .width = 1920 * Scale,
    .height = 1080 * Scale,
    .fullscreen = true,
    .window_title = "A Trick of Fate",
    .win32_console_create = true,
  };
}