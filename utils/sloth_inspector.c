#if SLOTH_INSPECTOR_IMPLEMENTATION

Sloth_Widget_Desc sloth_onscreen_inspector_desc = {
  .layout = {
    .margin = {
      .left = SLOTH_SIZE_PIXELS(8),
      .right = SLOTH_SIZE_PIXELS(8),
      .top = SLOTH_SIZE_PIXELS(4),
      .bottom = SLOTH_SIZE_PIXELS(16),
    },
  },
  .style = {
    .color_bg = 0x333333FF,
  }
};

Sloth_Widget_Desc sloth_onscreen_inspector_inset_box = {
  .layout = {
    .width  = SLOTH_SIZE_CHILDREN_SUM,
    .height = SLOTH_SIZE_CHILDREN_SUM,
    .margin = {
      .left = SLOTH_SIZE_PIXELS(16),
    },
  },
};

Sloth_Widget_Desc sloth_onscreen_inspector_row = {
  .layout = {
    .width  = SLOTH_SIZE_CHILDREN_SUM,
    .height = SLOTH_SIZE_CHILDREN_SUM,
    .direction = Sloth_LayoutDirection_LeftToRight,
    .margin.bottom = SLOTH_SIZE_PIXELS(8),
  },
};

Sloth_Widget_Desc sloth_onscreen_inspector_button = {
  .layout = {
    .width = SLOTH_SIZE_TEXT_CONTENT,
    .height = SLOTH_SIZE_TEXT_CONTENT,
    .margin = {
      .left = SLOTH_SIZE_PIXELS(8),
      .right = SLOTH_SIZE_PIXELS(8),
      .top = SLOTH_SIZE_PIXELS(4),
      .bottom = SLOTH_SIZE_PIXELS(4),
    },
  },
  .style = {
    .color_bg   = 0x3300FFFF,
    .color_text = 0xFFFFFFFF,
    .color_outline = 0x000000FF,
    .outline_thickness = 1,
    .text_style = Sloth_TextStyle_Align_Center,
  },
};

Sloth_Function Sloth_Widget*
sloth_inspector_find_widget(Sloth_Ctx* sloth, Sloth_ID id)
{
  Sloth_Widget* result = 0;
  Sloth_Widget_Pool pool = sloth->widgets;
  for (Sloth_U32 i = 0; i < pool.len; i++)
  {
    Sloth_Widget* at = pool.values + i;
    if (sloth_ids_equal(at->id, id)) {
      result = at;
      break;
    }
  }
  return result;
}

Sloth_Function Sloth_Bool
sloth_inspector_button(Sloth_Ctx* sloth, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_U32 color_bg = 0x0000FFFF;
  Sloth_U32 color_text = 0xFFFFFFFF;
  Sloth_U32 color_outline = 0x000000FF;
  Sloth_Bool result = sloth_cmp_button_v(sloth, color_bg, color_text, color_outline, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Bool
sloth_inspector_toggle(Sloth_Ctx* sloth, Sloth_Bool state, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_U32 color_bg = 0x0000FFFF;
  Sloth_U32 color_text = 0xFFFFFFFF;
  Sloth_U32 color_outline = 0x000000FF;
  Sloth_Bool result = sloth_cmp_toggle_v(sloth, state, color_bg, color_text, color_outline, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function void
sloth_inspector_print_size_s(Sloth_Ctx* sloth, Sloth_Size size, char* title, char* id)
{
  // TODO(PS): we can probably make this a grid so they end up evenly spaced
  char* kind_str = sloth_size_kind_strs[size.kind];
  sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "%s: %s - %.2f##si_size_%s",
    title, kind_str, size.value, id);
}

Sloth_Function void
sloth_inspector_print_size_i(Sloth_Ctx* sloth, Sloth_Size size, char* title, Sloth_U32 id)
{
  // TODO(PS): we can probably make this a grid so they end up evenly spaced
  char* kind_str = sloth_size_kind_strs[size.kind];
  sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "%s: %s - %.2f##si_size_%u",
    title, kind_str, size.value, id);
}

Sloth_Function void
sloth_inspector_print_size_box(Sloth_Ctx* sloth, Sloth_Size_Box box, char* title, char* id)
{
  Sloth_Widget_Result r = sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "%s##si_sizebox_%s", title, id);
  Sloth_U32 id_inner = r.widget->id.value;
  Sloth_Widget_Result r0 = sloth_push_widget_f(sloth, sloth_onscreen_inspector_inset_box, "##%s_%s_inset", id, title);
  {
    sloth_inspector_print_size_i(sloth, box.left, "Left", id_inner);
    sloth_inspector_print_size_i(sloth, box.right, "Right", id_inner);
    sloth_inspector_print_size_i(sloth, box.top, "Top", id_inner);
    sloth_inspector_print_size_i(sloth, box.bottom, "Bottom", id_inner);
  }
  sloth_pop_widget_safe(sloth, r0);
}

Sloth_Function void
sloth_inspector_print_position(Sloth_Ctx* sloth, Sloth_Layout_Position pos, char* title, char* id)
{
  Sloth_Widget_Result r = sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "%s##si_position_%s", title, id);
  Sloth_U32 id_inner = r.widget->id.value;
  Sloth_Widget_Result r0 = sloth_push_widget_f(sloth, sloth_onscreen_inspector_inset_box, "##%s_%s_inset", id, title);
  {
    char* kind_str = sloth_layout_position_kind_strs[pos.kind];
    sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "Kind: %s##position_kind_%llu_%s", kind_str, id_inner, title);
    sloth_inspector_print_size_i(sloth, pos.left, "Left", id_inner);
    sloth_inspector_print_size_i(sloth, pos.right, "Right", id_inner);
    sloth_inspector_print_size_i(sloth, pos.top, "Top", id_inner);
    sloth_inspector_print_size_i(sloth, pos.bottom, "Bottom", id_inner);
    sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "Z: %f##position_z_%llu_%s", pos.z, id_inner, title);
  }
  sloth_pop_widget_safe(sloth, r0);
}

Sloth_Function void
sloth_inspector_print_widget_layout(Sloth_Ctx* sloth, Sloth_Widget_Layout layout, char* title, char* id)
{
  sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "%s##id", title, id);
  Sloth_Widget_Result r0 = sloth_push_widget_f(sloth, sloth_onscreen_inspector_inset_box, "##%s_%s_layout_inset", title, id);
  {
    sloth_inspector_print_size_s(sloth, layout.width, "Width", id);
    sloth_inspector_print_size_s(sloth, layout.height, "Height", id);
    sloth_inspector_print_size_box(sloth, layout.margin, "Margin", id);
    sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "Direction: %s##%s", sloth_layout_direction_strs[layout.direction], id);
    sloth_inspector_print_position(sloth, layout.position, "Position", id);
  }
  sloth_pop_widget_safe(sloth, r0);
}

Sloth_Function void
sloth_inspector_print_widget_style(Sloth_Ctx* sloth, Sloth_Widget_Style style, char* title, char* id)
{
  
}

static Sloth_Widget sloth_inspector_last_hot_widget;
static Sloth_Bool   sloth_inspector_show_texture_atlas;
static Sloth_U32    sloth_inspector_active_atlas;

Sloth_Function void
sloth_inspector_prepare(Sloth_Ctx* sloth)
{
  if (sloth->hot_widget.value != 0)
  {
    Sloth_Widget* lhw = sloth_inspector_find_widget(sloth, sloth->hot_widget);
    if (lhw) {
      sloth_inspector_last_hot_widget = *lhw;
    }
  }
}

// NOTE(PS): push a font_id on sloth before calling this to control
// what font it will display as.
Sloth_Function void
sloth_inspector_onscreen(Sloth_Ctx* sloth)
{
  Sloth_Widget_Result r0 = sloth_push_widget_f(sloth, sloth_onscreen_inspector_desc, "##sloth onscreen inspector");
  {
    sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "Sloth Inspector##si");
    sloth_push_widget_f(sloth, sloth_onscreen_inspector_row, "##sloth_inspector_nav_row");
    {
      if (sloth_inspector_button(sloth, "<##sloth_inspector_textureatlas_down"))
      {
        if (sloth_inspector_active_atlas > 0) {
          sloth_inspector_active_atlas -= 1;
        }
      }
      
      sloth_inspector_show_texture_atlas = sloth_inspector_toggle(sloth, sloth_inspector_show_texture_atlas, "Show Atlases##sloth_inspector");
      
      if (sloth_inspector_button(sloth, ">##sloth_inspector_textureatlas_up"))
      {
        if (sloth_inspector_active_atlas < (sloth->glyph_atlases_cap - 1)) {
          sloth_inspector_active_atlas += 1;
        }
      }
    }
    sloth_pop_widget(sloth);
    
    if (sloth_inspector_show_texture_atlas)
    {
      Sloth_VIBuffer* vibuf = sloth->vibuffers + sloth_inspector_active_atlas;
      Sloth_Glyph_Atlas* atlas = sloth->glyph_atlases + sloth_inspector_active_atlas;
      Sloth_Rect tex_bounds;
      tex_bounds.value_min.x = 512;
      tex_bounds.value_min.y = 10;
      tex_bounds.value_max.x = tex_bounds.value_min.x + atlas->dim;
      tex_bounds.value_max.y = tex_bounds.value_min.y + atlas->dim;
      Sloth_V2 uv_min, uv_max;
      uv_min.x = 0; uv_min.y = 0;
      uv_max.x = 1; uv_max.y = 1;
      sloth_render_quad_ptc(vibuf, tex_bounds, 0, uv_min, uv_max, 0xFFFFFFFF);
    }
    
    if (sloth->hot_widget.value != 0) {
      Sloth_Widget hot_widget = sloth_inspector_last_hot_widget;
      sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "Hot Widget##si");
      sloth_cmp_text_f(sloth, 0, 0xFFFFFFFF, "ID: %u##si", hot_widget.id);
      sloth_inspector_print_widget_layout(sloth, hot_widget.layout, "Layout", "hot_widget");
      sloth_inspector_print_widget_style(sloth, hot_widget.style, "Style", "hot_widget");
    }
  }
  sloth_pop_widget_safe(sloth, r0);
}

#endif // SLOTH_INSPECTOR_IMPLEMENTATION