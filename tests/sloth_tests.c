
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <math.h>

// Leaving the interface here, and including the implementation at the end
// to test doing the single file include trick
#include "../src/sloth.h"
#include "sloth_tests_utils.h"

#include "utest.h"

static Sloth_U32 sloth_test_widget_order_count = 0;
Sloth_Tree_Walk_Result
sloth_test_widget_order(Sloth_Ctx* ctx, Sloth_Widget* widget, Sloth_U8* user_data)
{
  Sloth_ID* ids = (Sloth_ID*)user_data;
  Sloth_ID id_at = ids[sloth_test_widget_order_count++];
  sloth_test_assert(sloth_ids_equal(widget->id, id_at));
  return Sloth_TreeWalk_Continue;
}

void
sloth_test_multi_frame_removal_frame(Sloth_Ctx* sloth, int num_to_remove)
{
  Sloth_Widget_Desc d = {};
  sloth_frame_prepare(sloth, (Sloth_Frame_Desc){});
  
  sloth_push_widget(sloth, d, "Root"); // root
  {  
    if (num_to_remove < 1) {
      sloth_push_widget(sloth, d, "remove 1");
      sloth_pop_widget(sloth);
    }
    
    sloth_push_widget(sloth, d, "bar_bounds_inner");
    {
      sloth_push_widget(sloth, d, "fg_bar");
      sloth_pop_widget(sloth);
    }
    sloth_pop_widget(sloth);
    
    if (num_to_remove < 2) {
      sloth_push_widget(sloth, d, "remove 2");
      sloth_pop_widget(sloth);
    }
  }
  sloth_pop_widget(sloth);
  
  sloth_frame_advance(sloth);
}
void
sloth_test_multi_frame_removal_frame_b(Sloth_Ctx* sloth, int num_to_remove)
{
  Sloth_Widget_Desc d = {};
  sloth_frame_prepare(sloth, (Sloth_Frame_Desc){});
  
  sloth_push_widget(sloth, d, "Root"); // root
  {  
    if (num_to_remove < 1) {
      sloth_push_widget(sloth, d, "remove 1");
      sloth_pop_widget(sloth);
    }
    
    sloth_push_widget(sloth, d, "a");
    {
      sloth_push_widget(sloth, d, "bar_bounds_inner");
      {
        sloth_push_widget(sloth, d, "fg_bar");
        sloth_pop_widget(sloth);
      }
      sloth_pop_widget(sloth);
      
      if (num_to_remove < 2) {
        sloth_push_widget(sloth, d, "remove 2");
        sloth_pop_widget(sloth);
      }
    }
    sloth_pop_widget(sloth);
  }
  sloth_pop_widget(sloth);
  
  sloth_frame_advance(sloth);
}

UTEST(widget_tree, multi_frame_removal)
{
  return;
  // The error this is trying to catch takes place across three frames
  // Frame 1: all widgets are drawn
  // Frame 2: a widget early in teh tree is removed
  // Frame 3: a widget late in the tree is removed
  //   importantly, this widget comes after a widget that
  //   has children. ie. The tree must look like:
  //     root
  //       <removed on frame 1>
  //       widget
  //         child
  //       <removed on frame 2>
  
  Sloth_Ctx sloth = {};
  sloth_test_multi_frame_removal_frame(&sloth, 0);
  sloth_test_multi_frame_removal_frame(&sloth, 1);
  sloth_test_multi_frame_removal_frame(&sloth, 2);
  sloth_ctx_free(&sloth);    
  
  sloth = (Sloth_Ctx){};
  sloth_test_multi_frame_removal_frame_b(&sloth, 0);
  sloth_test_multi_frame_removal_frame_b(&sloth, 1);
  sloth_test_multi_frame_removal_frame_b(&sloth, 2);
  sloth_ctx_free(&sloth);    
}

UTEST(math, vectors)
{
  Sloth_V2 va = { 25, 32.1f };
  Sloth_V2 vb = { 19, 18.1f };
  
  Sloth_V2 rv0 = sloth_v2_add(va, vb);
  EXPECT_TRUE(sloth_r32_equals(rv0.x, 44));
  EXPECT_TRUE(sloth_r32_equals(rv0.y, 50.2f));
  
  Sloth_V2 rv1 = sloth_v2_sub(va, vb);
  EXPECT_TRUE(sloth_r32_equals(rv1.x, 6));
  EXPECT_TRUE(sloth_r32_equals(rv1.y, 14));
  
  Sloth_V2 rv2 = sloth_v2_mulf(va, 2);
  EXPECT_TRUE(sloth_r32_equals(rv2.x, 50));
  EXPECT_TRUE(sloth_r32_equals(rv2.y, 64.2f));
  
}

UTEST(math, rects)
{
  // baseline rect
  Sloth_Rect rect0 = {
    .value_min = { 0, 0 },
    .value_max = { 100, 100 },
  };
  // overlaps rect0 to right and top
  Sloth_Rect rect1 = {
    .value_min = { 50, 50 },
    .value_max = { 150, 150 },
  };
  // overlaps rect1 to the left and bottom
  Sloth_Rect rect2 = {
    .value_min = { -50, -50 },
    .value_max = { 50, 50 },
  };
  // no overlap with rect0 to the left and bottom
  Sloth_Rect rect3 = {
    .value_min = { -250, -250 },
    .value_max = { -200, -200 }
  };
  // no overlap with rect0 to the right and top
  Sloth_Rect rect4 = {
    .value_min = { 250, 250 },
    .value_max = { 200, 200 }
  };
  // contains rect0
  Sloth_Rect rect5 = {
    .value_min = { -50, -50 },
    .value_max = { 200, 200 }
  };
  
  Sloth_Rect rr0 = sloth_rect_union(rect0, rect1);
  EXPECT_EQ(rr0.value_min.x, 50); EXPECT_EQ(rr0.value_min.y, 50);
  EXPECT_EQ(rr0.value_max.x, 100); EXPECT_EQ(rr0.value_max.y, 100);
  
  Sloth_Rect rr1 = sloth_rect_union(rect0, rect2);
  EXPECT_EQ(rr1.value_min.x, 0); EXPECT_EQ(rr1.value_min.y, 0);
  EXPECT_EQ(rr1.value_max.x, 50); EXPECT_EQ(rr1.value_max.y, 50);
  
  Sloth_Rect rr2 = sloth_rect_union(rect0, rect3);
  EXPECT_EQ(rr2.value_min.x, 0); EXPECT_EQ(rr2.value_min.y, 0);
  EXPECT_EQ(rr2.value_max.x, 0); EXPECT_EQ(rr2.value_max.y, 0);
  
  Sloth_Rect rr3 = sloth_rect_union(rect0, rect4);
  EXPECT_EQ(rr3.value_min.x, 0); EXPECT_EQ(rr3.value_min.y, 0);
  EXPECT_EQ(rr3.value_max.x, 0); EXPECT_EQ(rr3.value_max.y, 0);
  
  Sloth_Rect rr4 = sloth_rect_union(rect0, rect5);
  EXPECT_EQ(rr4.value_min.x, 0); EXPECT_EQ(rr4.value_min.y, 0);
  EXPECT_EQ(rr4.value_max.x, 100); EXPECT_EQ(rr4.value_max.y, 100);
  
  // contained by rect0
  Sloth_V2 rectp0 = { 25, 25 };
  // not contained by rect0 to the right and top
  Sloth_V2 rectp1 = { 150, 150 };
  // not contained by rect0 to the left and bottom
  Sloth_V2 rectp2 = { -25, -25 };
  
  EXPECT_TRUE(sloth_rect_contains(rect0, rectp0));
  EXPECT_TRUE(!sloth_rect_contains(rect0, rectp1));
  EXPECT_TRUE(!sloth_rect_contains(rect0, rectp2));
  
  // insetting rects
  Sloth_Rect clip   = {  5,  5, 10, 10 };
  {
    Sloth_Rect bounds = {  0,  0, 10, 10 };
    Sloth_Rect uv     = {  0,  0,  1,  1 };
    Sloth_Rect bounds_out, uv_out;
    sloth_clip_rect_and_uv(clip, bounds, uv, &bounds_out, &uv_out);
    EXPECT_EQ(bounds_out.value_min.x, 5 ); EXPECT_EQ(bounds_out.value_min.y, 5);
    EXPECT_EQ(bounds_out.value_max.x, 10); EXPECT_EQ(bounds_out.value_max.y, 10);
    EXPECT_EQ(uv_out.value_min.x, 0.5f);   EXPECT_EQ(uv_out.value_min.y, 0.5f);
    EXPECT_EQ(uv_out.value_max.x, 1   );   EXPECT_EQ(uv_out.value_max.y, 1);
  }
  {
    Sloth_Rect bounds = { 8, 8, 12, 12 };
    Sloth_Rect uv     = { 0.5f, 0.4f, 0.7f, 0.6f };
    Sloth_Rect bounds_out, uv_out;
    sloth_clip_rect_and_uv(clip, bounds, uv, &bounds_out, &uv_out);
    EXPECT_EQ(bounds_out.value_min.x, 8 ); EXPECT_EQ(bounds_out.value_min.y, 8);
    EXPECT_EQ(bounds_out.value_max.x, 10); EXPECT_EQ(bounds_out.value_max.y, 10);
    EXPECT_EQ(uv_out.value_min.x, 0.5f);   EXPECT_EQ(uv_out.value_min.y, 0.4f);
    EXPECT_EQ(uv_out.value_max.x, 0.6f);   EXPECT_EQ(uv_out.value_max.y, 0.5f);
  }
}

UTEST(math, gamma_correction)
{
  Sloth_R32 r_in = 0.2f;
  Sloth_R32 g_in = 0.5f;
  Sloth_R32 b_in = 0.9f;
  Sloth_R32 a_in = 0.1f;
  Sloth_U32 color = (((Sloth_U32)(r_in * 255) << 24) |
    ((Sloth_U32)(g_in * 255) << 16) |
    ((Sloth_U32)(b_in * 255) <<  8) |
    ((Sloth_U32)(a_in * 255)));
  
  // gamma = 1, no change
  Sloth_U32 color_out0 = sloth_color_apply_gamma(color, 1);
  sloth_assert(color_out0 == color);
  
  // gamma = 2.2, verify changes
  Sloth_U32 color_out1 = sloth_color_apply_gamma(color, 2.2f);
  Sloth_R32 r = (Sloth_R32)((color_out1 >> 24) & 0xFF) / 255.0f;
  Sloth_R32 g = (Sloth_R32)((color_out1 >> 16) & 0xFF) / 255.0f;
  Sloth_R32 b = (Sloth_R32)((color_out1 >>  8) & 0xFF) / 255.0f;
  Sloth_R32 a = (Sloth_R32)((color_out1      ) & 0xFF) / 255.0f;
  
  Sloth_R32 delta_r = fabsf(r - powf(r_in, 2.2f));
  Sloth_R32 delta_g = fabsf(g - powf(g_in, 2.2f));
  Sloth_R32 delta_b = fabsf(b - powf(b_in, 2.2f));
  Sloth_R32 delta_a = fabsf(a - powf(a_in, 2.2f));
  
  EXPECT_LT(delta_r, 0.01f);
  EXPECT_LT(delta_g, 0.01f);
  EXPECT_LT(delta_b, 0.01f);
  EXPECT_LT(delta_a, 0.01f);
}

UTEST(data, hashtable)
{
  Sloth_Hashtable table = {};
  sloth_hashtable_add(&table, 256, (Sloth_U8*)1);
  sloth_hashtable_add(&table, 394, (Sloth_U8*)2);
  sloth_hashtable_add(&table, 81932, (Sloth_U8*)3);
  
  // this should force chaining
  sloth_hashtable_add(&table, table.cap + 256, (Sloth_U8*)4);
  
  Sloth_U64 v0 = (Sloth_U64)sloth_hashtable_get(&table, 256);
  EXPECT_EQ(v0, 1);
  Sloth_U64 v1 = (Sloth_U64)sloth_hashtable_get(&table, 394);
  EXPECT_EQ(v1, 2);
  Sloth_U64 v2 = (Sloth_U64)sloth_hashtable_get(&table, 81932);
  EXPECT_EQ(v2, 3);
  Sloth_U64 v3 = (Sloth_U64)sloth_hashtable_get(&table, table.cap + 256);
  EXPECT_EQ(v3, 4);
  
  // getting a value that's not present
  Sloth_U64 vi = (Sloth_U64)sloth_hashtable_get(&table, 3333);
  EXPECT_EQ(vi, 0);
  
  Sloth_Bool r0 = sloth_hashtable_rem(&table, 256);
  EXPECT_TRUE(r0);
  v0 = (Sloth_U64)sloth_hashtable_get(&table, 256);
  EXPECT_EQ(v0, 0);
}

UTEST(memory, arena)
{
  Sloth_Arena arena = {};
  Sloth_U32* array_0 = sloth_arena_push_array(&arena, Sloth_U32, 32);
  for (Sloth_U32 i = 0; i < 32; i++) array_0[i] = i;
  EXPECT_NE(array_0, (Sloth_U32*)0);
  
  Sloth_Arena_Loc old_at = sloth_arena_at(&arena);
  EXPECT_EQ(old_at.bucket_at, sizeof(Sloth_U32) * 32);
  
  Sloth_U32* array_1 = sloth_arena_push_array(&arena, Sloth_U32, 32);
  for (Sloth_U32 i = 0; i < 32; i++) array_1[i] = (i + 32);
  EXPECT_GE(array_1, (array_0 + 32));
  EXPECT_NE(array_1, (Sloth_U32*)0);
  EXPECT_EQ(array_0[31], 31);
  EXPECT_EQ(array_1[0], 32);
  
  // testing memory reuse after popping
  sloth_arena_pop(&arena, old_at);
  // test that in debug mode, popped memory is cleared
  // NOTE: that if we aren't in debug mode, sloth_test_assert evaluates to
  // nothing, so the test won't run
  for (Sloth_U32 i = 0; i < 32; i++) EXPECT_EQ(array_1[i], 0);
  
  Sloth_U32* array_1b = sloth_arena_push_array(&arena, Sloth_U32, 32);
  EXPECT_EQ(array_1, array_1b);
  
  // testing memory reuse after clearing
  sloth_arena_clear(&arena);
  Sloth_U32* array_0b = sloth_arena_push_array(&arena, Sloth_U32, 32);
  EXPECT_EQ(array_0, array_0b);
  
  sloth_arena_free(&arena);
  EXPECT_EQ(arena.buckets, (Sloth_U8**)0);
  EXPECT_EQ(arena.buckets_len, 0);
  EXPECT_EQ(arena.buckets_cap, 0);
  EXPECT_EQ(arena.bucket_cap, 0);
  EXPECT_EQ(arena.curr_bucket_len, 0);
}

UTEST(widget, cache_pool)
{
  Sloth_Ctx sloth = {};
  Sloth_Widget_Cached* tracking[128];
  Sloth_U32 tracking_len = 0;
  for (Sloth_U32 i = 0; i < 2048; i++)
  {
    if (i == 1024) {
      int x = 5;
    }
    Sloth_Widget_Cached* c = sloth_widget_cached_pool_take(&sloth);
    c->canary_start_ = i % 255;
    if (i % 33 == 0 && tracking_len < 128) {
      tracking[tracking_len++] = c;
    }
  }
  Sloth_U32 cache_pool_size_before = (sloth.widget_caches.bucket_at * sloth.widget_caches.bucket_cap) + sloth.widget_caches.bucket_at_len;
  
  for (Sloth_U32 i = 0; i < tracking_len; i++)
  {
    sloth_widget_cached_pool_give(&sloth, tracking[i]);
  }
  EXPECT_NE(sloth.widget_caches.free_list, (Sloth_Widget_Cached*)0);
  EXPECT_EQ(sloth.widget_caches.free_list, tracking[tracking_len - 1]);
  EXPECT_NE(sloth.widget_caches.free_list->free_next, (Sloth_Widget_Cached*)0);
  
  for (Sloth_U32 i = 0; i < tracking_len; i++)
  {
    sloth_widget_cached_pool_take(&sloth);
  }
  Sloth_U32 cache_pool_size_after = (sloth.widget_caches.bucket_at * sloth.widget_caches.bucket_cap) + sloth.widget_caches.bucket_at_len;
  EXPECT_EQ(cache_pool_size_before, cache_pool_size_after);
  EXPECT_EQ(sloth.widget_caches.free_list, (Sloth_Widget_Cached*)0);
}

UTEST(widget, id)
{
  // ID Creation Tests
  char test_id_str[]   = "Test id##53";
  int  test_id_str_len = (sizeof(test_id_str) / sizeof(char)) - 1;
  
  Sloth_ID_Result id0 = sloth_make_id(test_id_str);
  EXPECT_NE(id0.id.value, 0);
  EXPECT_EQ(id0.display_len, test_id_str_len - 4);
  
  Sloth_ID_Result id1 = sloth_make_id_len(11, "Test id##53");
  EXPECT_EQ(id0.id.value, id1.id.value);
  
  Sloth_ID_Result id2 = sloth_make_id_f("Test id###%d", 53);
  EXPECT_NE(id2.id.value, 0);
  EXPECT_NE(id2.id.value, id0.id.value);
  EXPECT_EQ(id2.display_len, 7);
}

UTEST(glyph, glyph_id)
{
  // Glyph ID Tests
  Sloth_Glyph_ID g_id = sloth_make_glyph_id(24, 'G');
  Sloth_Glyph_ID newline_id = sloth_make_glyph_id(32, '\n');
  Sloth_Glyph_ID space_id = sloth_make_glyph_id(127, ' ');
  EXPECT_TRUE(sloth_glyph_id_matches_charcode(g_id, 'G'));
  EXPECT_TRUE(sloth_glyph_id_matches_charcode(newline_id, '\n'));
  EXPECT_TRUE(sloth_glyph_id_matches_charcode(space_id, ' '));
}

UTEST(glyph, glyph_store)
{
  Sloth_U32 test_icon[] = {
    0xFFFFFFFF, 0x000000FF, 0xFFFFFFFF, 0x000000FF,
    0x000000FF, 0xFFFFFFFF, 0x000000FF, 0xFFFFFFFF,
    0xFFFFFFFF, 0x000000FF, 0xFFFFFFFF, 0x000000FF,
    0x000000FF, 0xFFFFFFFF, 0x000000FF, 0xFFFFFFFF,
  };
  
  Sloth_Ctx sloth = {};
  Sloth_Glyph_Atlas* atlas = sloth_create_atlas(&sloth, 0, 16);
  EXPECT_NE(atlas->data, (Sloth_U8*)0);
  EXPECT_GE(atlas->dim, 16);
  
  Sloth_Glyph_Store store_ = {};
  Sloth_U32 atlas_i = 0;
  sloth_glyph_atlas_resize(&sloth.glyph_atlases[atlas_i], 32);
  
  Sloth_Glyph_Desc gd0 = {
    .family = 1,
    .id = 25,
    .data = (Sloth_U8*)test_icon,
    .src_width = 4,
    .src_height = 4,
    .stride = 4,
    .format = Sloth_GlyphData_RGBA8,
  };
  Sloth_Glyph_ID id_0 = sloth_register_glyph(&sloth, gd0);
  Sloth_U32 last_glyph = sloth.glyph_atlases[atlas_i].last_glyph;
  EXPECT_EQ(sloth.glyph_store.glyphs_table.used, 1);
  
  // testing adding the same glyph a second time. 
  Sloth_Glyph_ID id_01 = sloth_register_glyph(&sloth, gd0);
  EXPECT_EQ(id_01.value, id_0.value);
  EXPECT_EQ(sloth.glyph_atlases[atlas_i].last_glyph, last_glyph);
  EXPECT_EQ(sloth.glyph_store.glyphs_table.used, 1); // no sprite was added
  
  Sloth_Glyph_Desc gd2 = gd0;
  gd2.id = 26;
  Sloth_Glyph_ID id_2 = sloth_register_glyph(&sloth, gd2);
  EXPECT_NE(id_2.value, 0);
  
  Sloth_Glyph_Desc gd3 = gd0;
  gd3.id = 27;
  Sloth_Glyph_ID id_3 = sloth_register_glyph(&sloth, gd3);
  EXPECT_NE(id_3.value,  0);
  
  Sloth_Glyph_Desc gd4 = gd0;
  gd4.id = 28;
  Sloth_Glyph_ID id_4 = sloth_register_glyph(&sloth, gd4);
  EXPECT_NE(id_4.value,  0);
  EXPECT_EQ(id_4.family,  gd0.family);
  EXPECT_EQ(id_4.id[0],  28); EXPECT_EQ(id_4.id[1], 0); EXPECT_EQ(id_4.id[2], 0);
  
  Sloth_Glyph_Desc gd5 = gd0;
  gd5.id = 29; 
  Sloth_Glyph_ID id_5 = sloth_register_glyph(&sloth, gd5);
  EXPECT_NE(id_5.value, 0);
  EXPECT_EQ(id_5.family, 1);
  EXPECT_EQ(id_5.id[0], 29); EXPECT_EQ(id_5.id[1], 0); EXPECT_EQ(id_5.id[2], 0);
  
  sloth_ctx_free(&sloth);
}

UTEST(layout, size)
{
  // see @Maintenance tag in Sloth_Size_Box if this fails
  Sloth_Size_Box b = {
    .left = SLOTH_SIZE_PIXELS(0),
    .right = SLOTH_SIZE_PIXELS(5),
    .top = SLOTH_SIZE_PIXELS(10),
    .bottom = SLOTH_SIZE_PIXELS(15),
  };
  
  // testing to make sure left corresponds to E[Axis_X].min
  // and so on
  EXPECT_EQ(b.E[Sloth_Axis_X].min.value, 0);
  EXPECT_EQ(b.E[Sloth_Axis_X].max.value, 5);
  EXPECT_EQ(b.E[Sloth_Axis_Y].min.value, 10);
  EXPECT_EQ(b.E[Sloth_Axis_Y].max.value, 15);
}

UTEST(widget_tree, construction)
{
  Sloth_Ctx sloth = {
    .per_frame_memory.name = "pfm",
    .scratch.name = "scratch",
  };
  
  Sloth_Widget_Desc d = {}; // these tests don't depend on the desc at all
  Sloth_ID ids0_preorder[] = {
    sloth_make_id_f("Root").id, 
    sloth_make_id_f("W1").id, 
    sloth_make_id_f("W11").id, 
    sloth_make_id_f("W12").id, 
    sloth_make_id_f("W2").id, 
    sloth_make_id_f("W3").id, 
    sloth_make_id_f("W31").id
  };
  Sloth_ID ids0_postorder[] = {
    sloth_make_id_f("W11").id,
    sloth_make_id_f("W12").id,
    sloth_make_id_f("W1").id,
    sloth_make_id_f("W2").id,
    sloth_make_id_f("W31").id,
    sloth_make_id_f("W3").id,
    sloth_make_id_f("Root").id,
  };
  
  sloth_frame_prepare(&sloth, (Sloth_Frame_Desc){});
  Sloth_Widget_Result r0 = sloth_push_widget(&sloth, d, "Root"); // root
  Sloth_Widget_Result r1 = sloth_push_widget(&sloth, d, "W1");
  Sloth_Widget_Result r2 = sloth_push_widget(&sloth, d, "W11"); sloth_pop_widget_safe(&sloth, r2);
  Sloth_Widget_Result r3 = sloth_push_widget(&sloth, d, "W12"); sloth_pop_widget_safe(&sloth, r3);
  sloth_pop_widget_safe(&sloth, r1);
  Sloth_Widget_Result r4 = sloth_push_widget(&sloth, d, "W2"); sloth_pop_widget_safe(&sloth, r4);
  Sloth_Widget_Result r5 = sloth_push_widget(&sloth, d, "W3"); 
  Sloth_Widget_Result r6 = sloth_push_widget(&sloth, d, "W31"); sloth_pop_widget_safe(&sloth, r6);
  sloth_pop_widget_safe(&sloth, r5);
  sloth_pop_widget_safe(&sloth, r0); // root - won't pop
  
  // walking the tree
  EXPECT_NE(sloth.widget_tree_root, (Sloth_Widget*)0);
  EXPECT_EQ(sloth.widget_tree_depth_max, 3);
  sloth_test_widget_order_count = 0; // reset test
  sloth_tree_walk_preorder(&sloth, sloth_test_widget_order, (Sloth_U8*)&ids0_preorder);
  sloth_test_widget_order_count = 0; // reset test
  sloth_tree_walk_postorder(&sloth, sloth_test_widget_order, (Sloth_U8*)&ids0_postorder);
  //sloth_widget_tree_print(&sloth);
  
  sloth_frame_advance(&sloth);
  sloth_frame_prepare(&sloth, (Sloth_Frame_Desc){});
  
  // Same Frame as above
  sloth_push_widget(&sloth, d, "Root"); // root
  sloth_push_widget(&sloth, d, "W1"); 
  sloth_push_widget(&sloth, d, "W11"); sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W12"); sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W2"); sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W3"); 
  sloth_push_widget(&sloth, d, "W31"); sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth); // root - won't pop
  
  // walking the tree
  EXPECT_NE(sloth.widget_tree_root, (Sloth_Widget*)0);
  sloth_test_widget_order_count = 0; // reset test
  sloth_tree_walk_preorder(&sloth, sloth_test_widget_order, (Sloth_U8*)&ids0_preorder);
  
  sloth_frame_advance(&sloth);
  sloth_frame_prepare(&sloth, (Sloth_Frame_Desc){});
  
  // Different frame from above
  Sloth_ID ids1[] = {
    sloth_make_id_f("Root").id, 
    sloth_make_id_f("W1").id, 
    sloth_make_id_f("W11").id, 
    sloth_make_id_f("W13").id, 
    sloth_make_id_f("W14").id, 
    sloth_make_id_f("W12").id, 
    sloth_make_id_f("W2").id, 
    sloth_make_id_f("W21").id, 
    sloth_make_id_f("W3").id,
  };
  sloth_push_widget(&sloth, d, "Root"); // root
  sloth_push_widget(&sloth, d, "W1"); 
  sloth_push_widget(&sloth, d, "W11"); sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W13"); sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W14"); sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W12"); sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W2"); 
  sloth_push_widget(&sloth, d, "W21"); sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  sloth_push_widget(&sloth, d, "W3"); 
  // old child should get freed
  sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth); // root - won't pop
  
  sloth_test_widget_order_count = 0; // reset test
  sloth_tree_walk_preorder(&sloth, sloth_test_widget_order, (Sloth_U8*)&ids1);    
  
  sloth_ctx_free(&sloth);
}

UTEST(layout, widget_sizing)
{
  Sloth_Ctx sloth = {};
  
  sloth_frame_prepare(&sloth, (Sloth_Frame_Desc){});
  
  Sloth_Widget_Desc ele_desc;
  Sloth_Widget_Desc root_desc = {
    .layout = {
      .width = SLOTH_SIZE_PIXELS(800),
      .height = SLOTH_SIZE_PIXELS(900),
      //.margin.top = SLOTH_SIZE_PIXELS(32),
      .direction = Sloth_LayoutDirection_TopDown,
    },
    .style.color_bg = 0x333333FF,
  };
  sloth_push_widget(&sloth, root_desc, "root");
  ele_desc = (Sloth_Widget_Desc){
    .layout = {
      .width = SLOTH_SIZE_PIXELS(850),
      .height = SLOTH_SIZE_PIXELS(200),
    },
    .style.color_bg = 0xFFFFFFFF,
  };
  sloth_push_widget(&sloth, ele_desc, "ele0"); sloth_pop_widget(&sloth);
  ele_desc.style.color_bg = 0xFF00FFFF;
  sloth_push_widget(&sloth, ele_desc, "ele1"); sloth_pop_widget(&sloth);
  sloth_pop_widget(&sloth);
  
  sloth_frame_advance(&sloth);
  sloth_frame_prepare(&sloth, (Sloth_Frame_Desc){});
  
  Sloth_Widget* root = sloth.widget_tree_next_child;
  EXPECT_EQ(root->cached->offset.x, 0); EXPECT_EQ(root->cached->offset.y, 0);
  EXPECT_EQ(root->cached->dim.x, 800); EXPECT_EQ(root->cached->dim.y, 900);
  
  Sloth_Widget* ele0 = root->child_first;
#if DO_CLIP
  EXPECT_EQ(ele0->cached->offset.x, 0); EXPECT_EQ(ele0->cached->offset.y, 0);
  EXPECT_EQ(ele0->cached->dim.x, 800); EXPECT_EQ(ele0->cached->dim.y, 200);
#endif
  
  Sloth_Widget* ele1 = ele0->sibling_next;
#if DO_CLIP
  EXPECT_EQ(ele1->cached->offset.x, 0); EXPECT_EQ( ele1->cached->offset.y, 200);
  EXPECT_EQ(ele1->cached->dim.x, 800); EXPECT_EQ(ele1->cached->dim.y, 200);
#endif
  
  sloth_ctx_free(&sloth);
}

#define SLOTH_IMPLEMENTATION 1
#include "../src/sloth.h"

UTEST(math, is_pow2)
{
  EXPECT_TRUE(sloth_is_pow2(2048));
  EXPECT_FALSE(sloth_is_pow2(1920));
}

UTEST(math, remap)
{
  Sloth_R32 v0 = sloth_remap_r32(2, 1.5f, 2.5f, 5, 6);
  EXPECT_EQ(v0, 5.5f);
  
  Sloth_R32 v1 = sloth_remap_r32(1, 1.5f, 2.5f, 5, 6);
  EXPECT_EQ(v1, 4.5f);
  
  Sloth_R32 v2 = sloth_remap_r32(3, 1.5f, 2.5f, 5, 6);
  EXPECT_EQ(v2, 6.5f);
}

UTEST_MAIN();
