#ifndef SLOTH_INTERFACE
#define SLOTH_INTERFACE

// DOCUMENTATION
// 
// Bare minimum usage requirements:
// Copy the following into ONE file in your codebase
//
//   #define SLOTH_IMPLEMENTATION 1
//   #include "stb_sprintf.h"
//   #include "sloth.h"
//
// Examples:
// 
// Immediately following this block comment is an example
// of setting sloth.h up with a sokol_app/sokol_gfx environment.
//
// How It Works:
//
// sloth.h exposes an immediate mode interface that allows
// you to write expressive interfaces via simple, composable
// commands.
// 
// For example, a button might look like:
//     if (my_button(&sloth_ctx).clicked) {
//       // respond to button click
//     }
//
// Internally, sloth is actually a retained mode ui that
// maintains a tree of widgets which make up the interface.
// That tree is pruned and growns each frame based on the 
// immediate mode function calls.
//
// So if on Frame N, you call my_button for the first time
// a widget will be added to the widget tree.
// On Frame N+1, if you call my_button again, the existing
// widget is used.
// On Frame N+2, if you don't call my_button, the widget
// corresponding to the button will be removed from the tree.
//
// This allows for an immediate mode interface (simple, easy to understand)
// that is rendered with arbitrarily complex layout routines
// (expressive, hand tailored).
//
// The one drawback of this is that all user input must happen
// on a one frame delay, since a widget doesn't know its
// size and position until the layout routine has been run,
// which happens at the end of the frame. 
// This is usually a small price to pay.

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// TODO
//   
// - CLEANUP
//   - use u16 indices in teh vertex buffer
// - STYLE IMPROVEMENTS
//   - border radius, in a way that works with outlines too, regardless
//     size on screen
//   
// - figure out a way for scroll bars to retain their own state, and
//   update the value parameter every frame so that the calling code
//   doesn't need a "visible" and "actual" version of every variable
//   - I suspect this will involve ignoring "value" while the mouse is
//     down
// - copying selected text
// - UTF-8 and UTF-32 Support in fonts and glyphs
//   - Glpyh IDs should have an 8 bit family and a 32 bit id, so that
//     the id supports utf8 
// - Figure out things like weight and bold/italics for font rendering
// - glyph atlas should use premultiplied alpha
// - if we use a separate texture for different glyph families, is there 
//   a way we could use one bit alpha mask textures for families that
//   only have text glyphs?
//   - possibly, we could default to one bit textures until it contains
//     an image that has rgb data in it, and then upgrade the texture
//     to rgb?
// - popup buffer - A way to essentially offset the z index of a widget 
//   so that when it is eventually rendered, it appears on top. This should
//   be pretty easy. 
//   - of the visible range, reserve 50% for default rendering. The back 25%
//     can then be for z-indices -1 -> -infinity, and the front 25% can be for
//     z-indices 1->infinity. The offsets of these z-indexed widgets get scaled
//     based on the number of z-indices used. ie if the z-indices used are 
//     0 (default), 1 and 2, then z-index 1 gets to take from 75% -> 87.5%, and
//     z-index 2 gets to take from 87.5% -> 100% of the visible range
// - widget pool growing reallocates in the middle of a frame. We need to 
//   turn it into a buffer arena
// - multi-font-property line heights - right now the last line height used
//   will be what the entire text block is treated like. Need a more sophisticated
//   solution.
//   - probably involves tracking the actual line offsets used for each line
//   - also will require lookahead for each line, since the baseline would be
//     decided by the highest offset
// - ability to load in a whole sprite atlas, along with glyph data prebaked

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// 
// OPTIMIZATION CANDIDATES:
//
// - widgets currently contain their entire style, layout, and input 
//   descriptors. Creating pointers to these will make the structure
//   size of each widget much smaller (especially as style and layout 
//   continue to grow). 
//   - One downside is that you'd want to force users to register 
//     these descriptors so that they have known memory lifetimes. 
//     On the other hand, this would also reduce duplicate memory
//     storage of the same descriptor.
//   - One thing that could work is to hash the descriptors when they
//     are provided, and either point to an existing one thats already
//     stored, or store the new one. 
//     This would require some eviction strategy.
//   - This might make things like style composability an option
//     ie. The default for fields becomes (INHERIT OR DEFAULT)
//     On the other hand, having to loop looking upwards in a hierarchy
//     isn't great. Maybe at registration time, the actual values for
//     every field are computed (a styles parent would have to be known
//     at registration time)
// 
// - Turn Glyph_Layout's into a discriminated union that serves one of
//   several purposes:
//   - Describe a glyph
//   - Change active font properties like typeface, size, bold, color, etc.
//     (effectively, start a span)
// 
// - Long Text - odds are, long blocks of text will not all be visible
//   at once. 

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//  SAMPLE PROGRAM
//

#if 0

int main(int argc, char** args)
{
  // TODO(PS): set up a basic context
  Sloth_Ctx* sloth = 0;
  
  
  while (do_next_frame)
  {
    {
      // EXAMPLE:
      // How to push text of multiple fonts into a single widget
      //
      Sloth_Font_ID font_a, font_b;
      
      // Creates an id for a widget with the display text of "Button"
      Sloth_ID_Result id_result = sloth_make_id_f("Button##Text");
      
      // Create the widget, and allocate space for the "Button" string glyphs
      Sloth_Widget_Result widget_result = sloth_push_widget_id(sloth, multi_font_widget_desc, id_result.id);
      sloth_widget_allocate_text(S, widget_result, id_result.display_len);
      
      // pushes the 'B' character onto the glyph array as a font_a glyph
      sloth_widget_text_to_glyphs_append(sloth, widget_result, font_a, id_result.formatted, 1);
      
      // pushes everything after the 'B' character onto the glyph array as font_b glyphs
      sloth_widget_text_to_glyphs_append(sloth, widget_result, font_b, id_result.formatted + 1, Idr.display_len - 1);
      
      // pop the widget off
      sloth_pop_widget_safe(sloth, widget_result);
    }
  }
  
  // TODO(PS): cleanup & shutdown
  
  return 0;
}

#endif

//////// TYPE DEFINITIONS ////////
// NOTE: These allow for the standard
// types to be overridden by outer code
// simply by defining the macro first

#ifndef Sloth_Char
#  define Sloth_Char char
#endif

#ifndef Sloth_U8
#  define Sloth_U8 unsigned char
#endif

#ifndef Sloth_U32
#  define Sloth_U32 unsigned int
#endif

#ifndef Sloth_U64
#  define Sloth_U64 unsigned long long int
#endif

#ifndef Sloth_S32
#  define Sloth_S32 int
#endif

#ifndef Sloth_R32
#  define Sloth_R32 float
#endif
#define Sloth_R32_Max  3.402823466e+38f
#define Sloth_R32_Min -3.402823466e+38f

#ifndef Sloth_Bool
#  ifdef SLOTH_USE_STD_BOOL
#    include <stdbool.h>
#    define Sloth_Bool bool
#  else
#    define Sloth_Bool unsigned char
#    if !defined(true) && !defined(false)
#      define true 1
#      define false 0
#    endif
#    if !defined(true) || !defined(false)
#      error "You must define both true and false or neither.
#    endif
#  endif // SLOTH_USE_STD_BOOL
#endif

#ifndef Sloth_Function
#  define Sloth_Function
#endif

#ifndef Sloth_Temp_String_Memory_Size
#  define Sloth_Temp_String_Memory_Size 512
#endif

#ifndef SLOTH_PROFILE_BEGIN
#  define SLOTH_PROFILE_BEGIN
#endif

#ifndef SLOTH_FORCE_NO_STDARG
#  include <stdarg.h>
#endif

//////// DATA TYPES  ////////

typedef struct Sloth_Ctx Sloth_Ctx;

typedef struct Sloth_ID Sloth_ID;
struct Sloth_ID
{
  Sloth_U32 value;
};

typedef struct Sloth_ID_Result Sloth_ID_Result;
struct Sloth_ID_Result
{
  Sloth_ID id;
  Sloth_U32 display_len;
  Sloth_Char* formatted;
};

typedef union Sloth_V2 Sloth_V2;
union Sloth_V2
{
  struct {
    Sloth_R32 x;
    Sloth_R32 y;
  };
  Sloth_R32 E[2];
};

typedef union Sloth_V4 Sloth_V4;
union Sloth_V4
{
  struct {
    Sloth_R32 x;
    Sloth_R32 y;
    Sloth_R32 z;
    Sloth_R32 w;
  };
  struct {
    Sloth_R32 r;
    Sloth_R32 g;
    Sloth_R32 b;
    Sloth_R32 a;
  };
  Sloth_R32 E[4];
};

typedef union Sloth_Rect Sloth_Rect;
union Sloth_Rect
{
  struct {
    Sloth_V2 value_min;
    Sloth_V2 value_max;
  };
};

enum {
  Sloth_Axis_X = 0,
  Sloth_Axis_Y = 1,
  
  // this is useful for doing things like
  // u32 other_axis = (my_axis + 1) % Sloth_Axis_Count
  Sloth_Axis_Count = 2,
};

#define sloth_other_axis(axis) (((axis) + 1) % Sloth_Axis_Count)

// HASHTABLE
// Implementation Details
// - Open Addressing - on collision, the table probes outward looking for
//   a suitable empty slot
// - Robin Hood Hashing - when probing, keys that will probe more times
//   are stored first, increasing lookup speed.
typedef struct Sloth_Hashtable Sloth_Hashtable;
struct Sloth_Hashtable
{
  Sloth_U32* keys;
  Sloth_U8**  values;
  
  // The allocated count of keys and values
  // ie. the size of keys is sizeof(U32) * cap;
  Sloth_U32  cap;
  
  // The total number of registered values
  Sloth_U32  used;
};

// ARENA
// A push buffer arena - only supports push/pop memory and clearing
typedef struct Sloth_Arena Sloth_Arena;
struct Sloth_Arena
{
  char* name;
  Sloth_U8** buckets;
  Sloth_U32 buckets_cap;
  Sloth_U32 buckets_len;
  Sloth_U32 bucket_cap;
  Sloth_U32 curr_bucket_len;
};

typedef struct Sloth_Arena_Loc Sloth_Arena_Loc;
struct Sloth_Arena_Loc
{
  Sloth_U32 bucket_index;
  Sloth_U32 bucket_at; // pos in bucket
};

typedef union Sloth_Glyph_ID Sloth_Glyph_ID;
union Sloth_Glyph_ID
{
  Sloth_U32 value;
  struct {
    Sloth_U8 id[3];
    Sloth_U8 family;
  };
};

typedef struct Sloth_Font_Weight_Family Sloth_Font_Weight_Family;
struct Sloth_Font_Weight_Family
{
  Sloth_U32 weight;
  Sloth_U32 glyph_family;
};

#define SLOTH_FONT_WEIGHTS_CAP 8

typedef struct Sloth_Font_ID Sloth_Font_ID;
struct Sloth_Font_ID
{
  Sloth_U32 value;
  Sloth_U32 weight_index;
};

typedef struct Sloth_Font_Metrics Sloth_Font_Metrics;
struct Sloth_Font_Metrics
{
  Sloth_R32 pixel_height;
  Sloth_R32 line_height;
  Sloth_R32 to_baseline;
};

typedef struct Sloth_Font Sloth_Font;
struct Sloth_Font
{
  char name[32];
  Sloth_U8* renderer_data;
  Sloth_Font_Metrics metrics;
  Sloth_Font_Weight_Family weights[SLOTH_FONT_WEIGHTS_CAP];
  Sloth_U32 weights_len;
};

typedef Sloth_U8*      Sloth_Font_Renderer_Load_Font(Sloth_Ctx* sloth, Sloth_Font* font, Sloth_U8* font_data, Sloth_U32 font_data_size, Sloth_U32 font_index, Sloth_R32 pixel_height);
typedef Sloth_Glyph_ID Sloth_Font_Renderer_Register_Glyph(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 codepoint);

typedef Sloth_U32 Sloth_Glyph_Data_Format;
enum 
{
  Sloth_GlyphData_RGBA8,
  Sloth_GlyphData_RGB8,
  Sloth_GlyphData_Alpha8,
};

static Sloth_U32 sloth_glyph_data_format_strides[] = {
  4, // Sloth_GlyphData_RGBA8
  3, // Sloth_GlyphData_RGB8
  1, // Sloth_GlyphData_Alpha8
};

typedef struct Sloth_Glyph_Desc Sloth_Glyph_Desc;
struct Sloth_Glyph_Desc
{
  Sloth_U32 family;
  Sloth_U32 id;
  Sloth_U8* data;
  
  // Describes how many pixels in the atlas this
  // glyph occupies
  Sloth_U32 src_width;
  Sloth_U32 src_height;
  Sloth_U32 stride;
  
  // Describes how many pixels in the output image
  // this glyph should occupy when rendered at 100%
  // scale.
  //
  // If these are equal to zero, they will be set to 
  // the values of src_Width and src_height respectively
  //
  // NOTE: This is to allow for high dpi font glyphs
  Sloth_U32 dst_width;
  Sloth_U32 dst_height;
  
  Sloth_Glyph_Data_Format format;
  
  Sloth_R32 cursor_to_glyph_start_xoff;
  Sloth_R32 cursor_to_next_glyph;
  Sloth_R32 baseline_offset_y;
  
  Sloth_R32 copy_gamma;
};

typedef struct Sloth_Glyph Sloth_Glyph;
struct Sloth_Glyph
{
  Sloth_U32 src_width;
  Sloth_U32 src_height;
  Sloth_U32 dst_width;
  Sloth_U32 dst_height;
  Sloth_U32 offset_x;
  Sloth_U32 offset_y;
  
  Sloth_R32 lsb;
  Sloth_R32 x_advance;
  Sloth_R32 baseline_offset_y;
};

// The starting dimensions of a newly created Sloth_Glyph_Atlas
#define SLOTH_GLYPH_ATLAS_START_DIM 1024

// The minimum number of Sloth_Glyph_Atlases that can be allocated
// (The starting number is, naturally, zero. And if you never register
//  a glyph, then these will never get allocated)
#define SLOTH_GLYPH_ATLASES_MIN_CAP 4

typedef Sloth_U8 Sloth_Glyph_Atlas_Dirty_State;
enum
{
  Sloth_GlyphAtlas_Clean,
  Sloth_GlyphAtlas_Dirty_UpdateData,
  Sloth_GlyphAtlas_Dirty_Grow,
};

// The in-memory representation of a texture where
// width == height, and the width is a power of 2.
typedef struct Sloth_Glyph_Atlas Sloth_Glyph_Atlas;
struct Sloth_Glyph_Atlas
{
  Sloth_U8* data;
  Sloth_U32 dim;
  Sloth_U32 last_glyph;
  Sloth_U32 last_row_first_glyph;
  Sloth_Glyph_Atlas_Dirty_State dirty_state;
  Sloth_U8  id;
};

typedef struct Sloth_Glyph_Store Sloth_Glyph_Store;
struct Sloth_Glyph_Store
{
  Sloth_Glyph* glyphs;
  Sloth_U32 glyphs_cap;
  Sloth_Hashtable glyphs_table;
};

typedef struct Sloth_Glyph_Info Sloth_Glyph_Info;
struct Sloth_Glyph_Info
{
  Sloth_Glyph glyph;
  Sloth_Rect uv;
};

typedef void Sloth_Renderer_Atlas_Updated(Sloth_Ctx* sloth, Sloth_U32 atlas_index);
typedef void Sloth_Renderer_Render(Sloth_Ctx* sloth, Sloth_U32 glyph_atlas_index);

typedef Sloth_U8 Sloth_Size_Kind;
enum {
  Sloth_SizeKind_None,
  Sloth_SizeKind_Pixels,
  Sloth_SizeKind_TextContent,
  Sloth_SizeKind_PercentOfSelf, // NOTE: not valid for widget width/height fields
  Sloth_SizeKind_PercentOfParent,
  Sloth_SizeKind_ChildrenSum,
  Sloth_SizeKind_Count,
};

char* sloth_size_kind_strs[] = {
  "None",
  "Pixels",
  "TextContent",
  "PercentOfSelf",
  "PercentOfParent",
  "ChildrenSum",
  "Count",
};

typedef struct Sloth_Size Sloth_Size;
struct Sloth_Size
{
  Sloth_R32 value;
  Sloth_Size_Kind kind;
};

typedef union Sloth_Size_Range Sloth_Size_Range;
union Sloth_Size_Range
{
  struct {
    Sloth_Size min;
    Sloth_Size max;
  };
  Sloth_Size E[2];
};

typedef union Sloth_Size_Box Sloth_Size_Box;
union Sloth_Size_Box
{
  struct {
    // @Maintenance
    // left & right correspond to E[Sloth_Axis_X].min and E[Sloth_Axis_X].max
    // top & bottom correspond to E[Sloth_Axis_Y].min and E[Sloth_Axis_Y].max
    Sloth_Size left;
    Sloth_Size right;
    
    Sloth_Size top;    
    Sloth_Size bottom;
  };
  Sloth_Size_Range E[2];
};

typedef Sloth_U8 Sloth_Layout_Direction;
enum {
  Sloth_LayoutDirection_TopDown,
  Sloth_LayoutDirection_BottomUp,
  Sloth_LayoutDirection_LeftToRight,
  Sloth_LayoutDirection_RightToLeft,
};

char* sloth_layout_direction_strs[] = {
  "TopDown",
  "BottomUp",
  "LeftToRight",
  "RightToLeft",
};

typedef Sloth_U8 Sloth_Layout_Position_Kind;
enum {    
  // The parent elements Sloth_Layout_Direction value
  // dictates how this child gets layed out. Subsequent
  // siblings will reference this widget's position to
  // determine their position
  Sloth_LayoutPosition_ParentDecides,
  
  // The widget's margin.top and margin.left are used to 
  // offset this widget from its parents top and left.
  // Subsequent siblings will ignore this widget in their
  // layout passes
  Sloth_LayoutPosition_FixedInParent,
  
  // The widget's margin.top and margin.left are used to 
  // offset this widget from the top and left of the screen
  // Subsequent siblings will ignore this widget in their
  // layout passes
  Sloth_LayoutPosition_FixedOnScreen,
};

char* sloth_layout_position_kind_strs[] = {
  "ParentDecides",
  "FixedInParent",
  "FixedOnScreen",
};

typedef struct Sloth_Layout_Position Sloth_Layout_Position;
struct Sloth_Layout_Position 
{
  Sloth_Layout_Position_Kind kind;
  union {
    Sloth_Size_Box at;
    struct {
      Sloth_Size left;
      Sloth_Size right;
      Sloth_Size top;
      Sloth_Size bottom;
    };
  };
  Sloth_R32 z;
};

typedef struct Sloth_Widget_Layout Sloth_Widget_Layout;
struct Sloth_Widget_Layout
{
  union {
    struct {
      Sloth_Size width;
      Sloth_Size height;
    };
    Sloth_Size size[2];
  };
  
  Sloth_Size_Box margin;
  
  Sloth_Layout_Direction direction;
  Sloth_Layout_Position position;
};

typedef Sloth_U8 Sloth_Draw_Flags;
enum
{
  Sloth_Draw_Default = 0,
  Sloth_Draw_None    = 1,
};

typedef Sloth_U32 Sloth_Text_Style_Flags;
enum 
{
  Sloth_TextStyle_Defaults      = 0,
  
  // Alignment
  Sloth_TextStyle_Align_Left    = 1,
  Sloth_TextStyle_Align_Center  = 2,
  Sloth_TextStyle_Align_Right   = 4,
  
  // Wrapping
  // default is to wrap
  Sloth_TextStyle_NoWrapText = 8,
};

char* sloth_text_style_flag_strs[] = {
  "Defaults",
  "Align_Left",
  "Align_Center",
  "Align_Right",
  "NoWrapText",
};

typedef struct Sloth_Widget_Style Sloth_Widget_Style;
struct Sloth_Widget_Style
{
  Sloth_U8  draw_flags;
  Sloth_U32 color_bg;
  Sloth_U32 color_text;
  Sloth_U32 color_outline;
  
  Sloth_Glyph_ID bg_glyph;
  
  Sloth_R32 outline_thickness;
  
  // Top Left, Top Right, Bottom Right, Bottom Left
  Sloth_Size border_radius[4];
  
  Sloth_Text_Style_Flags text_style;
  Sloth_Font_ID font;
};

typedef Sloth_U32 Sloth_Widget_Input_Flags;
enum
{
  Sloth_WidgetInput_None = 0,
  Sloth_WidgetInput_Draggable = 1,
  Sloth_WidgetInput_TextSelectable = 2,
  Sloth_WidgetInput_TextCursor = 4,
  Sloth_WidgetInput_DoNotCaptureMouse = 8,
};

typedef struct Sloth_Widget_Input Sloth_Widget_Input;
struct Sloth_Widget_Input
{
  Sloth_Widget_Input_Flags flags;
};

typedef struct Sloth_Widget_Desc Sloth_Widget_Desc;
struct Sloth_Widget_Desc
{
  Sloth_Widget_Layout layout;
  Sloth_Widget_Style  style;
  Sloth_Widget_Input  input;
};

typedef struct Sloth_Glyph_Layout Sloth_Glyph_Layout;
struct Sloth_Glyph_Layout
{
  Sloth_Glyph_ID glyph_id;
  Sloth_Glyph_Info info;
  Sloth_Rect bounds;
  Sloth_U32 color;
  
  // TODO: Hanging bytes, could be much better used space
  // will take care of later
  Sloth_U8 draw; 
  Sloth_U8 is_line_start;
  Sloth_U8 selected;
};

typedef struct Sloth_Widget_Cached Sloth_Widget_Cached;
struct Sloth_Widget_Cached
{
  Sloth_U8 canary_start_;
  
  // Effective Desc
  // Fields that are used during interaction
  // These are cached from the previous frame
  Sloth_V2 offset; // offset from parent origin
  Sloth_V2 dim;    // pixel dimensions
  Sloth_Rect bounds;
  
  // cumulative bounds of all this widgets children
  Sloth_V2 children_bounds_min;
  Sloth_V2 children_bounds_max; 
  
  Sloth_V2 child_offset;
  
  // Cached Values
#define SLOTH_WIDGET_PERSISTENT_VALUE_CAP 32
  Sloth_U8  cached_value[SLOTH_WIDGET_PERSISTENT_VALUE_CAP];
  Sloth_U32 cached_value_len;
  
  // only used if in free list
  Sloth_Widget_Cached* free_next;
  
  Sloth_U8 canary_end_;
};

typedef struct Sloth_Widget Sloth_Widget;
struct Sloth_Widget
{
  // Tree Structuring
  Sloth_Widget* child_first;
  Sloth_Widget* child_last;
  Sloth_Widget* sibling_next;
  Sloth_Widget* sibling_prev;
  Sloth_Widget* parent;
  
  Sloth_ID id;
  Sloth_U32 touched_last_frame;
  
  Sloth_Widget_Cached* cached;
  
  // Primed Desc
  // Fields that describe how to render the 
  // widget, and which will be used for interaction
  // next frame
  Sloth_Widget_Layout layout;
  Sloth_Widget_Style  style;
  Sloth_Widget_Input  input;
  
  Sloth_Glyph_Layout* text;
  Sloth_U32           text_cap;
  Sloth_U32           text_len;
  Sloth_V2            text_dim;
};

typedef struct Sloth_Widget_Pool Sloth_Widget_Pool;
struct Sloth_Widget_Pool
{
  Sloth_Widget* values;
  Sloth_U32 cap;
  Sloth_U32 len;
  
  Sloth_Widget* free_list;
};

typedef struct Sloth_Widget_Cached_Pool Sloth_Widget_Cached_Pool;
struct Sloth_Widget_Cached_Pool
{
  Sloth_Widget_Cached** buckets;
  Sloth_U32 buckets_cap;
  Sloth_U32 buckets_len;
  Sloth_U32 bucket_at;
  Sloth_U32 bucket_at_len;
  Sloth_U32 bucket_cap;
  
  Sloth_Widget_Cached* free_list;
};

typedef struct Sloth_Widget_Result Sloth_Widget_Result;
struct Sloth_Widget_Result
{
  Sloth_Widget* widget;
  
  // mouse input
  Sloth_U8 released;
  Sloth_U8 clicked;
  Sloth_U8 held;
  Sloth_V2 drag_offset_pixels;
  Sloth_V2 drag_offset_percent_parent;
  
  // text input
  Sloth_U32 selected_glyphs_first;
  Sloth_U32 selected_glyphs_one_past_last;
  Sloth_U32 glyph_cursor_pos;
};

#define SLOTH_VERTEX_STRIDE 9

typedef struct Sloth_VIBuffer Sloth_VIBuffer;
struct Sloth_VIBuffer
{
  Sloth_R32* verts;
  Sloth_U32  verts_cap;
  Sloth_U32  verts_len;
  
  Sloth_U32* indices;
  Sloth_U32  indices_cap;
  Sloth_U32  indices_len;
};

enum {
  SLOTH_DEBUG_DID_CALL_ADVANCE,
  SLOTH_DEBUG_DID_CALL_PREPARE,
};

typedef Sloth_U8 Sloth_Mouse_State;
enum {
  Sloth_MouseState_None = 0,
  Sloth_MouseState_IsDown = 1,
  Sloth_MouseState_WasDown = 2,
};

// Sloth_Frame_Desc
// gets passed in to sloth_frame_prepare and contains
typedef struct Sloth_Frame_Desc Sloth_Frame_Desc;
struct Sloth_Frame_Desc
{
  Sloth_V2 screen_dim;
  Sloth_V2 mouse_pos;
  Sloth_Mouse_State mouse_button_l;
  Sloth_Mouse_State mouse_button_r;
  Sloth_U8 high_dpi;
  Sloth_R32 dpi_scale;
};

typedef void Sloth_Render_VIBuffer(Sloth_Ctx* sloth, Sloth_VIBuffer buffer);

struct Sloth_Ctx
{
  Sloth_Arena per_frame_memory;
  Sloth_Arena scratch;
  
  Sloth_Widget_Pool widgets;
  Sloth_Widget_Cached_Pool widget_caches;
  Sloth_Hashtable widget_cache_lut;
  
  // Fonts
  Sloth_U8* font_renderer_data;
  Sloth_Font_Renderer_Load_Font* font_renderer_load_font;
  Sloth_Font_Renderer_Register_Glyph* font_renderer_register_glyph;  
  Sloth_Font* fonts;
  Sloth_U32   fonts_cap;
  Sloth_U32   fonts_len;
  Sloth_Font_ID font_active;
  
  // the actual root of the current tree
  Sloth_Widget* widget_tree_root;
  Sloth_Widget* widget_tree_parent_cur;
  
  // what the tree expects the next child will
  // be, based on the tree layout cached from the 
  // previous frame. 
  Sloth_Widget* widget_tree_next_child;
  Sloth_U32 widget_tree_depth_cur;
  Sloth_U32 widget_tree_depth_max;
  
  // Glyphs & Fonts
  Sloth_Glyph_Atlas* glyph_atlases;
  Sloth_VIBuffer*    vibuffers;
  Sloth_U32          glyph_atlases_cap;
  
  Sloth_Glyph_Store  glyph_store;
  
  Sloth_Font_ID      active_text_glyph_family;
  Sloth_Renderer_Atlas_Updated* renderer_atlas_updated;
  
  // Renderer Data
  // The geometry to render this frame
  Sloth_VIBuffer vibuf_;
  Sloth_U8* render_data;
  Sloth_R32 z_depth_min;
  Sloth_R32 z_depth_max;
  Sloth_Renderer_Render* renderer_render;
  
  // Input Tracking
  Sloth_V2 screen_dim;
  Sloth_R32 screen_dpi_scale;
  Sloth_V2 mouse_pos;
  Sloth_Mouse_State mouse_button_l;
  Sloth_Mouse_State mouse_button_r;
  Sloth_V2 mouse_down_pos;
  
  // 
  Sloth_ID last_active_widget;
  Sloth_ID hot_widget;
  Sloth_U8 hot_widget_age;
  Sloth_ID active_widget;
  Sloth_U8 active_widget_age;
  Sloth_U32 active_widget_selected_glyphs_first;
  Sloth_U32 active_widget_selected_glyphs_one_past_last;
  
  // Debug checks
  Sloth_U32 sentinel;
};

//////// INTERFACE  ////////

// Sloth Id Generation
// An Id will be constructed by hashing the string provided
// If ## appears in the input string, everything before the ##
// will be returned as a display string;
// If ### appears in the input string, the id will be constructed
// only of everything after the ###.
Sloth_Function Sloth_ID_Result sloth_make_id_v(Sloth_Char* fmt, va_list args);
Sloth_Function Sloth_ID_Result sloth_make_id_f(Sloth_Char* fmt, ...);
Sloth_Function Sloth_ID_Result sloth_make_id_len(Sloth_U32 len, Sloth_Char* str);
Sloth_Function Sloth_ID_Result sloth_make_id(Sloth_Char* str);
Sloth_Function Sloth_Bool      sloth_ids_equal(Sloth_ID a, Sloth_ID b);

// Sloth Vector and Rect
Sloth_Function Sloth_V2 sloth_make_v2(Sloth_R32 x, Sloth_R32 y);
Sloth_Function Sloth_V2 sloth_v2_add(Sloth_V2 a, Sloth_V2 b);
Sloth_Function Sloth_V2 sloth_v2_sub(Sloth_V2 a, Sloth_V2 b);
Sloth_Function Sloth_V2 sloth_v2_mulf(Sloth_V2 a, Sloth_R32 b);

#define SLOTH_SIZE(v, k) (Sloth_Size){ (v), (k) }
#define SLOTH_SIZE_CHILDREN_SUM         SLOTH_SIZE(0, Sloth_SizeKind_ChildrenSum)
#define SLOTH_SIZE_PERCENT_OF_PARENT(v) SLOTH_SIZE((v), Sloth_SizeKind_PercentOfParent)
#define SLOTH_SIZE_TEXT_CONTENT         SLOTH_SIZE(0, Sloth_SizeKind_TextContent)
#define SLOTH_SIZE_PIXELS(v)            SLOTH_SIZE((v), Sloth_SizeKind_Pixels)

Sloth_Function Sloth_Rect sloth_rect_union(Sloth_Rect a, Sloth_Rect b);
Sloth_Function void       sloth_rect_expand(Sloth_Rect* target, Sloth_R32 left, Sloth_R32 top, Sloth_R32 right, Sloth_R32 bottom);
Sloth_Function Sloth_V2   sloth_rect_dim(Sloth_Rect r);
Sloth_Function Sloth_Bool sloth_rect_contains(Sloth_Rect r, Sloth_V2 p);
Sloth_Function Sloth_V2   sloth_rect_get_closest_point(Sloth_Rect r, Sloth_V2 p);
Sloth_Function Sloth_Bool sloth_clip_rect_and_uv(Sloth_Rect clip, Sloth_Rect bounds, Sloth_Rect uv, Sloth_Rect* bounds_clipped, Sloth_Rect* uv_clipped);

Sloth_Function Sloth_Size_Box sloth_size_box_uniform(Sloth_Size_Kind k, Sloth_R32 v);
Sloth_Function Sloth_Size_Box sloth_size_box_uniform_pixels(Sloth_R32 v);

// Color
Sloth_Function Sloth_U32 sloth_color_apply_gamma(Sloth_U32 color, Sloth_R32 power);

// Hashtable Functions
Sloth_Function void       sloth_hashtable_add(Sloth_Hashtable* table, Sloth_U32 key, Sloth_U8* value);
Sloth_Function Sloth_Bool sloth_hashtable_rem(Sloth_Hashtable* table, Sloth_U32 key);
Sloth_Function Sloth_U8*  sloth_hashtable_get(Sloth_Hashtable* table, Sloth_U32 key);
Sloth_Function void       sloth_hashtable_free(Sloth_Hashtable* table);

// Arena Functions
Sloth_Function void      sloth_arena_grow(Sloth_Arena* arena, Sloth_U32 min_size);

// convenience function that auto casts to a type size
#define sloth_arena_push_array(arena, type, count) (type*)sloth_arena_push((arena), sizeof(type) * (count))
Sloth_Function Sloth_U8* sloth_arena_push(Sloth_Arena* arena, Sloth_U32 size);

Sloth_Function void            sloth_arena_pop(Sloth_Arena* arena, Sloth_Arena_Loc to);
Sloth_Function Sloth_Arena_Loc sloth_arena_at(Sloth_Arena* arena);
Sloth_Function void            sloth_arena_clear(Sloth_Arena* arena);
Sloth_Function void            sloth_arena_free(Sloth_Arena* arena);

// Fonts
Sloth_Function Sloth_Font_ID  sloth_font_load(Sloth_Ctx* sloth, char* path, Sloth_R32 pixel_height);
Sloth_Function Sloth_Font_ID  sloth_font_load_from_memory(Sloth_Ctx* sloth, char* font_name, Sloth_U32 font_name_len, Sloth_U8* data, Sloth_U32 data_size, Sloth_R32 pixel_height);
Sloth_Function Sloth_Font_ID  sloth_font_register_family(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 weight, Sloth_U32 family);
Sloth_Function Sloth_Glyph_ID sloth_font_register_codepoint(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 codepoint);
Sloth_Function Sloth_Font*    sloth_glyph_to_font(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph_id);
Sloth_Function void           sloth_font_set_metrics(Sloth_Ctx* sloth, Sloth_Font_ID id, Sloth_Font_Metrics metrics);

//////////////////////////////////////////////
// Glyphs
//////////////////////////////////////////////

// Glyph IDs
Sloth_Function Sloth_Glyph_ID sloth_make_glyph_id(Sloth_U32 family, Sloth_U32 id);
Sloth_Function Sloth_Bool           sloth_glyph_id_matches_charcode(Sloth_Glyph_ID id, Sloth_U32 charcode);

// Glyph Atlas
Sloth_Function void sloth_glyph_atlas_resize(Sloth_Glyph_Atlas* atlas, Sloth_U32 new_dim);
Sloth_Function Sloth_Glyph_Atlas* sloth_create_atlas(Sloth_Ctx* sloth, Sloth_U8 family, Sloth_U32 min_dim);
// Glyph Store
Sloth_Function Sloth_Bool sloth_glyph_store_contains(Sloth_Glyph_Store* store, Sloth_Glyph_ID id);
Sloth_Function void sloth_glyph_store_free(Sloth_Glyph_Store* store);

// Glyph Registration
Sloth_Function Sloth_Glyph_Atlas* sloth_get_atlas_for_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph);
Sloth_Function Sloth_VIBuffer*    sloth_get_vibuffer_for_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph);
Sloth_Function Sloth_Glyph_ID     sloth_register_glyph(Sloth_Ctx* sloth, Sloth_Glyph_Desc desc);
Sloth_Function void               sloth_unregister_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID id);
Sloth_Function Sloth_Glyph_Info   sloth_lookup_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID id);

//////////////////////////////////////////////
// Input
//////////////////////////////////////////////

Sloth_Function Sloth_Bool sloth_mouse_button_transitioned_down(Sloth_Mouse_State btn);
Sloth_Function Sloth_Bool sloth_mouse_button_transitioned_up  (Sloth_Mouse_State btn);
Sloth_Function Sloth_Bool sloth_mouse_button_held_down (Sloth_Mouse_State btn);
Sloth_Function Sloth_Bool sloth_mouse_button_held_up   (Sloth_Mouse_State btn);

//
// Widget Functions
//

// Widget Data Caching
// Each widget is allowed to cache one block of memory that is associated with 
// its id. this allows for custom functionality that relies on persistent state
// while retaining (pun intended) its immediate mode interface.
// This cached data gets purged when the system realizes that a particular id is
// no longer being rendered.
//
// The cached data getter allows you to pass a fallback to use in the event that
// the cached version is not created yet. This fallback value can change frame to
// frame and it will not affect the value returned from the function.
Sloth_Function Sloth_U8* sloth_persistent_value_get_(Sloth_Ctx* sloth, Sloth_ID id, Sloth_U32 size, Sloth_U8* fallback);
Sloth_Function void      sloth_persistent_value_set_(Sloth_Ctx* sloth, Sloth_ID id, Sloth_U32 size, Sloth_U8* value);
#define sloth_persistent_value_get(sloth, id, type, fallback_ptr) (type*)sloth_persistent_value_get_((sloth), (id), sizeof(type), (Sloth_U8*)(fallback_ptr))
#define sloth_persistent_value_set(sloth, id, type, value_ptr) sloth_persistent_value_set_((sloth), (id), sizeof(type), (Sloth_U8*)(value_ptr))

//
// Widget Pool Functions
//
Sloth_Function Sloth_Widget* sloth_widget_pool_take(Sloth_Ctx* sloth);
Sloth_Function void          sloth_widget_pool_give(Sloth_Ctx* sloth, Sloth_Widget* widget);
Sloth_Function void          sloth_widget_pool_grow(Sloth_Widget_Pool* pool);
Sloth_Function void          sloth_widget_pool_free(Sloth_Widget_Pool* pool);

//
// Widget Cached Pool
//
Sloth_Function Sloth_Widget_Cached* sloth_widget_cached_pool_take(Sloth_Ctx* sloth);
Sloth_Function void                 sloth_widget_cached_pool_give(Sloth_Ctx* sloth, Sloth_Widget_Cached* widget);
Sloth_Function void                 sloth_widget_cached_pool_grow(Sloth_Widget_Cached_Pool* pool);
Sloth_Function void                 sloth_widget_cached_pool_free(Sloth_Widget_Cached_Pool* pool);
Sloth_Function Sloth_Widget_Cached* sloth_get_cached_data_for_id(Sloth_Ctx* sloth, Sloth_ID id);

//
// Widget Tree Functions
//

typedef Sloth_U32 Sloth_Tree_Walk_Result;
enum {
  Sloth_TreeWalk_Default = 0, // 
  
  Sloth_TreeWalk_Continue,
  
  // Only works in preorder walks
  Sloth_TreeWalk_Continue_SkipChildren,
  
  Sloth_TreeWalk_Stop,
};

// Construction
Sloth_Function Sloth_Widget* sloth_push_widget_on_tree(Sloth_Ctx* sloth, Sloth_ID id);
Sloth_Function Sloth_Widget* sloth_pop_widget_off_tree(Sloth_Ctx* sloth);

// Walking
typedef Sloth_Tree_Walk_Result Sloth_Tree_Walk_Cb(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_inorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_preorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_postorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_inorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_preorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);
Sloth_Function void sloth_tree_walk_postorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data);

//
// Widget Operations
//
Sloth_Function Sloth_Widget_Result sloth_push_widget_v(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, va_list args);
Sloth_Function Sloth_Widget_Result sloth_push_widget_f(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...);
Sloth_Function Sloth_Widget_Result sloth_push_widget(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* text);
Sloth_Function void                sloth_pop_widget(Sloth_Ctx* sloth);
Sloth_Function void                sloth_pop_widget_safe(Sloth_Ctx* sloth, Sloth_Widget_Result result);

// sloth_widget_f and sloth_widget both push and pop the widget, wrapping its entire lifetime into a single call
Sloth_Function Sloth_Widget_Result sloth_widget_f(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...);
Sloth_Function Sloth_Widget_Result sloth_widget(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* text);

//
// VIBuffer Operations
//
Sloth_Function void      sloth_vibuffer_set_vert(Sloth_VIBuffer* buf, Sloth_U32 vert_index, Sloth_R32 x, Sloth_R32 y, Sloth_R32 z, Sloth_R32 u, Sloth_R32 v, Sloth_V4 c);
Sloth_Function Sloth_U32 sloth_vibuffer_push_vert(Sloth_VIBuffer* buf, Sloth_R32 x, Sloth_R32 y, Sloth_R32 z, Sloth_R32 u, Sloth_R32 v, Sloth_V4 c);
Sloth_Function Sloth_U32 sloth_vibuffer_push_tri(Sloth_VIBuffer* buf, Sloth_U32 a, Sloth_U32 b, Sloth_U32 c);
Sloth_Function void      sloth_vibuffer_push_quad(Sloth_VIBuffer* buf, Sloth_U32 a, Sloth_U32 b, Sloth_U32 c, Sloth_U32 d);
Sloth_Function void      sloth_vibuffer_reset(Sloth_VIBuffer* buf);
Sloth_Function void      sloth_vibuffer_free(Sloth_VIBuffer* buf);

//
// Ctx Operations
//
Sloth_Function void sloth_ctx_init(Sloth_Ctx* ctx);
Sloth_Function void sloth_ctx_activate_glyph_family(Sloth_Ctx* sloth, Sloth_U32 family);
Sloth_Function void sloth_ctx_free(Sloth_Ctx* sloth);
Sloth_Function void sloth_frame_prepare(Sloth_Ctx* sloth, Sloth_Frame_Desc desc);
Sloth_Function void sloth_frame_advance(Sloth_Ctx* sloth);
Sloth_Function void sloth_frame_render(Sloth_Ctx* sloth);

// TODO
// - Convenience functions
//    - load_sprite
//    - load_font

#endif // SLOTH_INTERFACE

//////// IMPLEMENTATION  ////////
#ifdef SLOTH_IMPLEMENTATION

#ifndef sloth_realloc

Sloth_Function Sloth_U8*
sloth_realloc_wrapper(Sloth_U8* base, Sloth_U32 old_size, Sloth_U32 new_size)
{
  if (new_size == 0) {
    free(base);
    return 0;
  } else {
    return (Sloth_U8*)realloc(base, new_size);
  }
}
#  define sloth_realloc(base, old_size, new_size) sloth_realloc_wrapper((Sloth_U8*)(base), (old_size), (new_size))

#endif

#define sloth_realloc_array(base, type, old_count, new_count) (type*)sloth_realloc((base), sizeof(type) * (old_count), sizeof(type) * (new_count))

#ifndef sloth_free
#  define sloth_free(base, size) free(base)
#endif

#ifndef sloth_assert
#  ifdef DEBUG
#    define sloth_assert(c) if (!(c)) { do{ *((volatile int*)0) = 0xFFFF; }while(0); }
#  else
#    define sloth_assert(c)
#  endif
#endif

#define sloth_invalid_code_path sloth_assert(false)
#ifdef DEBUG
#  define sloth_invalid_default_case default: { sloth_invalid_code_path; } break;
#else
#  define sloth_invalid_default_case default: {} break;
#endif

// we prefer the stbsp implementation of vsnprintf if its present
// otherwise fall back on the c standard libraries implementation
#ifndef sloth_vsnprintf
#  if defined(STB_SPRINTF_H_INCLUDE) && !defined(SLOTH_USE_STDLIB_VSNPRINTF)
#    define sloth_vsnprintf(...) stbsp_vsnprintf(__VA_ARGS__)
#  else
#    include <stdio.h>
#    define sloth_vsnprintf(...) vsnprintf(__VA_ARGS__)
#  endif
#endif

// Creates a universal way to zero a struct across C and CPP
#if defined(__cplusplus)
#  define SLOTH_ZII {}
#else
#  define SLOTH_ZII {0}
#endif

Sloth_Function Sloth_U32
sloth_snprintf(char* dst, Sloth_U32 dst_len, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_U32 result = (Sloth_U32)sloth_vsnprintf(dst, dst_len, fmt, args);
  va_end(args);
  return result;
}


#define sloth_array_grow(base, len, cap, min_cap, ele_type) (ele_type*)sloth_array_grow_((Sloth_U8*)(base), (len), (cap), (min_cap), sizeof(ele_type))
Sloth_Function Sloth_U8*
sloth_array_grow_(Sloth_U8* base, Sloth_U32 len, Sloth_U32* cap, Sloth_U32 min_cap, Sloth_U32 ele_size)
{
  SLOTH_PROFILE_BEGIN;
  if (len < *cap) return base;
  
  Sloth_U32 new_cap = *cap * 2;
  if (new_cap == 0) new_cap = min_cap;
  
  Sloth_U8* new_base = sloth_realloc(
      base,
    *cap * ele_size,
    new_cap * ele_size
  );
  *cap = new_cap;
  
  return new_base;
}


// Temporary string memory
// TODO: Store this in the Sloth_Ctx rather than as a global
static Sloth_U8 sloth_temp_string_memory[Sloth_Temp_String_Memory_Size];

#define Sloth_Max(a,b) ((a) > (b) ? (a) : (b))
#define Sloth_Min(a,b) ((a) < (b) ? (a) : (b))
#define Sloth_Clamp(lower, v, higher) Sloth_Max(lower, Sloth_Min(higher, v))

Sloth_Function Sloth_R32
sloth_lerp_r32(Sloth_R32 a, Sloth_R32 t, Sloth_R32 b)
{
  return a + (t * b);
}

Sloth_Function Sloth_V2
sloth_lerp_v2(Sloth_V2 a, Sloth_R32 t, Sloth_V2 b)
{
  Sloth_V2 r;
  r.x = sloth_lerp_r32(a.x, t, b.x);
  r.y = sloth_lerp_r32(a.y, t, b.y);
  return r;
}

Sloth_Function Sloth_R32
sloth_remap_r32(Sloth_R32 v, Sloth_R32 old_min, Sloth_R32 old_max, Sloth_R32 new_min, Sloth_R32 new_max)
{
  Sloth_R32 pct = (v - old_min) / (old_max - old_min);
  Sloth_R32 result = (pct * (new_max - new_min)) + new_min;
  return result;
}

Sloth_Function Sloth_V2
sloth_remap_v2(Sloth_V2 v, Sloth_V2 old_min, Sloth_V2 old_max, Sloth_V2 new_min, Sloth_V2 new_max)
{
  Sloth_V2 result;
  result.x = sloth_remap_r32(v.x, old_min.x, old_max.x, new_min.x, new_max.x);
  result.y = sloth_remap_r32(v.y, old_min.y, old_max.y, new_min.y, new_max.y);
  return result;
}

#define sloth_has_flag(value, flag) (((value) & (flag)) != 0)
#define sloth_floor_r32(v) (Sloth_R32)((int)(v))

#define sloth_copy_memory(dst, src, len) sloth_copy_memory_((Sloth_U8*)(dst), (Sloth_U8*)(src), (len))
Sloth_Function void
sloth_copy_memory_(Sloth_U8* dst, Sloth_U8* src, Sloth_U32 len)
{
  SLOTH_PROFILE_BEGIN;
  for (Sloth_U32 i = 0; i < len; i++) dst[i] = src[i];
}

#define sloth_is_pow2(a) (!((Sloth_U32)(a) & ((Sloth_U32)(a) - 1)))

Sloth_Function Sloth_U32
sloth_round_to_pow2_u32(Sloth_U32 v)
{
  Sloth_U32 r = v - 1;
  r |= r >> 1;
  r |= r >> 2; 
  r |= r >> 4;
  r |= r >> 8;
  r |= r >> 16; 
  r = r + 1;
  return r;
}

Sloth_Function Sloth_ID_Result 
sloth_make_id_v(Sloth_Char* fmt, va_list args)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U32 len = (Sloth_U32)sloth_vsnprintf((char*)sloth_temp_string_memory, 
    Sloth_Temp_String_Memory_Size,
    (char*)fmt,
    args);
  
  // Break up formatted string into its parts
  // (what to display, what to discard)
  Sloth_U32 discard_to = 0;
  Sloth_U32 display_before = len;
  for (Sloth_U32 i = 0; i < len; i++) 
  {
    if (sloth_temp_string_memory[i] == '#') {
      if (i + 1 < len && sloth_temp_string_memory[i + 1] == '#')
      {
        if (i + 2 < len && sloth_temp_string_memory[i + 2] == '#' && (i + 3) > discard_to)
        {
          discard_to = i + 3;
          display_before = i;
          i += 2;
        }
        else if (i < display_before)
        {
          display_before = i;
        }
      }
    }
  }
  
  // Hash the non-discarded formatted string
  // djb2 hash - http://www.cse.yorku.ca/~oz/hash.html
  Sloth_U32 hash = 5381;
  for (Sloth_U32 i = discard_to; i < len; i++)
  {
    hash = ((hash << 5) + hash) + (Sloth_U8)sloth_temp_string_memory[i];
  }
  
  Sloth_ID_Result result;
  result.id.value = hash;
  result.display_len = display_before;
  result.formatted = (Sloth_Char*)&sloth_temp_string_memory[0];
  return result;
}

Sloth_Function Sloth_ID_Result 
sloth_make_id_f(Sloth_Char* fmt, ...)
{
  SLOTH_PROFILE_BEGIN;
  va_list args;
  va_start(args, fmt);
  Sloth_ID_Result result = sloth_make_id_v(fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_ID_Result 
sloth_make_id_len(Sloth_U32 len, Sloth_Char* str)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_ID_Result result = sloth_make_id_f((char*)"%.*s", len, str);
  return result;
}

Sloth_Function Sloth_ID_Result 
sloth_make_id(Sloth_Char* str)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_ID_Result result = sloth_make_id_f((char*)"%s", str);
  return result;
}

Sloth_Function Sloth_Bool
sloth_ids_equal(Sloth_ID a, Sloth_ID b)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Bool result = a.value == b.value;
  return result;
}

Sloth_Function Sloth_V2
sloth_make_v2(Sloth_R32 x, Sloth_R32 y)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result;
  result.x = x;
  result.y = y;
  return result;
}

Sloth_Function Sloth_V2 
sloth_v2_add(Sloth_V2 a, Sloth_V2 b)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

Sloth_Function Sloth_V2 
sloth_v2_sub(Sloth_V2 a, Sloth_V2 b)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result;
  result.x = a.x - b.x;
  result.y = a.y - b.y;
  return result;
}

Sloth_Function Sloth_V2 
sloth_v2_mulf(Sloth_V2 a, Sloth_R32 b)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result;
  result.x = a.x * b;
  result.y = a.y * b;
  return result;
}

Sloth_Function Sloth_V2   
sloth_rect_dim(Sloth_Rect r)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result;
  result.x = r.value_max.x - r.value_min.x;
  result.y = r.value_max.y - r.value_min.y;
  return result;
}

Sloth_Function Sloth_Rect 
sloth_rect_union(Sloth_Rect a, Sloth_Rect b)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Rect result;
  result.value_min.x = Sloth_Max(a.value_min.x, b.value_min.x);
  result.value_min.y = Sloth_Max(a.value_min.y, b.value_min.y);
  result.value_max.x = Sloth_Min(a.value_max.x, b.value_max.x);
  result.value_max.y = Sloth_Min(a.value_max.y, b.value_max.y);
  Sloth_V2 result_dim = sloth_rect_dim(result);
  if (result_dim.x < 0 || result_dim.y < 0) {
    result.value_min.x = 0; result.value_min.y = 0;
    result.value_max.x = 0; result.value_max.y = 0;
  }
  return result;
}

Sloth_Function void
sloth_rect_expand(Sloth_Rect* target, Sloth_R32 left, Sloth_R32 top, Sloth_R32 right, Sloth_R32 bottom)
{
  SLOTH_PROFILE_BEGIN;
  target->value_min.x -= left;
  target->value_min.y -= top;
  target->value_max.x += right;
  target->value_max.y += bottom;
}

Sloth_Function Sloth_Bool 
sloth_rect_contains(Sloth_Rect r, Sloth_V2 p)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Bool result = (
      p.x >= r.value_min.x && p.x <= r.value_max.x &&
      p.y >= r.value_min.y && p.y <= r.value_max.y
  );
  return result;
}

Sloth_Function Sloth_V2 
sloth_rect_get_closest_point(Sloth_Rect r, Sloth_V2 p)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 result = p;
  result.x = Sloth_Max(r.value_min.x, Sloth_Min(r.value_max.x, p.x));
  result.y = Sloth_Max(r.value_min.y, Sloth_Min(r.value_max.y, p.y));
  return result;
}

// returns true if the resulting bounds have non-zero area
// ie. if the rect would be visible if rendered
Sloth_Function Sloth_Bool
sloth_clip_rect_and_uv(Sloth_Rect clip, Sloth_Rect bounds, Sloth_Rect uv, Sloth_Rect* bounds_clipped, Sloth_Rect* uv_clipped)
{
  bounds_clipped->value_min = sloth_rect_get_closest_point(clip, bounds.value_min);
  bounds_clipped->value_max = sloth_rect_get_closest_point(clip, bounds.value_max);
  
  uv_clipped->value_min = sloth_remap_v2(bounds_clipped->value_min,
    bounds.value_min, bounds.value_max,
    uv.value_min, uv.value_max);
  uv_clipped->value_max = sloth_remap_v2(bounds_clipped->value_max,
    bounds.value_min, bounds.value_max,
    uv.value_min, uv.value_max);
  
  Sloth_V2 dim = sloth_rect_dim(*bounds_clipped);
  return dim.x > 0 && dim.y > 0;
}

Sloth_Function Sloth_Size_Box 
sloth_size_box_uniform(Sloth_Size_Kind k, Sloth_R32 v)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Size_Box result;
  result.left = SLOTH_SIZE(k, v);
  result.right = result.left;
  result.top = result.left;
  result.bottom = result.left;
  return result;
}

Sloth_Function Sloth_Size_Box 
sloth_size_box_uniform_pixels(Sloth_R32 v)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_size_box_uniform(Sloth_SizeKind_Pixels, v);
}

Sloth_Function Sloth_Layout_Position
sloth_position_fixed_in_parent_px(Sloth_R32 left, Sloth_R32 top)
{
  Sloth_Layout_Position result;
  result.kind = Sloth_LayoutPosition_FixedInParent;
  result.left = SLOTH_SIZE_PIXELS(left);
  result.top  = SLOTH_SIZE_PIXELS(top);
  return result;
}

Sloth_Function Sloth_Layout_Position
sloth_position_fixed_on_screen_px(Sloth_R32 left, Sloth_R32 top)
{
  Sloth_Layout_Position result;
  result.kind = Sloth_LayoutPosition_FixedOnScreen;
  result.left = SLOTH_SIZE_PIXELS(left);
  result.top  = SLOTH_SIZE_PIXELS(top);
  return result;
}

#define sloth_zero_struct_(ptr) sloth_zero_size__(sizeof(*ptr), (Sloth_U8*)(ptr));
Sloth_Function void
sloth_zero_size__(Sloth_U32 size, Sloth_U8* base)
{
  for (Sloth_U32 i = 0; i < size; i++) base[i] = 0;
}

Sloth_Function void
sloth_hashtable_realloc(Sloth_Hashtable* table, Sloth_U32 old_cap, Sloth_U32 new_cap)
{
  table->keys = sloth_realloc_array(table->keys, Sloth_U32, old_cap, new_cap);
  table->values = (Sloth_U8**)sloth_realloc_array(
      table->values, Sloth_U8*, old_cap, new_cap
  );
  
  Sloth_U32 indices_to_zero = new_cap - old_cap;
  sloth_zero_size__(
      sizeof(Sloth_U32) * indices_to_zero, 
    (Sloth_U8*)(table->keys + old_cap)
  );
  sloth_zero_size__(
      sizeof(Sloth_U8*) * indices_to_zero, 
    (Sloth_U8*)(table->values + old_cap)
  );
  
  table->cap = new_cap;
}

#define SLOTH_HASHTABLE_TOMBSTONE (1 << 31)
// Since this hashtable takes any 32 bit integer as a key, this 
// macro simply masks off the tombstone bit if it was present
#define SLOTH_HASHTABLE_VALIDATE_KEY(key) ((key) & ~SLOTH_HASHTABLE_TOMBSTONE)

#define SLOTH_HASHTABLE_CAP_MASK(table) ((table)->cap - 1)

// effectively key % table->cap
// this will be true so long as cap is a power of two
Sloth_Function Sloth_U32
sloth_hashtable_desired_pos(Sloth_Hashtable* table, Sloth_U32 key)
{
  Sloth_U32 result = key & SLOTH_HASHTABLE_CAP_MASK(table); 
  if (result == 0) result = 1;
  return result;
}

#define SLOTH_HASHTABLE_PROBE_DISTANCE(table, key, pos) (pos) - (sloth_hashtable_desired_pos((table), (key)))

#define SLOTH_HASHTABLE_KEY_IS_DELETED(key) ((key) & SLOTH_HASHTABLE_TOMBSTONE)

Sloth_Function void
sloth_hashtable_insert_(Sloth_Hashtable* table, Sloth_U32 key, Sloth_U8* value, Sloth_U32 index)
{
  table->keys[index] = key;
  table->values[index] = value;
}

Sloth_Function void
sloth_hashtable_add(Sloth_Hashtable* table, Sloth_U32 key, Sloth_U8* value)
{
  if (table->cap == 0) sloth_hashtable_realloc(table, 0, 2048);
  
  Sloth_U32 active_key = SLOTH_HASHTABLE_VALIDATE_KEY(key);
  Sloth_U8* active_value = value;
  Sloth_U32 index = sloth_hashtable_desired_pos(table, active_key);
  Sloth_U32 dist = 0;
  for (;;) 
  {
    if (table->keys[index] == 0)
    {
      sloth_hashtable_insert_(table, active_key, active_value, index);
      break;
    }
    
    Sloth_U32 existing_key = table->keys[index];
    Sloth_U32 existing_dist = SLOTH_HASHTABLE_PROBE_DISTANCE(
        table, existing_key, index
    );
    if (existing_dist < dist) {
      if (SLOTH_HASHTABLE_KEY_IS_DELETED(table->keys[index]))
      {
        sloth_hashtable_insert_(table, active_key, active_value, index);
        break;
      }
      
      // swap existing with the insertion and keep probing
      Sloth_U8* existing_value = table->values[index];
      table->values[index] = active_value;
      table->keys[index] = active_key;
      active_key = existing_key;
      active_value = existing_value;
      dist = existing_dist;
    }
    
    index = (index + 1) & SLOTH_HASHTABLE_CAP_MASK(table);
    dist += 1;
  }
  
  table->used += 1;
}

Sloth_Function Sloth_U32
sloth_hashtable_lookup_index_(Sloth_Hashtable* table, Sloth_U32 key, Sloth_Bool* is_empty)
{
  if (!table->keys) {
    if (is_empty) *is_empty = true;
    return 0;
  }
  key = SLOTH_HASHTABLE_VALIDATE_KEY(key);
  Sloth_U32 index = sloth_hashtable_desired_pos(table, key);
  while (table->keys[index] != 0 && table->keys[index] != key) {
    index = (index + 1) & SLOTH_HASHTABLE_CAP_MASK(table);
  }
  Sloth_U32 fkey = table->keys[index];  
  if (is_empty) {
    if (fkey == 0) *is_empty = true;
    if (SLOTH_HASHTABLE_KEY_IS_DELETED(fkey)) *is_empty = true;
  } else {
    if (fkey == 0) return 0;
    if (SLOTH_HASHTABLE_KEY_IS_DELETED(fkey)) return 0;
  }
  return index;
}

Sloth_Function Sloth_Bool 
sloth_hashtable_rem(Sloth_Hashtable* table, Sloth_U32 key)
{
  Sloth_Bool unused = false;
  Sloth_U32 index = sloth_hashtable_lookup_index_(table, key, &unused);
  if (index == 0) return false;
  table->keys[index] = table->keys[index] | SLOTH_HASHTABLE_TOMBSTONE;
  table->values[index] = 0;
  table->used -= 1;
  return true;
}

Sloth_Function Sloth_U8*  
sloth_hashtable_get(Sloth_Hashtable* table, Sloth_U32 key)
{
  Sloth_U32 index = sloth_hashtable_lookup_index_(table, key, 0);
  if (index == 0) return 0;
  return table->values[index];
}

Sloth_Function void
sloth_hashtable_free(Sloth_Hashtable* table)
{
  Sloth_U8* unused;
  unused = sloth_realloc(table->keys, sizeof(Sloth_U32) * table->cap, 0);
  unused = sloth_realloc(table->values, table->cap, 0);
}

Sloth_Function void      
sloth_arena_grow(Sloth_Arena* arena, Sloth_U32 min_size)
{
  SLOTH_PROFILE_BEGIN;
  if (!arena->buckets) {
    arena->buckets = sloth_array_grow(arena->buckets, 0, &arena->buckets_cap, 64, Sloth_U8*);
    sloth_zero_size__(sizeof(Sloth_U8*) * 64, (Sloth_U8*)arena->buckets);
    arena->buckets_len = 0;
    arena->bucket_cap = 1024 * 1024; // 1 MB
    arena->curr_bucket_len = 0;
  }
  if (arena->curr_bucket_len + min_size >= arena->bucket_cap)
  {
    arena->buckets_len += 1;
  }
  sloth_assert(arena->buckets_len < arena->buckets_cap);
  if (!arena->buckets[arena->buckets_len]) {
    Sloth_U32 unused = 0;
    arena->buckets[arena->buckets_len] = sloth_array_grow(arena->buckets[arena->buckets_len], 0, &unused, Sloth_Max(arena->bucket_cap, min_size), Sloth_U8);
    arena->curr_bucket_len = 0;
  }
}

Sloth_Function Sloth_U8* 
sloth_arena_push(Sloth_Arena* arena, Sloth_U32 size)
{
  SLOTH_PROFILE_BEGIN;
  if (arena->curr_bucket_len + size > arena->bucket_cap) sloth_arena_grow(arena, size);
  
  Sloth_U8* bucket = arena->buckets[arena->buckets_len];
  Sloth_U8* result = bucket + arena->curr_bucket_len;
  arena->curr_bucket_len += size;
  
  return result;
}

Sloth_Function void
sloth_arena_pop(Sloth_Arena* arena, Sloth_Arena_Loc to)
{
  SLOTH_PROFILE_BEGIN;
  if (to.bucket_index > arena->buckets_cap) return;
  if (to.bucket_index > arena->buckets_len) return;
  if (to.bucket_at > arena->bucket_cap) return;
  if (to.bucket_index == arena->buckets_len && to.bucket_at > arena->curr_bucket_len) return;
  
  Sloth_U32 bucket_before = arena->buckets_len;
  Sloth_U32 bucket_before_len = arena->curr_bucket_len;
  
  arena->buckets_len = to.bucket_index;
  arena->curr_bucket_len = to.bucket_at;
  
  if (to.bucket_index == bucket_before)
  {
    Sloth_U32 rewind_dist = bucket_before_len - to.bucket_at;
    Sloth_U8* old_at = arena->buckets[arena->buckets_len] + to.bucket_at;
    sloth_zero_size__(rewind_dist, old_at);
  }
  else if (to.bucket_index < bucket_before)
  {
    // clear all buckets back to the current one (excluding the current one
    for (Sloth_U32 i = bucket_before; i > to.bucket_index && i < bucket_before; i--)
    {    
      sloth_zero_size__(arena->bucket_cap, arena->buckets[i]);
    }
    // clear the current one back to the current point
    sloth_zero_size__(arena->bucket_cap - to.bucket_at, arena->buckets[arena->buckets_len] + to.bucket_at);
  }
  else
  {
    sloth_invalid_code_path;
  }
}

Sloth_Function Sloth_Arena_Loc
sloth_arena_at(Sloth_Arena* arena)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Arena_Loc result;
  result.bucket_index = arena->buckets_len;
  result.bucket_at = arena->curr_bucket_len;
  return result;;
}

Sloth_Function void      
sloth_arena_clear(Sloth_Arena* arena)
{
  SLOTH_PROFILE_BEGIN;
  arena->buckets_len = 0;
  arena->curr_bucket_len = 0;
  
  // @DebugClear
#ifdef DEBUG
  for (Sloth_U32 i = 0; i < arena->buckets_cap; i++)
  {
    Sloth_U8* bucket = arena->buckets[i];
    if (bucket) sloth_zero_size__(arena->bucket_cap, bucket);
  }
#endif
}

Sloth_Function void      
sloth_arena_free(Sloth_Arena* arena)
{
  SLOTH_PROFILE_BEGIN;
  for (Sloth_U32 i = 0; i < arena->buckets_cap; i++)
  {
    Sloth_U8* bucket = arena->buckets[i];
    if (bucket) {
      arena->buckets[i] = sloth_realloc(bucket, arena->bucket_cap, 0);
    }
  }
  arena->buckets = sloth_realloc_array(arena->buckets, Sloth_U8*, arena->buckets_cap, 0);
  arena->buckets_cap = 0;
  arena->buckets_len = 0;
  arena->bucket_cap = 0;
  arena->curr_bucket_len = 0;
}

// TODO(PS): #ifdef this out if the user don't want to 
// have sloth include C standard library functions
Sloth_Function Sloth_Font_ID 
sloth_font_load(Sloth_Ctx* sloth, char* path, Sloth_R32 pixel_height)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font_ID result = SLOTH_ZII;
  
  FILE* file = fopen(path, "rb");
  if (!file) return result;
  
  fseek(file, 0, SEEK_END);
  Sloth_U32 file_size = (Sloth_U32)ftell(file);
  fseek(file, 0, SEEK_SET);
  
  Sloth_U8* file_data = (Sloth_U8*)malloc(file_size);
  Sloth_U32 num_read = fread(file_data, file_size, 1, file);
  
  // determine path length
  char* at = path;
  for (; *at != 0; at++) {};
  Sloth_U32 path_len = (Sloth_U32)(at - path);
  
  result = sloth_font_load_from_memory(sloth, path, path_len, file_data, file_size, pixel_height);
  return result;
}

Sloth_Function Sloth_Font_ID 
sloth_font_load_from_memory(Sloth_Ctx* sloth, char* font_name, Sloth_U32 font_name_len, Sloth_U8* data, Sloth_U32 data_size, Sloth_R32 pixel_height)
{
  SLOTH_PROFILE_BEGIN;
  sloth->fonts = sloth_array_grow(sloth->fonts, sloth->fonts_len, &sloth->fonts_cap, 8, Sloth_Font);
  
  Sloth_Font_ID result;
  result.value = sloth->fonts_len++;
  result.weight_index = 0;
  
  Sloth_Font* new_font = sloth->fonts + result.value;
  sloth_zero_struct_(new_font);
  sloth_copy_memory_((Sloth_U8*)new_font->name, (Sloth_U8*)font_name, font_name_len);
  new_font->renderer_data = sloth->font_renderer_load_font(sloth, new_font, data, data_size, 0, pixel_height);
  new_font->metrics.pixel_height = pixel_height;
  
  return result;
}

Sloth_Function Sloth_Font*
sloth_font_get_(Sloth_Ctx* sloth, Sloth_Font_ID font_id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font* font = sloth->fonts + font_id.value;
  return font;
}

Sloth_Function Sloth_Font_ID
sloth_font_register_family(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 weight, Sloth_U32 family)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font* font = sloth_font_get_(sloth, font_id);
  sloth_assert(font->weights_len < SLOTH_FONT_WEIGHTS_CAP);
  
  Sloth_Font_ID result = font_id;
  result.weight_index = font->weights_len++;
  font->weights[result.weight_index].weight = weight;
  font->weights[result.weight_index].glyph_family = family;
  
  return result;
}

Sloth_Function Sloth_Glyph_ID
sloth_font_register_codepoint(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 codepoint)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph_ID result = SLOTH_ZII;
  if (sloth->font_renderer_register_glyph) {
    sloth->font_renderer_register_glyph(sloth, font_id, codepoint);
  }
  return result;
}

Sloth_Function Sloth_Font*
sloth_glyph_to_font(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph_id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font* result = 0;
  for (Sloth_U32 i = 0; i < sloth->fonts_len; i++)
  {
    Sloth_Font* at = sloth->fonts + i;
    for (Sloth_U32 w = 0; w < at->weights_len; w++)
    {
      if (at->weights[w].glyph_family == glyph_id.family)
      {
        result = at;
        break;
      }
    }
    if (result) break;
  }
  return result;
}

Sloth_Function Sloth_Font_Metrics
sloth_font_get_metrics(Sloth_Ctx* sloth, Sloth_Font_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font_Metrics result = SLOTH_ZII;
  Sloth_Font* font = sloth_font_get_(sloth, id);
  if (!font) return result;
  return font->metrics;
}

Sloth_Function void
sloth_font_set_metrics(Sloth_Ctx* sloth, Sloth_Font_ID id, Sloth_Font_Metrics metrics)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Font* font = sloth_font_get_(sloth, id);
  if (!font) return;
  font->metrics = metrics;
}

Sloth_Function void
sloth_set_current_font(Sloth_Ctx* sloth, Sloth_Font_ID id)
{
  // TODO(PS): error checking on id
  sloth->font_active = id;
}

#define SLOTH_GLYPH_ID_TO_INDEX(id) ((id).family)

Sloth_Function Sloth_Glyph_Atlas* 
sloth_get_atlas_for_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph_Atlas* result = 0;
  Sloth_U32 index = SLOTH_GLYPH_ID_TO_INDEX(glyph);
  if (sloth->glyph_atlases_cap > index) {
    result = sloth->glyph_atlases + index;
    if (result->id != glyph.family) result = 0;
  }
  return result;
}

Sloth_Function Sloth_VIBuffer*
sloth_get_vibuffer_for_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID glyph)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_VIBuffer* result = 0;
  Sloth_U32 index = SLOTH_GLYPH_ID_TO_INDEX(glyph);
  if (sloth->glyph_atlases_cap > index) {
    result = sloth->vibuffers + index;
  }
  return result;
}

Sloth_Function void             
sloth_glyph_atlas_resize(Sloth_Glyph_Atlas* atlas, Sloth_U32 new_dim)
{
  SLOTH_PROFILE_BEGIN;
  sloth_assert(sloth_is_pow2(new_dim));
  Sloth_U32 new_cap = new_dim * new_dim;
  Sloth_U32 old_cap = atlas->dim * atlas->dim;
  atlas->data = (Sloth_U8*)sloth_realloc_array(atlas->data, Sloth_U32, old_cap, new_cap);
  atlas->dim = new_dim;
  atlas->dirty_state = Sloth_GlyphAtlas_Dirty_Grow;
}

Sloth_Function Sloth_Glyph_Atlas* 
sloth_create_atlas(Sloth_Ctx* sloth, Sloth_U8 family, Sloth_U32 min_dim)
{
  if (family >= sloth->glyph_atlases_cap) 
  {
    Sloth_U32 new_cap = Sloth_Max(family + 1, SLOTH_GLYPH_ATLASES_MIN_CAP);
    
    // resize the glyph atlas array
    Sloth_U32 old_size = sizeof(Sloth_Glyph_Atlas) * sloth->glyph_atlases_cap;
    Sloth_U32 new_size = sizeof(Sloth_Glyph_Atlas) * new_cap;
    sloth->glyph_atlases = (Sloth_Glyph_Atlas*)sloth_realloc(sloth->glyph_atlases, old_size, new_size);
    
    // resize the corresponding vertex buffer array
    old_size = sizeof(Sloth_VIBuffer) * sloth->glyph_atlases_cap;
    new_size = sizeof(Sloth_VIBuffer) * new_cap;
    sloth->vibuffers = (Sloth_VIBuffer*)sloth_realloc(sloth->vibuffers, old_size, new_size);
    
    // zero out new entries
    for (Sloth_U32 i = sloth->glyph_atlases_cap; i < new_cap; i++) {
      sloth_zero_struct_(&sloth->glyph_atlases[i]);
      sloth_zero_struct_(&sloth->vibuffers[i]);
    }
    
    sloth->glyph_atlases_cap = new_cap;
  }
  sloth_assert(sloth->glyph_atlases_cap > family);
  
  Sloth_U32 atlas_dim = Sloth_Max(SLOTH_GLYPH_ATLAS_START_DIM, min_dim);
  
  Sloth_Glyph_Atlas* result = sloth->glyph_atlases + family;
  sloth_assert(result->id == 0);
  result->id = family;
  sloth_glyph_atlas_resize(result, atlas_dim);
  
  return result;
}

Sloth_Function Sloth_U32
sloth_xy_to_texture_offset(Sloth_U32 x, Sloth_U32 y, Sloth_U32 dim, Sloth_U32 bytes_per_pixel)
{
  SLOTH_PROFILE_BEGIN;
  return (((y * dim) + x) * bytes_per_pixel);
}
Sloth_Function Sloth_U32
sloth_xy_to_texture_offset_u32(Sloth_U32 x, Sloth_U32 y, Sloth_U32 dim)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_xy_to_texture_offset(x, y, dim, sizeof(Sloth_U32));
}
Sloth_Function Sloth_U32
sloth_xy_to_texture_offset_u8(Sloth_U32 x, Sloth_U32 y, Sloth_U32 dim)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_xy_to_texture_offset(x, y, dim, sizeof(Sloth_U8));
}

Sloth_Function Sloth_U32
sloth_color_apply_gamma(Sloth_U32 color, Sloth_R32 power)
{
  SLOTH_PROFILE_BEGIN;
  if (power == 1) return color;
  
  Sloth_R32 r = (Sloth_R32)((color >> 24) & 0xFF) / 255.0f;
  Sloth_R32 g = (Sloth_R32)((color >> 16) & 0xFF) / 255.0f;
  Sloth_R32 b = (Sloth_R32)((color >>  8) & 0xFF) / 255.0f;
  Sloth_R32 a = (Sloth_R32)((color      ) & 0xFF) / 255.0f;
  
  Sloth_R32 gamma_r = powf(r, power);
  Sloth_R32 gamma_g = powf(g, power);
  Sloth_R32 gamma_b = powf(b, power);
  Sloth_R32 gamma_a = powf(a, power);
  
  Sloth_U32 result = (((Sloth_U32)(gamma_r * 255) << 24) |
    ((Sloth_U32)(gamma_g * 255) << 16) |
    ((Sloth_U32)(gamma_b * 255) <<  8) |
    ((Sloth_U32)(gamma_a * 255)));
  return result; 
}

Sloth_Function Sloth_Glyph_ID   
sloth_register_glyph(Sloth_Ctx* sloth, Sloth_Glyph_Desc desc)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph_Store* store = &sloth->glyph_store;
  if (!store->glyphs || store->glyphs_table.used >= store->glyphs_cap) 
  {
    store->glyphs = sloth_array_grow(store->glyphs, store->glyphs_table.used, &store->glyphs_cap, 256, Sloth_Glyph);
  }
  
  Sloth_Glyph_ID new_glyph_id;
  new_glyph_id.value = desc.id & 0x00FFFFFF;
  new_glyph_id.family = desc.family & 0xFF;
  
  // check if this glyph has already been registered
  if (sloth_hashtable_lookup_index_(&store->glyphs_table, new_glyph_id.value, 0) != 0) {
    return new_glyph_id;
  }
  
  Sloth_U32 new_glyph_index = store->glyphs_table.used;
  Sloth_Glyph* new_glyph = store->glyphs + new_glyph_index;
  sloth_hashtable_add(&store->glyphs_table, new_glyph_id.value, (Sloth_U8*)new_glyph);
  
  // Get correct atlas for the glyph being registered
  Sloth_Glyph_Atlas* atlas = sloth_get_atlas_for_glyph(sloth, new_glyph_id);
  if (!atlas) 
  {
    Sloth_U32 min_dim = Sloth_Max(desc.src_width, desc.src_height);
    min_dim = sloth_round_to_pow2_u32(min_dim);
    atlas = sloth_create_atlas(sloth, new_glyph_id.family, min_dim);
  }
  sloth_assert(atlas != 0);
  
  // TODO(PS): glyphs_table.used > 1 is because we've already
  // added the new glyph. If its the first glyph, there is no
  // previous glyph, but the table has a glyph in it already.
  //
  Sloth_U32 dst_x = 0;
  Sloth_U32 dst_y = 1;
  if (store->glyphs_table.used > 1) {
    Sloth_Glyph last_glyph = store->glyphs[atlas->last_glyph];
    dst_x = last_glyph.offset_x + last_glyph.src_width + 1; // add room for apron
    dst_y = last_glyph.offset_y;
  }
  atlas->last_glyph = new_glyph_index;
  
  // See if we need to move to a new row
  Sloth_U32 apron_dim = 2;
  if ((dst_x + desc.src_width + apron_dim) > atlas->dim)
  {    
    dst_x = 0;
    
    // Find the tallest glyph in the previous row and start the next
    // row above that glyph
    // TODO: This is inefficient, but its simple     
    for(Sloth_U32 glyph_row_i = atlas->last_row_first_glyph;
      glyph_row_i < new_glyph_index;
      glyph_row_i++
    ){
      Sloth_Glyph row_glyph = store->glyphs[glyph_row_i];
      Sloth_R32 next_row_y = row_glyph.offset_y + row_glyph.src_height;
      dst_y = Sloth_Max(dst_y, next_row_y);
    }
    dst_y += apron_dim / 2;
    
    atlas->last_row_first_glyph = new_glyph_index;
    
    if (dst_y + desc.src_height + (apron_dim / 2) >= atlas->dim)
    {
      // TODO: I'm punting this case till later. Current implementation
      // will auto create the texture with size 2048x2048 if its null, but
      // wont grow again beyond that.
      // The complexity I don't want to deal with right now is that wehn
      // the texture is resized, it either has to:
      //   a) reflow all existing glyphs (I don't like this)
      //   b) begin finding the empty space to the right of existing rows
      //      of glyphs.
      // b is the solution I want to go with but I don't want to do that
      // right now
      
      
      // NOTE: If this assert fires, you just need to grow your glyph dim
      
      // TODO: It would be nice if this would just do it for you, or better yet
      // do it if we're under some bound (Say 4096) but create a second texture 
      // if we're above. To do that, it'll also want to make sure things like
      // Glyph Families (ie. a font's sprites) are all on one texture
      sloth_assert(dst_y + desc.src_height < atlas->dim);
    }
  }
  
  Sloth_U32 apron_x = dst_x;
  Sloth_U32 apron_y = dst_y - 1;
  dst_x += 1;
  
  new_glyph->offset_x = dst_x;
  new_glyph->offset_y = dst_y;
  new_glyph->src_width  = desc.src_width;
  new_glyph->src_height = desc.src_height;
  new_glyph->dst_width  = desc.dst_width == 0  ? desc.src_width  : desc.dst_width;
  new_glyph->dst_height = desc.dst_height == 0 ? desc.src_height : desc.dst_height;
  new_glyph->lsb = desc.cursor_to_glyph_start_xoff;
  new_glyph->x_advance = desc.cursor_to_next_glyph;
  new_glyph->baseline_offset_y = desc.baseline_offset_y;
  
  if (desc.data)
  {
    Sloth_R32 copy_gamma = 1;
    if (desc.copy_gamma != 0) copy_gamma = desc.copy_gamma;
    
    Sloth_U8* src_row_at = desc.data;
    Sloth_U8* dst_row_at = atlas->data + (((dst_y * atlas->dim) + dst_x) * sizeof(Sloth_U32));
    switch (desc.format) 
    {
      case Sloth_GlyphData_RGBA8:
      case Sloth_GlyphData_RGB8:
      {
        Sloth_U32 stride = sloth_glyph_data_format_strides[desc.format];
        for (Sloth_U32 y = dst_y; y < dst_y + desc.src_height; y++)
        {
          Sloth_U8* dst_at = dst_row_at;
          Sloth_U8* src_at = src_row_at;
          for (Sloth_U32 x = dst_x; x < dst_x + desc.src_width; x++)
          {
            dst_at[0] = sloth_color_apply_gamma(*src_at++, copy_gamma);
            dst_at[1] = sloth_color_apply_gamma(*src_at++, copy_gamma);
            dst_at[2] = sloth_color_apply_gamma(*src_at++, copy_gamma);
            if (desc.format == Sloth_GlyphData_RGBA8) {
              dst_at[3] = sloth_color_apply_gamma(*src_at++, copy_gamma);
            } else {
              dst_at[3] = 0xFF;
            }
            dst_at += 4;
          }
          dst_row_at += atlas->dim * sizeof(Sloth_U32);
          src_row_at += desc.stride * stride;
        }
      } break;
      
      case Sloth_GlyphData_Alpha8:
      {
        for (Sloth_U32 y = dst_y; y < dst_y + desc.src_height; y++)
        {
          Sloth_U8* dst_at = dst_row_at;
          Sloth_U8* src_at = src_row_at;
          for (Sloth_U32 x = dst_x; x < dst_x + desc.src_width; x++)
          {
            Sloth_U8 alpha = *src_at++;
            dst_at[0] = 0xFF;
            dst_at[1] = 0xFF;
            dst_at[2] = 0xFF;
            dst_at[3] = sloth_color_apply_gamma(alpha, copy_gamma);;
            dst_at += 4;
          }
          dst_row_at += atlas->dim * sizeof(Sloth_U32);
          src_row_at += desc.stride;
        }
      } break;
      
      sloth_invalid_default_case;
    }
  }
  
  // Draw the Apron
#define DRAW_APRON_VISIBLE 0
#define DRAW_APRON 0
#if DRAW_APRON
  Sloth_U32 apron_vertical0 = sloth_xy_to_texture_offset_u32(apron_x, apron_y, store->dim);
  Sloth_U32 apron_vertical1 = sloth_xy_to_texture_offset_u32(apron_x + desc.width + 1, apron_y, store->dim);
  for (Sloth_U32 apron_y_at = 0; apron_y_at < desc.src_height + apron_dim; apron_y_at++)
  {
    Sloth_U32* a0 = (Sloth_U32*)(store->data + apron_vertical0);
    Sloth_U32* a1 = (Sloth_U32*)(store->data + apron_vertical1);
#if !DRAW_APRON_VISIBLE
    // get the pixel immediately to the inside of the apron and copy its value
    // out to the apron
    Sloth_U32* g0 = (Sloth_U32*)(store->data + apron_vertical0 + sizeof(Sloth_U32));
    Sloth_U32* g1 = (Sloth_U32*)(store->data + apron_vertical1 - sizeof(Sloth_U32));
    *a0 = *g0;
    *a1 = *g1;
#else
    *a0 = 0xFF0000FF;
    *a1 = 0xFF0000FF;
#endif
    apron_vertical0 += store->dim * sizeof(Sloth_U32);
    apron_vertical1 += store->dim * sizeof(Sloth_U32);
  }
  
  Sloth_U32 apron_horiz0 = sloth_xy_to_texture_offset_u32(apron_x, apron_y, store->dim);
  Sloth_U32 apron_horiz1 = sloth_xy_to_texture_offset_u32(apron_x, apron_y + desc.src_height + 1, store->dim);
  for (Sloth_U32 apron_x_at = 0; apron_x_at < desc.src_width + apron_dim; apron_x_at++)
  {
    Sloth_U32* a0 = (Sloth_U32*)(store->data + apron_horiz0);
    Sloth_U32* a1 = (Sloth_U32*)(store->data + apron_horiz1);
#if !DRAW_APRON_VISIBLE
    // get the pixel immediately to the inside of the apron and copy its value
    // out to the apron
    Sloth_U32* g0 = (Sloth_U32*)(store->data + apron_horiz0 + (store->dim * sizeof(Sloth_U32)));
    Sloth_U32* g1 = (Sloth_U32*)(store->data + apron_horiz1 - (store->dim * sizeof(Sloth_U32)));
    *a0 = *g0;
    *a1 = *g1;
#else
    *a0 = 0xFF0000FF;
    *a1 = 0xFF0000FF;
#endif
    apron_horiz0 += sizeof(Sloth_U32);
    apron_horiz1 += sizeof(Sloth_U32);
  }
#endif // DRAW_APRON
  
  atlas->dirty_state = Sloth_GlyphAtlas_Dirty_UpdateData;
  return new_glyph_id;
}

Sloth_Function void             
sloth_unregister_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID id)
{
  SLOTH_PROFILE_BEGIN;
  // TODO: 
  sloth_invalid_code_path;
}

Sloth_Function Sloth_Bool
sloth_glyph_store_contains(Sloth_Glyph_Store* store, Sloth_Glyph_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph* glyph = (Sloth_Glyph*)sloth_hashtable_get(&store->glyphs_table, id.value);
  return (glyph != 0);
}

Sloth_Function Sloth_Glyph_Info 
sloth_lookup_glyph(Sloth_Ctx* sloth, Sloth_Glyph_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph_Info result = SLOTH_ZII;
  
  Sloth_Glyph_Store* store = &sloth->glyph_store;
  Sloth_Glyph* glyph = (Sloth_Glyph*)sloth_hashtable_get(&store->glyphs_table, id.value);
  if (!glyph) return result;
  
  Sloth_Glyph_Atlas* atlas = sloth_get_atlas_for_glyph(sloth, id);
  Sloth_R32 atlas_dim = (Sloth_R32)atlas->dim;
  
  result.glyph = *glyph;
  result.uv.value_min.x = (Sloth_R32)glyph->offset_x / atlas_dim;
  result.uv.value_min.y = (Sloth_R32)glyph->offset_y / atlas_dim;
  result.uv.value_max.x = (Sloth_R32)(glyph->offset_x + glyph->src_width) / atlas_dim;
  result.uv.value_max.y = (Sloth_R32)(glyph->offset_y + glyph->src_height) / atlas_dim;
  
  return result;
}

Sloth_Function void
sloth_glyph_store_free(Sloth_Glyph_Store* store)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U8* unused;
  
  // Free Glyph Storage
  unused = (Sloth_U8*)sloth_realloc_array(store->glyphs, Sloth_Glyph, store->glyphs_cap, 0);
  sloth_hashtable_free(&store->glyphs_table);
}

Sloth_Function Sloth_Glyph_ID   
sloth_make_glyph_id(Sloth_U32 family, Sloth_U32 id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Glyph_ID result;
  result.value = id & 0x00FFFFFF;
  result.family = family;
  return result;
}

Sloth_Function Sloth_Bool
sloth_glyph_id_matches_charcode(Sloth_Glyph_ID id, Sloth_U32 charcode)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U32 id_code = (id.id[0]      |
      id.id[1] << 8 |
      id.id[2] << 16);
  return id_code == charcode;
}

Sloth_Function Sloth_Bool
sloth_mouse_button_is_down(Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return (btn & Sloth_MouseState_IsDown);
}
Sloth_Function Sloth_Bool
sloth_mouse_button_was_down(Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return (btn & Sloth_MouseState_WasDown);
}

Sloth_Function Sloth_Bool 
sloth_mouse_button_transitioned_down(Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_mouse_button_is_down(btn) && !sloth_mouse_button_was_down(btn);
}

Sloth_Function Sloth_Bool 
sloth_mouse_button_transitioned_up (Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return !sloth_mouse_button_is_down(btn) && sloth_mouse_button_was_down(btn);
}

Sloth_Function Sloth_Bool 
sloth_mouse_button_held_down (Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_mouse_button_is_down(btn) && sloth_mouse_button_was_down(btn);
}

Sloth_Function Sloth_Bool 
sloth_mouse_button_held_up (Sloth_Mouse_State btn)
{
  SLOTH_PROFILE_BEGIN;
  return !sloth_mouse_button_is_down(btn) && !sloth_mouse_button_was_down(btn);
}

Sloth_Function void          
sloth_widget_pool_grow(Sloth_Widget_Pool* pool)
{
  pool->values = sloth_array_grow(
      pool->values,
    pool->len,
    &pool->cap,
    2048,
    Sloth_Widget
  );
}

Sloth_Function void
sloth_persistent_value_set__(Sloth_Widget_Cached* cached, Sloth_U32 size, Sloth_U8* value)
{
  cached->cached_value_len = size;
  sloth_copy_memory_(cached->cached_value, value, size);
}

Sloth_Function Sloth_U8*
sloth_persistent_value_get_(Sloth_Ctx* sloth, Sloth_ID id, Sloth_U32 size, Sloth_U8* fallback)
{
  sloth_assert(size < SLOTH_WIDGET_PERSISTENT_VALUE_CAP);
  Sloth_Widget_Cached* c = sloth_get_cached_data_for_id(sloth, id);
  if (c->cached_value_len == 0) {
    sloth_persistent_value_set__(c, size, fallback);
  }
  return c->cached_value;
}

Sloth_Function void
sloth_persistent_value_set_(Sloth_Ctx* sloth, Sloth_ID id, Sloth_U32 size, Sloth_U8* value)
{
  sloth_assert(size < SLOTH_WIDGET_PERSISTENT_VALUE_CAP);
  Sloth_Widget_Cached* c = sloth_get_cached_data_for_id(sloth, id);
  sloth_persistent_value_set__(c, size, value);
}

Sloth_Function Sloth_Widget* 
sloth_widget_pool_take(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* result = 0;
  if (!sloth->widgets.free_list)
  {
    sloth_widget_pool_grow(&sloth->widgets);
    result = sloth->widgets.values + sloth->widgets.len++;
  }
  else
  {
    result = sloth->widgets.free_list;
    sloth->widgets.free_list = result->sibling_next;
  }
  sloth_zero_struct_(result);
  return result;
}

Sloth_Function void          
sloth_widget_pool_give(Sloth_Ctx* sloth, Sloth_Widget* widget)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* child = widget->child_first; 
  while (child) {
    Sloth_Widget* next = child->sibling_next;
    sloth_widget_pool_give(sloth, child);
    child = next;
  }
  
  widget->id.value = 0;
  widget->sibling_next = 0;
  widget->sibling_next = sloth->widgets.free_list;
  sloth->widgets.free_list = widget;
}

Sloth_Function void
sloth_widget_pool_free(Sloth_Widget_Pool* pool)
{
  SLOTH_PROFILE_BEGIN;
  sloth_free((void*)pool->values, sizeof(Sloth_Widget) * pool->cap);
  sloth_zero_struct_(pool);
}

Sloth_Function void          
sloth_widget_cached_pool_grow(Sloth_Widget_Cached_Pool* pool)
{
  if (!pool->buckets) {
    pool->buckets_cap = 32;
    pool->buckets = sloth_realloc_array(pool->buckets, Sloth_Widget_Cached*, 0, pool->buckets_cap);
    pool->buckets_len = 0;
    pool->bucket_at = 0;
    pool->bucket_at_len = 0;
    pool->bucket_cap = 1024;
    sloth_zero_size__(sizeof(Sloth_Widget_Cached_Pool*) * pool->buckets_cap, (Sloth_U8*)pool->buckets);
  }
  
  if (pool->bucket_at_len >= pool->bucket_cap)
  {
    pool->bucket_at += 1;
    pool->bucket_at_len = 0;
  }
  
  if (pool->buckets[pool->bucket_at] == 0) {
    Sloth_Widget_Cached** at = pool->buckets + pool->bucket_at;
    *at = sloth_realloc_array(*at, Sloth_Widget_Cached, 0, pool->bucket_cap);
  }
}

Sloth_Function Sloth_Widget_Cached* 
sloth_widget_cached_pool_take(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Cached_Pool* p = &sloth->widget_caches;
  Sloth_Widget_Cached* result = 0;
  if (!p->free_list)
  {    
    sloth_widget_cached_pool_grow(p);
    result = p->buckets[p->bucket_at] + p->bucket_at_len++;
  }
  else
  {
    result = p->free_list;
    p->free_list = result->free_next;
  }
  sloth_assert(result != 0);
  sloth_zero_struct_(result);
  return result;
}

Sloth_Function void          
sloth_widget_cached_pool_give(Sloth_Ctx* sloth, Sloth_Widget_Cached* widget)
{
  SLOTH_PROFILE_BEGIN;
  widget->free_next = sloth->widget_caches.free_list;
  sloth->widget_caches.free_list = widget;
}

Sloth_Function void
sloth_widget_cached_pool_free(Sloth_Widget_Cached_Pool* pool)
{
  SLOTH_PROFILE_BEGIN;
  for (Sloth_U32 i = 0; i < pool->buckets_cap; i++) 
  {
    sloth_free((void*)pool->buckets[i], sizeof(Sloth_Widget_Cached) * pool->bucket_cap);    
  }
  sloth_free((void*)pool->buckets, sizeof(Sloth_Widget_Cached*) * pool->buckets_cap);
  sloth_zero_struct_(pool);
}

#ifdef DEBUG
#  define sloth_validate_widget_(w) sloth_validate_widget__(w)
#else
#  define sloth_validate_widget_(w)
#endif

Sloth_Function void
sloth_validate_widget__(Sloth_Widget* w)
{
  sloth_assert(w->cached);
  sloth_assert(w->cached->canary_start_ == 0 && w->cached->canary_end_ == 0);
  
  // Look for cycles in w->siblings
  for (Sloth_Widget* s = w->sibling_prev; s != 0; s = s->sibling_prev)
  {
    sloth_assert(s != w);
  }
  
  // Look for cycles in w->children
  for (Sloth_Widget* c = w->child_first; c != 0; c = c->child_first) 
  {
    sloth_assert(c != w);
    sloth_validate_widget__(c);
  }
  
}

Sloth_Function Sloth_Widget_Cached*
sloth_get_cached_data_for_id(Sloth_Ctx* sloth, Sloth_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Cached* result = 0;
  result = (Sloth_Widget_Cached*)sloth_hashtable_get(&sloth->widget_cache_lut, id.value);
  if (!result) {
    result = sloth_widget_cached_pool_take(sloth);
    sloth_hashtable_add(&sloth->widget_cache_lut, id.value, (Sloth_U8*)result);
  } 
  sloth_assert(result != 0);  
  return result;
}

Sloth_Function Sloth_Widget* 
sloth_push_widget_on_tree(Sloth_Ctx* sloth, Sloth_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* widget = sloth_widget_pool_take(sloth);
  if (sloth->widget_tree_parent_cur) {
    Sloth_Widget* parent = sloth->widget_tree_parent_cur;
    if (parent->child_last)
    {
      widget->sibling_prev = parent->child_last;
      sloth_assert(widget->sibling_prev != widget->sibling_next);
      parent->child_last->sibling_next = widget;
      sloth_assert(parent->child_last->sibling_next != parent->child_last);
      parent->child_last = widget;
    }
    else
    {
      parent->child_first = widget;
      parent->child_last = widget;
      widget->sibling_prev = 0;
    }
    widget->parent = parent;
  }
  else
  {
    sloth->widget_tree_root = widget;
  }
  
  sloth->widget_tree_parent_cur = widget;
  sloth->widget_tree_next_child = widget->child_first;
  sloth->widget_tree_depth_cur += 1;
  if (sloth->widget_tree_depth_cur > sloth->widget_tree_depth_max) {
    sloth->widget_tree_depth_max = sloth->widget_tree_depth_cur;
  }
  widget->id = id;  
  widget->cached = sloth_get_cached_data_for_id(sloth, id);
  
  return widget;
}

Sloth_Function Sloth_Widget*        
sloth_pop_widget_off_tree(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* parent_cur = sloth->widget_tree_parent_cur;
  
  if (parent_cur != sloth->widget_tree_root) {
    // parent_cur is either the root or its parent has a valid
    // SLL of children
    sloth_assert((parent_cur->parent != 0 && parent_cur->parent->child_first != 0));
    
    // parent_cur either must have a previous sibling, or
    // it's parent must think this is the first child
    // not both.
    sloth_assert((Sloth_Bool)(parent_cur->sibling_prev != 0) != (Sloth_Bool)(parent_cur->parent->child_first == parent_cur));
  }
  
  // if there's an expected next child, that means last
  // frame this widget had more children than it does now
  if (sloth->widget_tree_next_child) {
    if (parent_cur->child_last) {
      Sloth_Widget* last_widget = parent_cur->child_last;
      last_widget->sibling_next = 0;
    } else {
      parent_cur->child_first = 0;
    }
    Sloth_Widget* at = sloth->widget_tree_next_child;
    while (at != 0) {
      Sloth_Widget* next_at = at->sibling_next;
      sloth_widget_pool_give(sloth, sloth->widget_tree_next_child);
      at = next_at;
    }
  }
  
  if (sloth->widget_tree_parent_cur != sloth->widget_tree_root)
  {
    sloth->widget_tree_parent_cur = parent_cur->parent;
    sloth->widget_tree_next_child = parent_cur->sibling_next;
    sloth->widget_tree_depth_cur -= 1;
  }
  else
  {
    // we're at the root
    sloth->widget_tree_next_child = 0;
  }
  
  sloth_assert(sloth->widget_tree_parent_cur);
  sloth_validate_widget_(parent_cur);
  return parent_cur;
}

// Order: Left Root Right
Sloth_Function void 
sloth_tree_walk_inorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (sloth->widgets.len == 0) return;
  
  Sloth_Arena_Loc scratch_at = sloth_arena_at(&sloth->scratch);
  Sloth_Widget** stack = sloth_arena_push_array(&sloth->scratch, Sloth_Widget*, sloth->widgets.len);
  Sloth_U32     stack_len = 0;
  
  Sloth_Widget* at = start;
  Sloth_Tree_Walk_Result last_result = Sloth_TreeWalk_Continue;
  while (last_result != Sloth_TreeWalk_Stop)
  {
    if (at != 0) 
    {
      stack[stack_len++] = at; // push
      at = at->child_first;
    }
    else if (stack_len > 0)
    {
      at = stack[--stack_len]; // pop
      last_result = cb(sloth, at, user_data);
      sloth_assert(last_result == Sloth_TreeWalk_Continue ||
          last_result == Sloth_TreeWalk_Stop);
      
      at = at->sibling_next;
    }
    else
    {
      break;
    }
  }
  
  sloth_arena_pop(&sloth->scratch, scratch_at);
}

// Order: root, children, siblings
Sloth_Function void 
sloth_tree_walk_preorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (sloth->widgets.len == 0) return;
  
  Sloth_Arena_Loc scratch_at = sloth_arena_at(&sloth->scratch);
  Sloth_Widget** stack = sloth_arena_push_array(&sloth->scratch, Sloth_Widget*, sloth->widgets.len);
  Sloth_U32     stack_len = 0;
  
  Sloth_Widget* at = start;
  stack[stack_len++] = at;
  
  Sloth_Tree_Walk_Result last_result = Sloth_TreeWalk_Continue;
  while (stack_len > 0 && last_result != Sloth_TreeWalk_Stop)
  {
    at = stack[--stack_len]; // pop
    if (at != 0) // if the requested node exists
    {
      if (last_result == Sloth_TreeWalk_Continue)
      {
        last_result = cb(sloth, at, user_data); // visit
      }
      stack[stack_len++] = at->sibling_next; // right node
      
      switch (last_result)
      {
        case Sloth_TreeWalk_Continue: {
          stack[stack_len++] = at->child_first; // left node
        } break;
        
        case Sloth_TreeWalk_Continue_SkipChildren: {
          last_result = Sloth_TreeWalk_Continue;
        } break;
        
        // acceptable values but no action requried here
        case Sloth_TreeWalk_Stop:
        
        sloth_invalid_default_case;
      }
    }
  }
  
  sloth_arena_pop(&sloth->scratch, scratch_at);
}

typedef struct Sloth_Postorder_Widget Sloth_Postorder_Widget;
struct Sloth_Postorder_Widget
{
  Sloth_Widget* widget;
  Sloth_U8 is_right_child;
};

// Order: children, siblings, root
// NOTE: I believe this is a bit of a modification on 
// postorder traversal, since we still want children
// to be visited from left to right. 
Sloth_Function void 
sloth_tree_walk_postorder_(Sloth_Ctx* sloth, Sloth_Widget* start, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (sloth->widgets.len == 0) return;
  
  Sloth_Arena_Loc scratch_at = sloth_arena_at(&sloth->scratch);
  Sloth_Postorder_Widget* stack = sloth_arena_push_array(&sloth->scratch, Sloth_Postorder_Widget, sloth->widgets.len);
  Sloth_U32     stack_len = 0;
  
  Sloth_Widget* at = start;
  Sloth_Tree_Walk_Result last_result = Sloth_TreeWalk_Continue;
  do {
    while (at != 0) // while at exists
    {
      if (at->sibling_next != 0) // if right exists
      {
        Sloth_U32 index_r = stack_len++; // push right
        stack[index_r].widget = at->sibling_next; // push right
        stack[index_r].is_right_child = true;
      }
      // push
      Sloth_U32 index_l = stack_len++;
      stack[index_l].widget = at; 
      stack[index_l].is_right_child = false;
      
      at = at->child_first; // go to left
    }
    
    Sloth_Postorder_Widget* wat = &stack[--stack_len];
    at = wat->widget;
    while (at != 0 && !wat->is_right_child) // while at exists and is not the right child of a node
    {
      last_result = cb(sloth, at, user_data); // visit
      if (last_result == Sloth_TreeWalk_Stop) break;
      
      if (stack_len == 0) {
        wat = 0;
        break;
      }
      wat = &stack[--stack_len];
      at = wat->widget;
    }
    
    if (wat && wat->is_right_child) 
    {
      wat->is_right_child = false;
      at = wat->widget;
    }
  } while (stack_len > 0 && last_result != Sloth_TreeWalk_Stop);
  
  sloth_arena_pop(&sloth->scratch, scratch_at);
}

Sloth_Function void 
sloth_tree_walk_inorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  sloth_tree_walk_inorder_(sloth, sloth->widget_tree_root, cb, user_data);
}

Sloth_Function void 
sloth_tree_walk_preorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  sloth_tree_walk_preorder_(sloth, sloth->widget_tree_root, cb, user_data);
}

Sloth_Function void 
sloth_tree_walk_postorder(Sloth_Ctx* sloth, Sloth_Tree_Walk_Cb* cb, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  sloth_tree_walk_postorder_(sloth, sloth->widget_tree_root, cb, user_data);
}


#ifdef DEBUG
#  define sloth_widget_validate_layout_(l) sloth_widget_validate_layout__(l)
#else
#  define sloth_widget_validate_layout_(l)
#endif
Sloth_Function void
sloth_widget_validate_layout__(Sloth_Widget_Layout layout)
{
  SLOTH_PROFILE_BEGIN;
  sloth_assert(layout.margin.left.kind != Sloth_SizeKind_TextContent && layout.margin.left.kind != Sloth_SizeKind_ChildrenSum);
  sloth_assert(layout.margin.right.kind != Sloth_SizeKind_TextContent && layout.margin.right.kind != Sloth_SizeKind_ChildrenSum);
  sloth_assert(layout.margin.top.kind != Sloth_SizeKind_TextContent && layout.margin.top.kind != Sloth_SizeKind_ChildrenSum);
  sloth_assert(layout.margin.bottom.kind != Sloth_SizeKind_TextContent && layout.margin.bottom.kind != Sloth_SizeKind_ChildrenSum);
  
  sloth_assert(layout.size[0].kind != Sloth_SizeKind_PercentOfSelf);
  sloth_assert(layout.size[1].kind != Sloth_SizeKind_PercentOfSelf);
}

Sloth_Function void
sloth_widget_handle_input_drag(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_Widget_Result* result)
{  
  if (!sloth_has_flag(widget->input.flags, Sloth_WidgetInput_Draggable)) return;
  result->drag_offset_pixels.x = sloth->mouse_pos.x - sloth->mouse_down_pos.x;
  result->drag_offset_pixels.y = sloth->mouse_pos.y - sloth->mouse_down_pos.y;
  
  Sloth_Widget* parent = widget->parent;
  if (parent) {
    Sloth_Widget_Cached parent_cached = *widget->parent->cached;
    result->drag_offset_percent_parent.x = result->drag_offset_pixels.x / parent_cached.dim.x;
    result->drag_offset_percent_parent.y = result->drag_offset_pixels.y / parent_cached.dim.y;
  }
}

Sloth_Function Sloth_Widget_Result
sloth_widget_handle_input(Sloth_Ctx* sloth, Sloth_Widget* widget)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Result result = SLOTH_ZII;
  result.widget = widget;
  
  Sloth_Widget_Input input = widget->input;
  Sloth_Widget_Cached cached = *widget->cached;
  if (sloth_ids_equal(sloth->active_widget, widget->id)) 
  {
    result.clicked = sloth_mouse_button_transitioned_down(sloth->mouse_button_l);
    result.held = sloth_mouse_button_held_down(sloth->mouse_button_l);  
    sloth_widget_handle_input_drag(sloth, widget, &result);
    
    if (sloth_has_flag(input.flags, Sloth_WidgetInput_TextSelectable))
    {
      result.selected_glyphs_first = sloth->active_widget_selected_glyphs_first;
      result.selected_glyphs_one_past_last = sloth->active_widget_selected_glyphs_one_past_last;
    }
  }
  
  if (sloth_ids_equal(sloth->last_active_widget, widget->id))
  {
    if (sloth_mouse_button_transitioned_up(sloth->mouse_button_l))
    {
      result.released = true;
      sloth_widget_handle_input_drag(sloth, widget, &result);
    }
    
    if (sloth_has_flag(input.flags, Sloth_WidgetInput_TextSelectable))
    {
      result.selected_glyphs_first = sloth->active_widget_selected_glyphs_first;
      result.selected_glyphs_one_past_last = sloth->active_widget_selected_glyphs_one_past_last;
    }
  }
  
  return result;
}

Sloth_Function Sloth_Widget_Layout
sloth_widget_layout_apply_defaults(Sloth_Ctx* sloth, Sloth_Widget_Layout layout)
{
  if (layout.width.kind == Sloth_SizeKind_None)  layout.width  = SLOTH_SIZE_CHILDREN_SUM;
  if (layout.height.kind == Sloth_SizeKind_None) layout.height = SLOTH_SIZE_CHILDREN_SUM;
  sloth_widget_validate_layout_(layout);
  return layout;
}

Sloth_Function Sloth_Widget_Style
sloth_widget_style_apply_defaults(Sloth_Ctx* sloth, Sloth_Widget_Style style)
{
  if (style.font.value == 0) style.font = sloth->font_active;
  return style;
}

Sloth_Function void
sloth_widget_allocate_text(Sloth_Ctx* sloth, Sloth_Widget_Result widget_result, Sloth_U32 text_len)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* widget = widget_result.widget;
  widget->text = sloth_arena_push_array(&sloth->per_frame_memory, Sloth_Glyph_Layout, text_len + 1);
  widget->text_cap = text_len;
}

Sloth_Function void
sloth_widget_text_to_glyphs_append(Sloth_Ctx* sloth, Sloth_Widget_Result widget_result, Sloth_Font_ID font, Sloth_Char* text, Sloth_U32 text_len)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* widget = widget_result.widget;
  
  Sloth_U32 text_family = 0;
  if (sloth->fonts) {
    text_family = sloth->fonts[font.value].weights[font.weight_index].glyph_family;
  }
  
  Sloth_Bool show_selected = sloth_has_flag(widget->input.flags, Sloth_WidgetInput_TextSelectable);
  show_selected &= sloth_ids_equal(sloth->last_active_widget, widget->id);
  
  sloth_assert(widget->text_len + text_len <= widget->text_cap);
  for (Sloth_U32 char_i = 0; char_i < text_len; char_i++)
  {
    Sloth_U32 glyph_i = widget->text_len++;
    
    Sloth_U32 char_code = (Sloth_U32)text[char_i];
    Sloth_Glyph_ID g = sloth_make_glyph_id(text_family, char_code);
    
    Sloth_Bool after_first = glyph_i >= widget_result.selected_glyphs_first;
    Sloth_Bool before_last = glyph_i < widget_result.selected_glyphs_one_past_last;
    Sloth_Bool is_selected = (show_selected && after_first && before_last);
    widget->text[glyph_i].selected = is_selected;
    widget->text[glyph_i].glyph_id = g;
    
    if (!sloth_glyph_store_contains(&sloth->glyph_store, g))
    {
      sloth_font_register_codepoint(sloth, font, char_code);
    }
  }
}

Sloth_Function Sloth_Widget_Result
sloth_push_widget_id(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, Sloth_ID id)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* widget = sloth_push_widget_on_tree(sloth, id);
  sloth_assert(widget->parent || widget == sloth->widget_tree_root);
  widget->layout = sloth_widget_layout_apply_defaults(sloth, desc.layout);
  widget->style  = sloth_widget_style_apply_defaults(sloth, desc.style);
  widget->input  = desc.input;
  
  Sloth_Widget_Result result = sloth_widget_handle_input(sloth, widget);
  return result;
}

Sloth_Function Sloth_Widget_Result
sloth_push_widget_v(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, va_list args)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_ID_Result idr = sloth_make_id_v(fmt, args);
  Sloth_Widget_Result result = sloth_push_widget_id(sloth, desc, idr.id);
  sloth_widget_allocate_text(sloth, result, idr.display_len);
  sloth_widget_text_to_glyphs_append(sloth, result, result.widget->style.font, idr.formatted, idr.display_len);
  return result;
}

Sloth_Function Sloth_Widget_Result 
sloth_push_widget_f(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...)
{
  SLOTH_PROFILE_BEGIN;
  va_list args; va_start(args, fmt);
  Sloth_Widget_Result result = sloth_push_widget_v(sloth, desc, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Widget_Result 
sloth_push_widget(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* text)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_push_widget_f(sloth, desc, text);
}

Sloth_Function void
sloth_pop_widget(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* last_widget = sloth_pop_widget_off_tree(sloth);
  sloth_assert(sloth->widget_tree_parent_cur);
}

Sloth_Function void
sloth_pop_widget_safe(Sloth_Ctx* sloth, Sloth_Widget_Result result)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget* last_widget = sloth_pop_widget_off_tree(sloth);
  sloth_assert(sloth->widget_tree_parent_cur);
  sloth_assert(last_widget == result.widget);
}

Sloth_Function Sloth_Widget_Result
sloth_widget_f(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...)
{
  SLOTH_PROFILE_BEGIN;
  va_list args; va_start(args, fmt);
  Sloth_Widget_Result result = sloth_push_widget_v(sloth, desc, fmt, args);
  va_end(args);
  sloth_pop_widget(sloth);
  return result;
}

Sloth_Function Sloth_Widget_Result
sloth_widget(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* text)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Result result = sloth_push_widget_f(sloth, desc, text);
  sloth_pop_widget(sloth);
  return result;
}


typedef struct Sloth_Layout_Cache Sloth_Layout_Cache;
struct Sloth_Layout_Cache
{
  Sloth_U8 axis;
  Sloth_R32 last_sibling_end;
  
  // fixed position offsets
};

Sloth_Function Sloth_R32
sloth_size_evaluate_margin(Sloth_Widget* widget, Sloth_Size margin_size, Sloth_U8 axis)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_R32 result = 0;
  if (margin_size.kind != Sloth_SizeKind_None)
  {
    if (margin_size.kind == Sloth_SizeKind_Pixels) {
      result = margin_size.value;
    } else {
      Sloth_Rect bounds;
      if (margin_size.kind == Sloth_SizeKind_PercentOfParent) {
        bounds = widget->parent->cached->bounds;
      } else if (margin_size.kind == Sloth_SizeKind_PercentOfSelf) {
        bounds = widget->cached->bounds;
      } else {
        sloth_invalid_code_path;
      }
      result = (bounds.value_max.E[axis] - bounds.value_min.E[axis]) * margin_size.value;
    }
  }
  return result;
}

Sloth_Function Sloth_R32
sloth_size_box_evaluate(Sloth_Widget* widget, Sloth_Size_Box margin, Sloth_U8 axis)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_R32 result = 0;
  result += sloth_size_evaluate_margin(widget, margin.E[axis].min, axis);
  result += sloth_size_evaluate_margin(widget, margin.E[axis].max, axis);
  return result;
}

Sloth_Function void
sloth_size_fixup_fixed_size_apply(Sloth_Widget* widget, Sloth_U8 axis)
{
  Sloth_R32 margin = sloth_size_evaluate_margin(widget, widget->layout.margin.E[axis].min, axis);
  margin += sloth_size_evaluate_margin(widget, widget->layout.margin.E[axis].max, axis);
  widget->cached->dim.E[axis] = widget->text_dim.E[axis] + margin;
}

// @PerAxisTreeWalkCB
Sloth_Function Sloth_Tree_Walk_Result
sloth_size_fixup_cb_fixed_size(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Layout_Cache* lc = (Sloth_Layout_Cache*)user_data;
  Sloth_U8 axis = lc->axis;
  
  switch (widget->layout.size[axis].kind)
  {
    case Sloth_SizeKind_Pixels:
    {
      widget->cached->dim.E[axis] = widget->layout.size[axis].value;
    } break;
    case Sloth_SizeKind_TextContent:
    {
      sloth_size_fixup_fixed_size_apply(widget, axis);
    } break;
    default: {} break; // do nothing
  }
  
  return Sloth_TreeWalk_Continue;
}

// @PerAxisTreeWalkCB
Sloth_Function Sloth_Tree_Walk_Result
sloth_size_fixup_cb_percent_parent(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Layout_Cache* lc = (Sloth_Layout_Cache*)user_data;
  Sloth_U8 axis = lc->axis;
  switch (widget->layout.size[axis].kind)
  {
    case Sloth_SizeKind_PercentOfParent:
    {
      Sloth_Widget* parent = widget->parent;
      sloth_assert(parent);
      
      // NOTE: this violation rises from the fact that that
      // this child relies on its parent for size, and the parent
      // relies on its children for size. This will be solved in 
      // the violation fixup step
      Sloth_Bool unsolved_violation = parent->layout.size[axis].kind == Sloth_SizeKind_ChildrenSum;
      if (!unsolved_violation)
      {
        Sloth_R32 parent_margin = sloth_size_box_evaluate(parent, parent->layout.margin, axis);
        widget->cached->dim.E[axis] = (parent->cached->dim.E[axis] - parent_margin) * widget->layout.size[axis].value;
      }
    } break;
    
    case Sloth_SizeKind_TextContent:
    {
      if (axis == Sloth_Axis_Y) {
        sloth_size_fixup_fixed_size_apply(widget, axis);
      }
      sloth_assert(widget->cached->dim.E[axis] >= widget->text_dim.E[axis]);
    } break;
    default: {} break; // do nothing
  }
  
  return Sloth_TreeWalk_Continue;
}

// @PerAxisTreeWalkCB
Sloth_Function Sloth_Tree_Walk_Result
sloth_size_fixup_cb_children_sum(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Layout_Cache* lc = (Sloth_Layout_Cache*)user_data;
  Sloth_U8 axis = lc->axis;
  
  // Determine relevant margins
  Sloth_Size_Range margin = widget->layout.margin.E[axis];
  Sloth_R32 margin_before = sloth_size_evaluate_margin(widget, margin.min, axis);
  Sloth_R32 margin_after  = sloth_size_evaluate_margin(widget, margin.max, axis);
  
  Sloth_Size_Kind kind = widget->layout.size[axis].kind;
  switch (kind)
  {
    case Sloth_SizeKind_ChildrenSum:
    {
      Sloth_R32 dim = margin_before + margin_after;
      Sloth_R32 max = 0;
      if (widget->child_first) {
        // Because no widgets have been laid out yet, we have to iterate
        // over all its children
        for (Sloth_Widget* child = widget->child_first; 
          child != 0; 
          child = child->sibling_next
        ){
          // TODO: Account for any child gap layout properties
          dim += child->cached->dim.E[axis];
          max = Sloth_Max(child->cached->dim.E[axis], max);
        }
      }
      max += margin_before + margin_after;
      
      Sloth_Bool dir_horizontal = (widget->layout.direction == Sloth_LayoutDirection_LeftToRight ||
          widget->layout.direction == Sloth_LayoutDirection_RightToLeft);
      Sloth_Bool dir_vertical   = (widget->layout.direction == Sloth_LayoutDirection_TopDown ||
          widget->layout.direction == Sloth_LayoutDirection_BottomUp);
      if (dir_horizontal)
      {
        if (axis == Sloth_Axis_X) {
          widget->cached->dim.x = dim;
        } else {
          widget->cached->dim.y = max;
        }
      }
      else if (dir_vertical)
      {
        if (axis == Sloth_Axis_X) {
          widget->cached->dim.x = max;
        } else {
          widget->cached->dim.y = dim;
        }
      }
      else
      {
        sloth_invalid_code_path;
      }
      
      // TODO: This is a place, where if we wanted to, we could implement
      // wrapping a-la flexbox 
      // ie. All the children should know their dimensions by now, so we 
      // can just reposition them if laying them out in a row would overflow
      // this widgets parent
    } break;
    default: {} break; // do nothing
  }
  
  return Sloth_TreeWalk_Continue;
}

// @PerAxisTreeWalkCB
Sloth_Function Sloth_Tree_Walk_Result
sloth_size_fixup_cb_violations(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Layout_Cache* lc = (Sloth_Layout_Cache*)user_data;
  Sloth_U8 axis = lc->axis;
  switch (widget->layout.size[axis].kind)
  {
    case Sloth_SizeKind_PercentOfParent:
    {
      Sloth_Widget* parent = widget->parent;
      Sloth_Widget_Layout pl = parent->layout;
      Sloth_Bool unsolved_violation = pl.size[axis].kind == Sloth_SizeKind_ChildrenSum;
      // TODO:
    } break;
    default: {} break; // do nothing
  }
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Rect
sloth_widget_calc_inner_bounds(Sloth_Widget* widget)
{
  SLOTH_PROFILE_BEGIN;
  SLOTH_PROFILE_BEGIN;
  sloth_widget_validate_layout_(widget->layout);
  Sloth_Rect result = widget->cached->bounds;
  result.value_min.x += sloth_size_evaluate_margin(widget, widget->layout.margin.left,    Sloth_Axis_X);
  result.value_min.y += sloth_size_evaluate_margin(widget, widget->layout.margin.top,     Sloth_Axis_Y);
  result.value_max.x -= sloth_size_evaluate_margin(widget, widget->layout.margin.right,   Sloth_Axis_X);
  result.value_max.y -= sloth_size_evaluate_margin(widget, widget->layout.margin.bottom,  Sloth_Axis_Y);
  if (result.value_max.x < result.value_min.x) {
    Sloth_R32 avg = (result.value_max.x + result.value_min.x) / 2;
    result.value_max.x = avg;
    result.value_min.x = avg;
  }
  if (result.value_max.y < result.value_min.y) {
    Sloth_R32 avg = (result.value_max.y + result.value_min.y) / 2;
    result.value_max.y = avg;
    result.value_min.y = avg;
  }
  return result;
}

// This is used to figure out which extents of a child are relevant
// to its next sibling attempting to draw itself. ie. based on this
// widgets dimensions, how far should the next widget offset itself
// based on how the parent wants to lay its children out
Sloth_Function Sloth_V2
sloth_layout_get_child_relevant_extents(Sloth_Widget* parent, Sloth_Rect widget_bounds)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 relevant_extents = SLOTH_ZII;
  switch (parent->layout.direction)
  {
    case Sloth_LayoutDirection_TopDown:
    {
      relevant_extents.E[Sloth_Axis_X] = widget_bounds.value_min.x;
      relevant_extents.E[Sloth_Axis_Y] = widget_bounds.value_max.y;
    } break;
    case Sloth_LayoutDirection_BottomUp:
    {
      relevant_extents.E[Sloth_Axis_X] = widget_bounds.value_min.x;
      relevant_extents.E[Sloth_Axis_Y] = widget_bounds.value_min.y;
    } break;
    case Sloth_LayoutDirection_LeftToRight:
    {
      relevant_extents.E[Sloth_Axis_X] = widget_bounds.value_max.x;
      relevant_extents.E[Sloth_Axis_Y] = widget_bounds.value_min.y;
    } break;
    case Sloth_LayoutDirection_RightToLeft:
    {
      relevant_extents.E[Sloth_Axis_X] = widget_bounds.value_min.x;
      relevant_extents.E[Sloth_Axis_Y] = widget_bounds.value_min.y;
    } break;
    default: break;
  }
  return relevant_extents;
}

Sloth_Function Sloth_V2
sloth_layout_clip_bounds_to_start_pos(Sloth_Widget* widget, Sloth_Rect clip_bounds)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V2 start;
  switch (widget->layout.direction)
  {
    case Sloth_LayoutDirection_TopDown:
    case Sloth_LayoutDirection_LeftToRight: { 
      start = clip_bounds.value_min;
    } break;
    case Sloth_LayoutDirection_RightToLeft: {
      start.x = clip_bounds.value_max.x;
      start.y = clip_bounds.value_min.y;
    } break;
    case Sloth_LayoutDirection_BottomUp: {
      start.x = clip_bounds.value_min.x;
      start.y = clip_bounds.value_max.y;
    } break;
  }
  
  Sloth_Widget_Cached c = *widget->cached;
  Sloth_V2 children_dim = sloth_v2_sub(c.children_bounds_max, c.children_bounds_min);
  start.x += children_dim.x * widget->cached->child_offset.x;
  start.y += children_dim.y * widget->cached->child_offset.y;
  
  return start;
}

Sloth_Function void
sloth_layout_position_parent_decides(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_Layout_Cache* lc)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U8 axis = lc->axis;
  Sloth_V2 offset = widget->cached->offset;
  Sloth_Rect bounds = widget->cached->bounds;
  
  Sloth_Rect clip_bounds;
  if (widget->parent) 
  {
    clip_bounds = sloth_widget_calc_inner_bounds(widget->parent);
    Sloth_V2 start = sloth_layout_clip_bounds_to_start_pos(widget->parent, clip_bounds);
    
    if (widget == widget->parent->child_first)
    {
      offset.E[axis] = start.E[axis];
    }
    else 
    {
      sloth_assert(widget->sibling_prev != 0);
      
      // seek backwards to find the last child that has had its
      // offset calculated. If none, treat this child as the first
      Sloth_Widget* last_relevant_sibling = widget->sibling_prev;
      while(last_relevant_sibling && last_relevant_sibling->layout.position.kind != Sloth_LayoutPosition_ParentDecides)
      {
        last_relevant_sibling = last_relevant_sibling->sibling_prev;
      }
      
      if (last_relevant_sibling)
      {
        Sloth_V2 extents = sloth_layout_get_child_relevant_extents(widget->parent, last_relevant_sibling->cached->bounds);
        offset.E[axis] = extents.E[axis];
      }
      else
      {
        offset.E[axis] = start.E[axis];
      }
    }
  } 
  else 
  {
    // Root
    lc->last_sibling_end = 0;
    offset.E[axis] = 0;
    
    // effectively, do not clip
    clip_bounds.value_min.x = Sloth_R32_Min;
    clip_bounds.value_min.y = Sloth_R32_Min;
    clip_bounds.value_max.x = Sloth_R32_Max;
    clip_bounds.value_max.y = Sloth_R32_Max;
  }
  
  // Offset based on parent layout direction
  if (widget->parent) {
    switch (widget->parent->layout.direction)
    {
      case Sloth_LayoutDirection_RightToLeft:
      {
        if (axis == Sloth_Axis_X) {
          offset.E[axis] -= widget->cached->dim.E[axis];
        }
      } break;
      case Sloth_LayoutDirection_BottomUp:
      {
        if (axis == Sloth_Axis_Y) {
          offset.E[axis] -= widget->cached->dim.E[axis];
        }
      } break;
    }
  }
  
  bounds.value_min.E[axis] = offset.E[axis];
  bounds.value_max.E[axis] = offset.E[axis] + widget->cached->dim.E[axis];
  
  widget->cached->dim.E[axis] = sloth_rect_dim(bounds).E[axis];
  
  widget->cached->offset = offset;
  widget->cached->bounds = bounds;
}

// At this point, everything has had its dimensions figured out
Sloth_Function void
sloth_layout_position_fixed(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_Layout_Cache* lc)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U32 axis = lc->axis;
  
  Sloth_Layout_Position pos = widget->layout.position;
  Sloth_R32 desired_offset_from_min = sloth_size_evaluate_margin(widget, pos.at.E[axis].min, axis);
  Sloth_R32 desired_offset_from_max = sloth_size_evaluate_margin(widget, pos.at.E[axis].max, axis);
  desired_offset_from_max += widget->cached->dim.E[axis];
  
  Sloth_Widget* parent = widget->parent;
  Sloth_R32 desired_offset = 0;
  if (widget->layout.position.kind == Sloth_LayoutPosition_FixedInParent)
  {
    desired_offset += parent->cached->offset.E[axis];
  }
  
  if (pos.at.E[axis].max.kind == Sloth_SizeKind_None) 
  {
    desired_offset += desired_offset_from_min;
  }
  else
  {
    desired_offset += parent->cached->dim.E[axis] - desired_offset_from_max;
  }
  
  widget->cached->offset.E[axis] = desired_offset;
  widget->cached->bounds.value_min.E[axis] = desired_offset;
  widget->cached->bounds.value_max.E[axis] = desired_offset + widget->cached->dim.E[axis];
}

// @PerAxisTreeWalkCB
// This function produces the final bounding boxes of each widget
Sloth_Function Sloth_Tree_Walk_Result
sloth_layout_cb(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Layout_Cache* lc = (Sloth_Layout_Cache*)user_data;
  
  switch (widget->layout.position.kind)
  {
    case Sloth_LayoutPosition_ParentDecides:
    {
      sloth_layout_position_parent_decides(sloth, widget, lc);
    } break;
    
    case Sloth_LayoutPosition_FixedInParent:
    case Sloth_LayoutPosition_FixedOnScreen:
    {
      sloth_layout_position_fixed(sloth, widget, lc);
    } break;
    
    sloth_invalid_default_case;
  }
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_clip_cb(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (!widget->parent) return Sloth_TreeWalk_Continue;
  
  // these layout specifiers don't get clipped
  if (widget->layout.position.kind == Sloth_LayoutPosition_FixedInParent ||
      widget->layout.position.kind == Sloth_LayoutPosition_FixedOnScreen)
  {
    return Sloth_TreeWalk_Continue;
  }
  
  Sloth_Rect parent_bounds = widget->parent->cached->bounds;
  Sloth_Rect bounds = widget->cached->bounds;
  bounds.value_min = sloth_rect_get_closest_point(parent_bounds, bounds.value_min);
  bounds.value_max = sloth_rect_get_closest_point(parent_bounds, bounds.value_max);
  
  // TODO(PS): if there is a glyph bg for this widget, 
  // this is where its uvs should be offset properly
  // using sloth_clip_rect_and_uv
  
  widget->cached->bounds = bounds;
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_U32
sloth_render_quad_ptc(Sloth_VIBuffer* vibuf, Sloth_Rect bounds, Sloth_R32 z, Sloth_V2 uv_min, Sloth_V2 uv_max, Sloth_U32 color)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V4 c4;
  c4.r = (Sloth_R32)((color >> 24) & 0xFF) / 255.0f;
  c4.g = (Sloth_R32)((color >> 16) & 0xFF) / 255.0f;
  c4.b = (Sloth_R32)((color >>  8) & 0xFF) / 255.0f;
  c4.a = (Sloth_R32)((color >>  0) & 0xFF) / 255.0f;
  
  Sloth_U32 v0 = sloth_vibuffer_push_vert(vibuf, bounds.value_min.x, bounds.value_min.y, z, uv_min.x, uv_min.y, c4);
  Sloth_U32 v1 = sloth_vibuffer_push_vert(vibuf, bounds.value_max.x, bounds.value_min.y, z, uv_max.x, uv_min.y, c4);
  Sloth_U32 v2 = sloth_vibuffer_push_vert(vibuf, bounds.value_max.x, bounds.value_max.y, z, uv_max.x, uv_max.y, c4);
  Sloth_U32 v3 = sloth_vibuffer_push_vert(vibuf, bounds.value_min.x, bounds.value_max.y, z, uv_min.x, uv_max.y, c4);
  sloth_vibuffer_push_quad(vibuf, v0, v1, v2, v3);
  
  return v0;
}

Sloth_Function void
sloth_render_update_quad_ptc(Sloth_VIBuffer* vibuf, Sloth_U32 quad_v0_index, Sloth_Rect bounds, Sloth_R32 z, Sloth_V2 uv_min, Sloth_V2 uv_max, Sloth_U32 color)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_V4 c4;
  c4.r = (Sloth_R32)((color >> 24) & 0xFF) / 255.0f;
  c4.g = (Sloth_R32)((color >> 16) & 0xFF) / 255.0f;
  c4.b = (Sloth_R32)((color >>  8) & 0xFF) / 255.0f;
  c4.a = (Sloth_R32)((color >>  0) & 0xFF) / 255.0f;
  
  sloth_assert(vibuf->verts_len > quad_v0_index + 3);
  
  sloth_vibuffer_set_vert(vibuf, quad_v0_index, bounds.value_min.x, bounds.value_min.y, z, uv_min.x, uv_min.y, c4);
  sloth_vibuffer_set_vert(vibuf, quad_v0_index + 1, bounds.value_max.x, bounds.value_min.y, z, uv_max.x, uv_min.y, c4);
  sloth_vibuffer_set_vert(vibuf, quad_v0_index + 2, bounds.value_max.x, bounds.value_max.y, z, uv_max.x, uv_max.y, c4);
  sloth_vibuffer_set_vert(vibuf, quad_v0_index + 3, bounds.value_min.x, bounds.value_max.y, z, uv_min.x, uv_max.y, c4);
}

Sloth_Function void
sloth_render_outline_ptc(Sloth_VIBuffer* vibuf, Sloth_Rect bounds, Sloth_R32 z, Sloth_R32 thickness, Sloth_V2 uv_min, Sloth_V2 uv_max, Sloth_U32 color)
{
  SLOTH_PROFILE_BEGIN;
  sloth_assert(vibuf != 0);
  
  // Outline is rendered like
  // 
  // t t t t t t << notice the overhangs
  // l         r
  // l         r
  // l         r
  // b b b b b b <<
  
  // TODO(rjf): I'm not sure I want the outline to 
  // extend beyond the bounds requested. Better to have
  // it an inset outline
  Sloth_V2 top_min, top_max;
  top_min.x = bounds.value_min.x;
  top_min.y = bounds.value_min.y;
  top_max.x = bounds.value_max.x;
  top_max.y = bounds.value_min.y + thickness;
  Sloth_Rect top;
  top.value_min = top_min;
  top.value_max = top_max;
  
  Sloth_V2 bot_min, bot_max;
  bot_min.x = bounds.value_min.x;
  bot_min.y = bounds.value_max.y - thickness;
  bot_max.x = bounds.value_max.x;
  bot_max.y = bounds.value_max.y;
  Sloth_Rect bot;
  bot.value_min = bot_min;
  bot.value_max = bot_max;
  
  Sloth_V2 rig_min, rig_max;
  rig_min.x = bounds.value_min.x;
  rig_min.y = bounds.value_min.y + thickness;
  rig_max.x = bounds.value_min.x + thickness;
  rig_max.y = bounds.value_max.y - thickness;
  Sloth_Rect rig;
  rig.value_min = rig_min;
  rig.value_max = rig_max;
  
  Sloth_V2 lef_min, lef_max;
  lef_min.x = bounds.value_max.x - thickness;
  lef_min.y = bounds.value_min.y + thickness;
  lef_max.x = bounds.value_max.x;
  lef_max.y = bounds.value_max.y - thickness;
  Sloth_Rect lef;
  lef.value_min = lef_min;
  lef.value_max = lef_max;
  
  // TODO(PS): uv_min and uv_max here are not correct
  // they need to be projected onto the outline
  sloth_render_quad_ptc(vibuf, top, z, uv_min, uv_max, color);
  sloth_render_quad_ptc(vibuf, bot, z, uv_min, uv_max, color);
  sloth_render_quad_ptc(vibuf, lef, z, uv_min, uv_max, color);
  sloth_render_quad_ptc(vibuf, rig, z, uv_min, uv_max, color);
}

Sloth_Function Sloth_Rect
sloth_render_get_glyph_bounds(Sloth_Glyph_Info glyph, Sloth_V2 at, Sloth_V2* new_at)
{
  SLOTH_PROFILE_BEGIN;
  
  Sloth_Rect bounds;
  bounds.value_min.x = at.x + glyph.glyph.lsb;
  bounds.value_min.y = at.y + glyph.glyph.baseline_offset_y;
  bounds.value_max = sloth_v2_add(bounds.value_min, sloth_make_v2((Sloth_R32)glyph.glyph.dst_width, (Sloth_R32)glyph.glyph.dst_height));
  if (new_at) {
    *new_at = at;
    new_at->x = sloth_floor_r32(at.x + glyph.glyph.x_advance);
  }
  return bounds;
}

Sloth_Function Sloth_Bool
sloth_render_clip_glyph_layout(Sloth_Ctx* sloth, Sloth_Glyph_Layout* glyph, Sloth_Rect text_bounds)
{
  SLOTH_PROFILE_BEGIN;
  
#if 1
  Sloth_V2 bounds_dim = sloth_rect_dim(glyph->bounds);
  Sloth_V2 uv_dim = sloth_rect_dim(glyph->info.uv);
  Sloth_Rect bounds_clipped, uv_clipped;
  if (!sloth_clip_rect_and_uv(text_bounds, glyph->bounds, glyph->info.uv, &bounds_clipped, &uv_clipped))
  {
    return false;
  }
  glyph->bounds = bounds_clipped;
  glyph->info.uv = uv_clipped;
#else
  Sloth_Rect bounds = glyph->bounds;
  
  // find nearest valid point for each extent that is within text_bounds
  Sloth_Rect clipped_bounds;
  clipped_bounds.value_min = sloth_rect_get_closest_point(text_bounds, bounds.value_min);
  clipped_bounds.value_max = sloth_rect_get_closest_point(text_bounds, bounds.value_max);
  
  // if those points are teh same, zero everything out return false (don't draw)
  Sloth_V2 clipped_dim = sloth_rect_dim(clipped_bounds);
  if (clipped_dim.x == 0 || clipped_dim.y == 0)
  {
    return false;
  }
  
  // get insets as percentage of overall width
  Sloth_V2 bounds_dim = sloth_rect_dim(glyph->bounds);
  Sloth_R32 inset_l_pct = (clipped_bounds.value_min.x - bounds.value_min.x) / bounds_dim.x;
  Sloth_R32 inset_r_pct = (clipped_bounds.value_max.x - bounds.value_max.x) / bounds_dim.x;
  Sloth_R32 inset_t_pct = (clipped_bounds.value_min.y - bounds.value_min.y) / bounds_dim.y;
  Sloth_R32 inset_b_pct = (clipped_bounds.value_max.y - bounds.value_max.y) / bounds_dim.y;
  
  // convert pct insets to be in terms of uv coordinates
  Sloth_V2 uv_dim = sloth_rect_dim(glyph->info.uv);
  Sloth_R32 inset_l_uv = inset_l_pct * uv_dim.x;
  Sloth_R32 inset_r_uv = inset_r_pct * uv_dim.x;
  Sloth_R32 inset_t_uv = inset_t_pct * uv_dim.y;
  Sloth_R32 inset_b_uv = inset_b_pct * uv_dim.y;
  
  // NOTE: I checked when I wrote this that this body
  // of operations doesn't modify the dimensions of 
  // clipped_bounds, meaning we don't also need to apply
  // some similar operation to the uvs
  clipped_bounds.value_min.x = sloth_floor_r32(clipped_bounds.value_min.x);
  clipped_bounds.value_min.y = sloth_floor_r32(clipped_bounds.value_min.y);
  clipped_bounds.value_max.x = sloth_floor_r32(clipped_bounds.value_max.x);
  clipped_bounds.value_max.y = sloth_floor_r32(clipped_bounds.value_max.y);
  
  glyph->bounds = clipped_bounds;
  
  glyph->info.uv.value_min.x += inset_l_uv;
  glyph->info.uv.value_min.y += inset_t_uv;
  glyph->info.uv.value_max.x += inset_r_uv;
  glyph->info.uv.value_max.y += inset_b_uv;
#endif
  
  // safety checks:
  // we should always be able to assume that the result of this operation
  // produces two rects that are smaller than the original
  Sloth_R32 epsilon = 0.01f;
  sloth_assert((sloth_rect_dim(glyph->bounds).x - bounds_dim.x) < epsilon);
  sloth_assert((sloth_rect_dim(glyph->bounds).y - bounds_dim.y) < epsilon);
  sloth_assert((sloth_rect_dim(glyph->info.uv).x - uv_dim.x) < epsilon);
  sloth_assert((sloth_rect_dim(glyph->info.uv).y - uv_dim.y) < epsilon);
  
  return true;
}

Sloth_Function void
sloth_render_shift_glyphs(Sloth_Glyph_Layout* glyphs, Sloth_U32 first, Sloth_U32 one_past_last, Sloth_U32 axis, Sloth_R32 shift)
{
  SLOTH_PROFILE_BEGIN;
  
  for (Sloth_U32 i = first; i < one_past_last; i++)
  {
    Sloth_Glyph_Layout* at = glyphs + i;
    at->bounds.value_min.E[axis] += shift;
    at->bounds.value_max.E[axis] += shift;
  }
}

Sloth_Function void
sloth_render_distribute_glyphs(Sloth_Glyph_Layout* glyphs, Sloth_U32 first, Sloth_U32 one_past_last, Sloth_U32 axis, Sloth_R32 space_between_glyphs)
{
  SLOTH_PROFILE_BEGIN;
  
  Sloth_Glyph_Layout* prev = glyphs + first;
  for (Sloth_U32 i = first + 1; i < one_past_last; i++)
  {
    Sloth_Glyph_Layout* at = glyphs + i;
    at->bounds.value_min.E[axis] = prev->bounds.value_max.E[axis] + space_between_glyphs;
    at->bounds.value_max.E[axis] = prev->bounds.value_max.E[axis] + space_between_glyphs;
    prev = at;
  }
}

Sloth_Function void
sloth_render_text_apply_align_center(Sloth_Widget* widget, Sloth_Glyph_Layout* glyphs, Sloth_U32 glyphs_cap, Sloth_Rect text_bounds)
{
  SLOTH_PROFILE_BEGIN;
  
  Sloth_R32 line_max_width = sloth_rect_dim(text_bounds).x;
  Sloth_U32 last_line_start_i = 0;
  Sloth_R32 last_line_start_x = glyphs[0].bounds.value_min.x;
  for (Sloth_U32 glyph_i = 1; glyph_i < glyphs_cap; glyph_i++)
  {
    if (!glyphs[glyph_i].is_line_start) continue;
    
    // process the line just completed
    Sloth_R32 last_line_end_x = glyphs[glyph_i - 1].bounds.value_max.x;
    Sloth_R32 last_line_w = last_line_end_x - last_line_start_x;
    Sloth_R32 last_line_room = line_max_width - last_line_w;
    //sloth_assert(last_line_room >= 0);
    Sloth_R32 last_line_offset = last_line_room / 2;
    sloth_render_shift_glyphs(glyphs, last_line_start_i, glyph_i, Sloth_Axis_X, last_line_offset);
    
    // start the new line
    last_line_start_i = glyph_i;
    last_line_start_x = glyphs[glyph_i].bounds.value_min.x;
  }
}

Sloth_Function void
sloth_render_text_apply_align_right(Sloth_Widget* widget, Sloth_Glyph_Layout* glyphs, Sloth_U32 glyphs_cap, Sloth_Rect text_bounds)
{
  SLOTH_PROFILE_BEGIN;
  
  Sloth_R32 line_max_width = sloth_rect_dim(text_bounds).x;
  Sloth_U32 last_line_start_i = 0;
  Sloth_R32 last_line_start_x = glyphs[0].bounds.value_min.x;
  for (Sloth_U32 glyph_i = 1; glyph_i < glyphs_cap; glyph_i++)
  {
    if (!glyphs[glyph_i].is_line_start) continue;
    
    // process the line just completed
    Sloth_R32 last_line_end_x = glyphs[glyph_i - 1].bounds.value_max.x;
    Sloth_R32 last_line_w = last_line_end_x - last_line_start_x;
    Sloth_R32 last_line_room = line_max_width - last_line_w;
    //sloth_assert(last_line_room >= 0);
    sloth_render_shift_glyphs(glyphs, last_line_start_i, glyph_i, Sloth_Axis_X, last_line_room);
    
    // start the new line
    last_line_start_i = glyph_i;
    last_line_start_x = glyphs[glyph_i].bounds.value_min.x;
  }
}

Sloth_Function Sloth_V2
sloth_layout_text_in_widget(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_Rect text_bounds)
{
  SLOTH_PROFILE_BEGIN;
  
  Sloth_V2 text_dim = SLOTH_ZII;
  
  Sloth_U32 last_line_break = 0;
  Sloth_Glyph_Layout* text = widget->text;
  
  Sloth_Font* active_font = 0;
  Sloth_R32 line_advance = 0;
  Sloth_V2 at; at.x = 0; at.y = 0;
  Sloth_U32 text_lines_count = 0;
  for (Sloth_U32 glyph_i = 0; glyph_i < widget->text_len; glyph_i++)
  {
    Sloth_Glyph_Layout* text_at = text + glyph_i;
    
    Sloth_V2 next_at;
    Sloth_Rect glyph_bounds;
    Sloth_Bool is_newline = sloth_glyph_id_matches_charcode(text_at->glyph_id, '\n');
    if (!is_newline)
    {
      text_at->info = sloth_lookup_glyph(sloth, text_at->glyph_id);
      text_at->color = widget->style.color_text;
      
      // The first glyph needs to be treated differently as it
      // will also offset to the first baseline
      if (glyph_i == 0) 
      {
        text_at->is_line_start = true;      
        active_font = sloth_glyph_to_font(sloth, text_at->glyph_id);
        if (active_font) {
          line_advance = active_font->metrics.line_height;
          at.y += active_font->metrics.to_baseline;
        } else {
          // TODO(PS): does line advance really make sense if there's no font
          // data we're drawing from? The characters wont be visible
          line_advance = text_at->info.glyph.dst_height;
        }
        text_lines_count += 1;
      }
      
      next_at = at;
      glyph_bounds = sloth_render_get_glyph_bounds(text_at->info, at, &next_at);
      text_at->bounds = glyph_bounds;
    }
    
    if(is_newline) 
    {
      next_at.x = 0;
      next_at.y += line_advance;
      text_lines_count += 1;
    }
    else if(glyph_bounds.value_max.x > text_bounds.value_max.x)
    {
      // look backwards to the last line-break glyph and move everything since to a new line
      Sloth_U32 line_break = glyph_i;
      for (Sloth_U32 lb_i = glyph_i; lb_i > last_line_break && lb_i < widget->text_len; lb_i--)
      {
        Sloth_Bool is_space = sloth_glyph_id_matches_charcode(widget->text[lb_i].glyph_id, ' ');
        Sloth_Bool is_newline = sloth_glyph_id_matches_charcode(widget->text[lb_i].glyph_id, '\n');
        if (is_space || is_newline)
        {
          line_break = lb_i + 1;
          break;
        }
      }
      
      if (!sloth_has_flag(widget->style.text_style, Sloth_TextStyle_NoWrapText))
      {
        at.x = 0;
        at.y += line_advance;
        text_lines_count += 1;
        
        if (line_break == last_line_break)
        {
          // since there was nowhere on this line acceptable for
          // a line break, advance the current glyph to the next
          // line since its the first glyph that will overflow
          line_break = glyph_i;
        }
        
        text[line_break].is_line_start = true;
        if (text[line_break].glyph_id.family != text[last_line_break].glyph_id.family)
        {
          active_font = sloth_glyph_to_font(sloth, text_at->glyph_id);
          if (active_font) {
            line_advance = active_font->metrics.line_height;
          } else {
            line_advance = text_at->info.glyph.dst_height;
          }
        }
        
        for (Sloth_U32 new_line_i = line_break; new_line_i <= glyph_i; new_line_i++)
        {
          Sloth_Glyph_Layout new_line_glyph = widget->text[new_line_i];
          Sloth_Rect new_bounds = sloth_render_get_glyph_bounds(new_line_glyph.info, at, &next_at);
          widget->text[new_line_i].bounds = new_bounds;
          at = next_at;         
        }
        last_line_break = line_break;
      }
      
      // TODO: stop creating glyphs if we're at a point where they won't
      // appear within text_bounds at all (ie you've moved to the next
      // line down, and the new glyph will get completely clipped).
      // NOTE: This will involve truncating the later iterations to layout
      // glyphs
    }
    
    at = next_at;
  }
  
  // Handle text alignment
  // This just adjusts the existing positions given
  // during the default layout step above 
  Sloth_Text_Style_Flags text_style = widget->style.text_style;
  // no action necessary for Align_Left
  if (sloth_has_flag(text_style, Sloth_TextStyle_Align_Center))
  {
    sloth_render_text_apply_align_center(widget, widget->text, widget->text_len, text_bounds);
  }
  else if (sloth_has_flag(text_style, Sloth_TextStyle_Align_Right))
  {
    sloth_render_text_apply_align_right(widget, widget->text, widget->text_len, text_bounds);
  }
  
  Sloth_V2 text_max = SLOTH_ZII;
  for (Sloth_U32 glyph_i = 0; glyph_i < widget->text_len; glyph_i++)
  {
    text_max.x = Sloth_Max(text_max.x, text[glyph_i].bounds.value_max.x);
    text_max.y = Sloth_Max(text_max.y, text[glyph_i].bounds.value_max.y);
  }
  text_dim.x = text_max.x - text_bounds.value_min.x;
  text_dim.y += (text_lines_count * line_advance);
  
  return text_dim;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_size_kind_text_contents_layout_text(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  
  // NOTE(PS): if this widget was specified as being sized by its children
  // but it doesnt have any, AND it has text contents, we want to treat
  // that text content as its children.
  // TODO(PS): There's probably a way to simplify this whole relationship
  if (widget->child_first == 0 && widget->text_len > 0)
  {
    if (widget->layout.width.kind == Sloth_SizeKind_ChildrenSum) {
      widget->layout.width = SLOTH_SIZE_TEXT_CONTENT;
    }
    if (widget->layout.height.kind == Sloth_SizeKind_ChildrenSum) {
      widget->layout.height = SLOTH_SIZE_TEXT_CONTENT;
    }
  }
  
  Sloth_Rect text_bounds = SLOTH_ZII;
  Sloth_Widget_Layout l = widget->layout;
  if (l.width.kind == Sloth_SizeKind_TextContent) 
  {
    text_bounds.value_max.x = Sloth_R32_Max;
    if (l.height.kind == Sloth_SizeKind_Pixels) {
      Sloth_R32 margin_t = sloth_size_evaluate_margin(widget, widget->layout.margin.top, 0);
      Sloth_R32 margin_b = sloth_size_evaluate_margin(widget, widget->layout.margin.bottom, 0);
      text_bounds.value_max.y = Sloth_Max(0, l.height.value - (margin_t + margin_b));
    } else {
      text_bounds.value_max.y = Sloth_R32_Max;
    }
  }
  else if (l.width.kind == Sloth_SizeKind_Pixels &&
      l.height.kind == Sloth_SizeKind_TextContent)
  {
    Sloth_R32 margin_l = sloth_size_evaluate_margin(widget, widget->layout.margin.left, 0);
    Sloth_R32 margin_r = sloth_size_evaluate_margin(widget, widget->layout.margin.right, 0);
    text_bounds.value_max.x = Sloth_Max(0, l.width.value - (margin_l + margin_r));
    text_bounds.value_max.y = Sloth_R32_Max;
  }
  else
  {
    return Sloth_TreeWalk_Continue; // will be handled later
  }
  
  widget->text_dim = sloth_layout_text_in_widget(sloth, widget, text_bounds);
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_percent_parent_width_layout_text(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Layout l = widget->layout;
  if (l.width.kind != Sloth_SizeKind_PercentOfParent) return Sloth_TreeWalk_Continue;
  if (l.height.kind != Sloth_SizeKind_TextContent) return Sloth_TreeWalk_Continue;
  
  Sloth_R32 margin = sloth_size_evaluate_margin(widget, widget->layout.margin.left, Sloth_Axis_X);
  margin += sloth_size_evaluate_margin(widget, widget->layout.margin.right, Sloth_Axis_X);
  
  Sloth_Rect text_bounds = SLOTH_ZII;
  text_bounds.value_max.x = Sloth_Max(0, widget->cached->dim.x - margin);
  text_bounds.value_max.y = Sloth_R32_Max;
  widget->text_dim = sloth_layout_text_in_widget(sloth, widget, text_bounds);
  
  return Sloth_TreeWalk_Continue;
  
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_child_sum_width_layout_text(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Layout l = widget->layout;
  if (l.width.kind != Sloth_SizeKind_ChildrenSum) return Sloth_TreeWalk_Continue;
  if (l.height.kind != Sloth_SizeKind_TextContent) return Sloth_TreeWalk_Continue;
  
  Sloth_R32 margin = sloth_size_evaluate_margin(widget, widget->layout.margin.left, Sloth_Axis_X);
  margin += sloth_size_evaluate_margin(widget, widget->layout.margin.right, Sloth_Axis_X);
  
  Sloth_Rect text_bounds = SLOTH_ZII;
  text_bounds.value_max.x = Sloth_Max(0, widget->cached->dim.x - margin);
  text_bounds.value_max.y = Sloth_R32_Max;
  widget->text_dim = sloth_layout_text_in_widget(sloth, widget, text_bounds);
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_known_size_layout_text(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Widget_Layout l = widget->layout;
  Sloth_Size_Kind wk = l.width.kind;
  Sloth_Size_Kind hk = l.height.kind;
  if (wk == Sloth_SizeKind_TextContent || hk == Sloth_SizeKind_TextContent) return Sloth_TreeWalk_Continue;
  
  Sloth_R32 margin_x = sloth_size_evaluate_margin(widget, widget->layout.margin.left, Sloth_Axis_X);
  margin_x += sloth_size_evaluate_margin(widget, widget->layout.margin.right, Sloth_Axis_X);
  
  Sloth_R32 margin_y = sloth_size_evaluate_margin(widget, widget->layout.margin.top, Sloth_Axis_Y);
  margin_y += sloth_size_evaluate_margin(widget, widget->layout.margin.bottom, Sloth_Axis_Y);
  
  Sloth_Rect text_bounds = SLOTH_ZII;
  text_bounds.value_max.x = Sloth_Max(0, widget->cached->dim.x - margin_x);
  text_bounds.value_max.y = Sloth_Max(0, widget->cached->dim.y - margin_y);
  
  widget->text_dim = sloth_layout_text_in_widget(sloth, widget, text_bounds);
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_measure_children(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (!widget->parent) return Sloth_TreeWalk_Continue;
  Sloth_V2 cb_min = widget->parent->cached->children_bounds_min;
  Sloth_V2 cb_max = widget->parent->cached->children_bounds_max;  
  Sloth_Widget_Cached* c = widget->cached;
  cb_min.x = Sloth_Min(c->offset.x,            cb_min.x);
  cb_min.y = Sloth_Min(c->offset.y,            cb_min.y);
  cb_max.x = Sloth_Max(c->offset.x + c->dim.x, cb_max.x);
  cb_max.y = Sloth_Max(c->offset.y + c->dim.y, cb_max.y);
  widget->parent->cached->children_bounds_min = cb_min;
  widget->parent->cached->children_bounds_max = cb_max;
  
  // Reset own children_bounds before evaluating children
  c->children_bounds_min.x = Sloth_R32_Max;
  c->children_bounds_min.y = Sloth_R32_Max;
  c->children_bounds_max.x = Sloth_R32_Min;
  c->children_bounds_max.y = Sloth_R32_Min;
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_offset_and_clip_text(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{  
  Sloth_V2 offset = widget->cached->offset;
  offset.x += sloth_size_evaluate_margin(widget, widget->layout.margin.left, Sloth_Axis_X);
  offset.y += sloth_size_evaluate_margin(widget, widget->layout.margin.top,  Sloth_Axis_Y);
  for (Sloth_U32 i = 0; i < widget->text_len; i++)
  {
    // offset
    widget->text[i].bounds.value_min.x += offset.x;
    widget->text[i].bounds.value_min.y += offset.y;
    widget->text[i].bounds.value_max.x += offset.x;
    widget->text[i].bounds.value_max.y += offset.y;
    
    // clip
    widget->text[i].draw = sloth_render_clip_glyph_layout(sloth, widget->text + i, widget->cached->bounds);
  }
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function void
sloth_render_text_in_widget(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_Rect text_bounds, Sloth_R32 z)
{
  Sloth_Glyph_ID id;
  id.value = 0;
  id.id[0] = 1;
  Sloth_Glyph_Info white = sloth_lookup_glyph(sloth, id);
  
  Sloth_U32 selection_line_start = 0;
  Sloth_Rect line_dim;
  Sloth_S32 selection_quad_v0 = -1;
  Sloth_U8 text_vibuf_family = 0;
  Sloth_VIBuffer* text_vibuf = 0;
  Sloth_VIBuffer* selection_vibuf = sloth->vibuffers + 0;
  for (Sloth_U32 i = 0; i < widget->text_len; i++)
  {
    if (!widget->text[i].draw) continue;
    
    // Text Selection Rendering
    Sloth_Glyph_Layout gl = widget->text[i];
    if (text_vibuf_family != gl.glyph_id.family || text_vibuf == 0)
    {
      text_vibuf = sloth_get_vibuffer_for_glyph(sloth, gl.glyph_id);
      text_vibuf_family = gl.glyph_id.family;
    }
    
    if (gl.selected) 
    {
      Sloth_Bool is_selection_start = selection_quad_v0;
      Sloth_Bool is_selected_line_start = gl.is_line_start;
      if (is_selection_start || is_selected_line_start) 
      {
        // if there's already a selection being processed, render that
        // selection before beginning this new one
        if (selection_quad_v0 >= 0) 
        {
          sloth_rect_expand(&line_dim, 4, 4, 4, 4);
          sloth_render_update_quad_ptc(selection_vibuf, selection_quad_v0, line_dim, z, white.uv.value_min, white.uv.value_max, 0x0088FFFF);
        }
        
        // push a new quad so that it's behind all subsequent glyphs on this line
        // this quad will be updated on subsequent iterations of this loop
        selection_line_start = i;
        line_dim = gl.bounds;
        selection_quad_v0 = (Sloth_S32)sloth_render_quad_ptc(selection_vibuf, gl.bounds, z, white.uv.value_min, white.uv.value_max, 0x0088FFFF);
      }
      else
      {
        // expand the existing selection
        line_dim.value_min.x = Sloth_Min(gl.bounds.value_min.x, line_dim.value_min.x);
        line_dim.value_min.y = Sloth_Min(gl.bounds.value_min.y, line_dim.value_min.y);
        line_dim.value_max.x = Sloth_Max(gl.bounds.value_max.x, line_dim.value_max.x);
        line_dim.value_max.y = Sloth_Max(gl.bounds.value_max.y, line_dim.value_max.y);
      }
    }
    else if (selection_quad_v0 != -1)
    {
      // if there's a selection being tracked but this glyph isn't
      // part of it, render that selection and stop tracking it
      sloth_rect_expand(&line_dim, 4, 4, 4, 4);
      sloth_render_update_quad_ptc(selection_vibuf, selection_quad_v0, line_dim, z, white.uv.value_min, white.uv.value_max, 0x0088FFFF);
      selection_quad_v0 = -1;
    }
    
    // render the glyph
    sloth_render_quad_ptc(text_vibuf, gl.bounds, z, gl.info.uv.value_min, gl.info.uv.value_max, gl.color);
  }
  
  // render any pending selection quad
  if (selection_quad_v0 != -1)
  {
    sloth_rect_expand(&line_dim, 4, 4, 4, 4);
    sloth_render_update_quad_ptc(selection_vibuf, selection_quad_v0, line_dim, z, white.uv.value_min, white.uv.value_max, 0x0088FFFF);
    selection_quad_v0 = -1;
  }
}

// Z Offsets
// for a given z depth and a given widget, that widgets
// elements should be rendered at the following offsets
// ie. the bg is at z_at + ((Sloth_R32)Sloth_ZOff_BG * z_step)
// and the next widget begins at z_at + ((Sloth_R32)Sloth_ZOff_Next * z_step)
//
// z_step can be calculated using total_depth / (num_widgets * Sloth_ZOff_Next)
enum {
  Sloth_ZOff_BG = 0,
  Sloth_ZOff_Text = 1,
  Sloth_ZOff_Outline = 2,
  Sloth_ZOff_Next
};

#if 1
#define Sloth_GetZOff(sloth_render_ctx, offset) (((sloth_render_ctx)->z_at) + ((Sloth_R32)(offset) * (sloth_render_ctx)->z_step))
#else
#define Sloth_GetZOff(sloth_render_ctx, offset) ((sloth_render_ctx)->z_at)
#endif

typedef struct Sloth_Render_Ctx Sloth_Render_Ctx;
struct Sloth_Render_Ctx
{
  Sloth_R32 z_step;
  Sloth_R32 z_at;
};

Sloth_Function Sloth_Tree_Walk_Result
sloth_render_cb(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  if (widget->style.draw_flags == Sloth_Draw_None) return Sloth_TreeWalk_Continue_SkipChildren;
  
  Sloth_Render_Ctx* rc = (Sloth_Render_Ctx*)user_data;
  
  Sloth_Rect bounds = widget->cached->bounds;
  
  // White glyph id
  Sloth_Glyph_ID white_id = SLOTH_ZII;
  white_id.id[0] = 1;
  
  // Z Depths
  Sloth_R32 z_bg      = Sloth_GetZOff(rc, Sloth_ZOff_BG);
  Sloth_R32 z_text    = Sloth_GetZOff(rc, Sloth_ZOff_Text);
  Sloth_R32 z_outline = Sloth_GetZOff(rc, Sloth_ZOff_Outline);
  rc->z_at            = Sloth_GetZOff(rc, Sloth_ZOff_Next);
  
  // Background
  Sloth_Glyph_ID bg_id = widget->style.bg_glyph;
  if (bg_id.value == 0) {
    sloth_zero_struct_(&bg_id);
    bg_id.id[0] = 1;
  }
  Sloth_Glyph_Info bg_glyph = sloth_lookup_glyph(sloth, bg_id);
  Sloth_V2 bg_uv_min = bg_glyph.uv.value_min;
  Sloth_V2 bg_uv_max = bg_glyph.uv.value_max;  
  Sloth_VIBuffer* vibuf = sloth_get_vibuffer_for_glyph(sloth, bg_id);
  if (vibuf) {
    sloth_render_quad_ptc(vibuf, bounds, z_bg, bg_uv_min, bg_uv_max, widget->style.color_bg);
  }
  
  // Text
  sloth_render_text_in_widget(sloth, widget, widget->cached->bounds, z_text);
  
  // Outline
  if (widget->style.outline_thickness > 0)
  {
    Sloth_R32 t = widget->style.outline_thickness;
    Sloth_U32 c = widget->style.color_outline;
    
    vibuf = sloth_get_vibuffer_for_glyph(sloth, white_id);
    if (vibuf) {
      // TODO(PS): This is incorrect - if there's an outline on a widget
      // that's being clipped, the outline will still be drawn all around 
      // the clipped rect. It should be hidden on at least one side
      // ie. if an outlined rect is scrolled half out of its parent
      sloth_render_outline_ptc(vibuf, widget->cached->bounds, z_outline, t, bg_uv_min, bg_uv_max, c);
    }
  }
  
  // Debug Outline
#define OUTLINE_HOT_AND_ACTIVE false
#if OUTLINE_HOT_AND_ACTIVE
  {
    Sloth_U32 color = 0x005555FF;
    if (sloth_ids_equal(sloth->hot_widget, widget->id)) {
      color = 0xFF00FFFF;
    } else if (sloth_ids_equal(sloth->active_widget, widget->id)) {
      color = 0x00FFFFFF;
    }
    Sloth_Glyph_Info white_glyph = sloth_lookup_glyph(sloth, white_id);
    vibuf = sloth_get_vibuffer_for_glyph(sloth, white_id);
    if (vibuf) {
      sloth_render_outline_ptc(vibuf, widget->cached->bounds, z_outline, 2, white_glyph.uv.value_min, white_glyph.uv.value_max, color);
    }
  }
#endif
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function Sloth_Tree_Walk_Result
sloth_find_hot_and_active(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* ud)
{
  if (sloth_has_flag(widget->input.flags, Sloth_WidgetInput_DoNotCaptureMouse)) return Sloth_TreeWalk_Continue;
  
  // Active
  if (sloth_mouse_button_is_down(sloth->mouse_button_l) &&
      sloth_rect_contains(widget->cached->bounds, sloth->mouse_down_pos))
  {
    sloth->active_widget = widget->id;
    for (Sloth_U32 glyph_i = 0; glyph_i < widget->text_len; glyph_i++)
    {
      Sloth_Glyph_Layout g = widget->text[glyph_i];
      if (sloth_rect_contains(g.bounds, sloth->mouse_down_pos)) {
        sloth->active_widget_selected_glyphs_first = glyph_i;
      }
      if (sloth_rect_contains(g.bounds, sloth->mouse_pos))
      {
        sloth->active_widget_selected_glyphs_one_past_last = glyph_i + 1;
      }
      
      // TODO(PS): what if they drag away from the widget?
    }
  }
  
  // Hot
  if (sloth_rect_contains(widget->cached->bounds, sloth->mouse_pos))
  {
    sloth->hot_widget = widget->id;
  }
  
  return Sloth_TreeWalk_Continue;
}

Sloth_Function void
sloth_frame_prepare(Sloth_Ctx* sloth, Sloth_Frame_Desc desc)
{
  SLOTH_PROFILE_BEGIN;
  sloth_assert(sloth->sentinel == SLOTH_DEBUG_DID_CALL_ADVANCE);
  
  // Update Sloth's Screen & Mouse Information
  sloth->screen_dim = desc.screen_dim;
  sloth->mouse_pos.x = desc.mouse_pos.x;
  sloth->mouse_pos.y = desc.mouse_pos.y;
  sloth->mouse_button_l = desc.mouse_button_l;
  sloth->mouse_button_r = desc.mouse_button_r;
  if (sloth_mouse_button_transitioned_down(desc.mouse_button_l))
  {
    sloth->mouse_down_pos = desc.mouse_pos;
  }
  
  // Reset Vertex Buffers
  for (Sloth_U32 vibuf_i = 0; vibuf_i < sloth->glyph_atlases_cap; vibuf_i++)
  {
    sloth_vibuffer_reset(sloth->vibuffers + vibuf_i);
  }
  
  // Handle this frames input on the visuals rendered at the end 
  // of last frame
  sloth->last_active_widget.value = 0;
  if (sloth->active_widget.value != 0) 
  {
    if (!sloth_ids_equal(sloth->active_widget, sloth->last_active_widget))
    {
      sloth->active_widget_selected_glyphs_first = 0;
      sloth->active_widget_selected_glyphs_one_past_last = 0;
    }
    sloth->last_active_widget = sloth->active_widget;
  }
  
  sloth->hot_widget.value = 0;
  sloth->active_widget.value = 0;
  sloth_tree_walk_preorder(sloth, sloth_find_hot_and_active, 0);
  
  // Fixup Tree
  sloth->widget_tree_next_child = sloth->widget_tree_root;
  sloth->widget_tree_root = 0;
  sloth->widget_tree_parent_cur = 0;
  
  sloth->widgets.len = 0;
  sloth->widgets.free_list = 0;
  
  sloth_arena_clear(&sloth->per_frame_memory);
  sloth_arena_clear(&sloth->scratch);
  
  sloth->sentinel = SLOTH_DEBUG_DID_CALL_PREPARE;
}

#ifdef DEBUG
#  define sloth_debug_validate_at_frame_advance(sloth) sloth_debug_validate_at_frame_advance_(sloth)
#else
#  define sloth_debug_validate_at_frame_advance(sloth)
#endif

Sloth_Function void
sloth_debug_validate_at_frame_advance_(Sloth_Ctx* sloth)
{
  if (sloth->widget_tree_parent_cur != sloth->widget_tree_root)
  {
    printf("\nSloth Error:\n");
    printf("You forgot to close one of your open widgets. Widget tree in invalid state during sloth_frame_advance\n");
    sloth_invalid_code_path;
  }
}

// This is responsible for:
// - updating each widget's cached sizes
// - laying out the next frames data
// - outputting the vertex and index buffers needed for the 
//   frame to be rendered
Sloth_Function void
sloth_frame_advance(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  sloth_assert(sloth->sentinel == SLOTH_DEBUG_DID_CALL_PREPARE);
  sloth_debug_validate_at_frame_advance(sloth);
  
  Sloth_Layout_Cache lc;
  
  // Update the atlas_texture if necessary
  Sloth_Glyph_Store store = sloth->glyph_store;
  Sloth_Renderer_Atlas_Updated* renderer_atlas_updated = sloth->renderer_atlas_updated;
  for (Sloth_U32 atlas_i = 0; atlas_i < sloth->glyph_atlases_cap; atlas_i++)
  {
    Sloth_Glyph_Atlas* atlas = sloth->glyph_atlases + atlas_i;
    if (atlas->dirty_state == Sloth_GlyphAtlas_Clean) continue;
    renderer_atlas_updated(sloth, atlas_i);
    atlas->dirty_state = Sloth_GlyphAtlas_Clean;
  }
  
  // TODO(PS): come back here and do better at cleaning breaking out
  // text related tasks
  // - Glyph Aquisition + Kerning + Ligatures
  // - Shaping - treat it all as one line, figure out spacing
  // - Layout - line breaks, etc.
  
  // Layout text for widgets that will rely on the size of their
  // text contents.
  sloth_tree_walk_preorder(sloth, sloth_size_kind_text_contents_layout_text, 0);
  
  // Update widget's cached sizes in the following ways:
  // 1. Preorder - Sloth_SizeKind_Pixels & Sloth_SizeKind_TextContent
  //    can be set outright, don't rely on other information
  lc.axis = 0; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_fixed_size, (Sloth_U8*)&lc);
  lc.axis = 1; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_fixed_size, (Sloth_U8*)&lc);
  
  // 2. Preorder - Calculate sizes that rely on parents
  lc.axis = 0; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_percent_parent, (Sloth_U8*)&lc);
  sloth_tree_walk_preorder(sloth, sloth_percent_parent_width_layout_text, 0);
  lc.axis = 1; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_percent_parent, (Sloth_U8*)&lc);
  
  // 3. Postorder - Calculate sizes that rely on size of children
  lc.axis = 0; sloth_tree_walk_postorder(sloth, sloth_size_fixup_cb_children_sum, (Sloth_U8*)&lc);
  sloth_tree_walk_preorder(sloth, sloth_child_sum_width_layout_text, 0);
  lc.axis = 1; sloth_tree_walk_postorder(sloth, sloth_size_fixup_cb_children_sum, (Sloth_U8*)&lc);
  
  // 4. Preorder - Handle any unhandled cases, including ones that
  //    might not have a neat solution. 
  lc.axis = 0; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_violations, (Sloth_U8*)&lc);
  lc.axis = 1; sloth_tree_walk_preorder(sloth, sloth_size_fixup_cb_violations, (Sloth_U8*)&lc);
  
  // Pass: Final Text Layout
  // Layout text for widgets didn't previously layout their text
  // for sizing purposes. This procedure also clips text for all widgets
  sloth_tree_walk_preorder(sloth, sloth_known_size_layout_text, 0);
  
  // Pass: Measure Children
  // Before clipping, we want to know the total size of each nodes children
  // This information is used in offsetting in the case where the offset
  // is a percent of that total size (as opposed to a static pixel value)
  sloth_tree_walk_preorder(sloth, sloth_measure_children, 0);
  
  // Pass: Set final bounding boxes for all widgets (preorder)
  lc.axis = 0; 
  lc.last_sibling_end = 0;
  sloth_tree_walk_preorder(sloth, sloth_layout_cb, (Sloth_U8*)&lc);
  lc.axis = 1; 
  lc.last_sibling_end = 0;
  sloth_tree_walk_preorder(sloth, sloth_layout_cb, (Sloth_U8*)&lc);
  
  // Pass: Clipping
  sloth_tree_walk_preorder(sloth, sloth_clip_cb, (Sloth_U8*)&lc);
  sloth_tree_walk_preorder(sloth, sloth_offset_and_clip_text, 0);
  
  // Pass: Widgets -> Vertex Buffers
  // Each vertex buffer is associated with a texture. 
  Sloth_Render_Ctx rc = SLOTH_ZII;
  Sloth_R32 z_depth = sloth->z_depth_min - sloth->z_depth_max;
  Sloth_R32 z_step_dir = z_depth >= 0 ? 1 : -1; // sign(z_depth)
  rc.z_step = z_depth / (Sloth_R32)(sloth->widgets.len * Sloth_ZOff_Next);
  rc.z_at = sloth->z_depth_max;
  sloth_tree_walk_preorder(sloth, sloth_render_cb, (Sloth_U8*)&rc);
  
  sloth->sentinel = SLOTH_DEBUG_DID_CALL_ADVANCE;
}

Sloth_Function void
sloth_vibuffer_set_vert(Sloth_VIBuffer* buf, Sloth_U32 vert_index, Sloth_R32 x, Sloth_R32 y, Sloth_R32 z, Sloth_R32 u, Sloth_R32 v, Sloth_V4 c)
{
  Sloth_U32 vi = vert_index * SLOTH_VERTEX_STRIDE;
  buf->verts[vi++] = x;
  buf->verts[vi++] = y;
  buf->verts[vi++] = z;
  buf->verts[vi++] = u;
  buf->verts[vi++] = v;
  buf->verts[vi++] = c.x;
  buf->verts[vi++] = c.y;
  buf->verts[vi++] = c.z;
  buf->verts[vi++] = c.w;
}

Sloth_Function Sloth_U32 
sloth_vibuffer_push_vert(Sloth_VIBuffer* buf, Sloth_R32 x, Sloth_R32 y, Sloth_R32 z, Sloth_R32 u, Sloth_R32 v, Sloth_V4 c)
{
  SLOTH_PROFILE_BEGIN;
  buf->verts = sloth_array_grow(buf->verts,
    buf->verts_len,
    &buf->verts_cap,
    SLOTH_VERTEX_STRIDE * 256,
    Sloth_R32);
  sloth_assert((buf->verts_len % SLOTH_VERTEX_STRIDE) == 0);
  
  Sloth_U32 vert_index = buf->verts_len / SLOTH_VERTEX_STRIDE;
  sloth_vibuffer_set_vert(buf, vert_index, x, y, z, u, v, c);
  buf->verts_len += SLOTH_VERTEX_STRIDE;
  sloth_assert((buf->verts_len % SLOTH_VERTEX_STRIDE) == 0);
  
  return vert_index;
}

Sloth_Function Sloth_U32 
sloth_vibuffer_push_tri(Sloth_VIBuffer* buf, Sloth_U32 a, Sloth_U32 b, Sloth_U32 c)
{
  SLOTH_PROFILE_BEGIN;
  buf->indices = sloth_array_grow(
      buf->indices,
    buf->indices_len,
    &buf->indices_cap,
    3 * (256 / 4),
    Sloth_U32
  );
  sloth_assert((buf->indices_len % 3) == 0);
  Sloth_U32 tri_index = buf->indices_len / 3;
  buf->indices[buf->indices_len++] = a;
  buf->indices[buf->indices_len++] = b;
  buf->indices[buf->indices_len++] = c;
  sloth_assert((buf->indices_len % 3) == 0);
  return tri_index;
}

Sloth_Function void
sloth_vibuffer_push_quad(Sloth_VIBuffer* buf, Sloth_U32 a, Sloth_U32 b, Sloth_U32 c, Sloth_U32 d)
{
  SLOTH_PROFILE_BEGIN;
  sloth_vibuffer_push_tri(buf, a, b, c);
  sloth_vibuffer_push_tri(buf, a, c, d);
}

Sloth_Function void
sloth_vibuffer_reset(Sloth_VIBuffer* buf)
{
  buf->verts_len = 0;
  buf->indices_len = 0;
}

Sloth_Function void
sloth_vibuffer_free(Sloth_VIBuffer* buf)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_U8* unused;
  unused = (Sloth_U8*)sloth_realloc_array(buf->verts, Sloth_R32, buf->verts_cap, 0);
  unused = (Sloth_U8*)sloth_realloc_array(buf->indices, Sloth_U32, buf->indices_cap, 0);
}

Sloth_Function void
sloth_ctx_free(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  sloth_widget_pool_free(&sloth->widgets);
  sloth_arena_free(&sloth->per_frame_memory);
  sloth_arena_free(&sloth->scratch);
  
  for (Sloth_U32 vibuf_i = 0; vibuf_i < sloth->glyph_atlases_cap; vibuf_i++)
  {
    sloth_vibuffer_free(sloth->vibuffers + vibuf_i);
  }
  
  for (Sloth_U32 atlas_i = 0; atlas_i < sloth->glyph_atlases_cap; atlas_i++)
  {
    Sloth_Glyph_Atlas* atlas = sloth->glyph_atlases + atlas_i;
    Sloth_U8* unused = sloth_realloc(atlas->data, atlas->dim * atlas->dim, 0);
  }
  
}

Sloth_Function void 
sloth_frame_render(Sloth_Ctx* sloth)
{
  Sloth_Renderer_Render* cb = sloth->renderer_render;
  if (!cb) return;
  
  for (Sloth_U32 i = 0; i < sloth->glyph_atlases_cap; i++)
  {
    cb(sloth, i);
  }
}

typedef struct Sloth_Tree_Print_Data Sloth_Tree_Print_Data;
struct Sloth_Tree_Print_Data
{
  Sloth_U32 indent;
  
  char* buffer;
  Sloth_U32 buffer_size;
  Sloth_U32 buffer_used;
};

#if 0
Sloth_Function void
sloth_widget_tree_print_cb(Sloth_Ctx* sloth, Sloth_Widget* widget, Sloth_U8* user_data)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Tree_Print_Data* d = (Sloth_Tree_Print_Data*)user_data;
  
  Sloth_U32 needed = sloth_snprintf(0, 0, "%.*s%s\n",
    d->indent * 2, "                                                ",
    widget->str);
  if (d->buffer_used + needed > d->buffer_size)
  {
    Sloth_U32 buffer_size = Sloth_Max(d->buffer_size * 2, 2048);
    d->buffer = (char*)sloth_realloc(d->buffer, d->buffer_size, buffer_size);
    d->buffer_size = buffer_size;
  }
  
  d->buffer_used += sloth_snprintf(d->buffer + d->buffer_used, 
    d->buffer_size - d->buffer_used, 
    "%.*s%s\n",
    d->indent * 2, "                                                ",
    widget->str);
  
  if (widget->child_first) {
    d->indent += 1;
  } else if (!widget->sibling_next) {
    d->indent -= 1;
  }
}

#endif

Sloth_Function char*
sloth_widget_tree_print(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Tree_Print_Data d = {
    .indent = 0,
  };
  //sloth_tree_walk_preorder(sloth, sloth_widget_tree_print_cb, (Sloth_U8*)&d);
  return d.buffer;
}

Sloth_Function void 
sloth_ctx_init(Sloth_Ctx* sloth)
{
  // set reasonable defaults if the user hasn't already
  if (sloth->z_depth_min == 0 && sloth->z_depth_min == sloth->z_depth_max) 
  {
    sloth->z_depth_min = -1;
    sloth->z_depth_max = 20;
  }
  
  { // Create a flat white glyph
    Sloth_U32 p[16];
    for (Sloth_U32 i = 0; i < 16; i++) { p[i] = 0xFFFFFFFF; }
    
    Sloth_Glyph_Desc gd = SLOTH_ZII;
    gd.family = 0;
    gd.id = 1;
    gd.format = Sloth_GlyphData_RGBA8;
    gd.stride = 4;
    gd.src_width = 4;
    gd.src_height = 4;
    gd.data = (Sloth_U8*)p;
    
    sloth_register_glyph(sloth, gd);
  }
}

///////////////////////////////////////////////////////
// USEFUL BUILDING BLOCKS
//
// NOTE: These aren't quite components themselves, but
// instead, wrap around more complex-to-build ui logic
// that lots of components might want to make use of

// Grids

typedef struct Sloth_Grid_Desc Sloth_Grid_Desc;
struct Sloth_Grid_Desc
{
  Sloth_U32 cols_count;
  Sloth_U32 rows_count;
  Sloth_Size col_gap;
  Sloth_Size row_gap;
  Sloth_Size cols[8];
  Sloth_Size rows[8];
  Sloth_Widget_Style style;
};

Sloth_Function Sloth_Widget_Desc
sloth_grid_desc_get_cell(Sloth_Grid_Desc grid, Sloth_U32 col, Sloth_U32 row)
{
  col = col % grid.cols_count;
  row = row % grid.rows_count;
  Sloth_Widget_Desc result = SLOTH_ZII;
  result.layout.width = grid.cols[col];
  result.layout.height = grid.rows[row];
  result.layout.margin.right = grid.col_gap;
  result.layout.margin.bottom = grid.row_gap;
  result.style = grid.style;
  return result;
}

// Text

Sloth_Function Sloth_Widget_Result
sloth_cmp_text_df(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Widget_Result result = sloth_push_widget_v(sloth, desc, fmt, args);
  sloth_pop_widget(sloth);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Widget_Result
sloth_cmp_text_f(Sloth_Ctx* sloth, Sloth_Text_Style_Flags text_style, Sloth_U32 color_text, char* fmt, ...)
{
  Sloth_Widget_Desc desc = SLOTH_ZII;
  desc.layout.width     = SLOTH_SIZE_TEXT_CONTENT;
  desc.layout.height    = SLOTH_SIZE_TEXT_CONTENT;
  desc.style.color_text = color_text;
  desc.style.text_style = text_style;
  desc.input.flags = Sloth_WidgetInput_TextSelectable;
  
  va_list args; va_start(args, fmt);
  Sloth_Widget_Result result = sloth_push_widget_v(sloth, desc, fmt, args);
  sloth_pop_widget_safe(sloth, result);
  va_end(args);
  
  return result;
}

// Spacers

// Constructs a widget that fills its parent along one dimension
// and has the other dimension specified by the size passed in
Sloth_Function void
sloth_cmp_space_axis(Sloth_Ctx* sloth, Sloth_Size size, Sloth_U32 on_axis)
{
  Sloth_U32 off_axis = sloth_other_axis(on_axis);
  
  Sloth_Widget_Desc desc = SLOTH_ZII;
  desc.layout.size[on_axis]  = size;
  desc.layout.size[off_axis] = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  desc.input.flags = Sloth_WidgetInput_DoNotCaptureMouse;
  
  // The id for this widget is constructed using the id of its
  // previous sibling. Since that widget should have had a unique
  // id, that will make this spacer unique
  Sloth_Widget* parent = sloth->widget_tree_parent_cur;
  Sloth_Widget* sibling_last = parent->child_last;
  
  Sloth_Widget_Result r = sloth_push_widget_f(sloth, desc, "##VSpaceAfter%u", sibling_last->id.value);
  sloth_pop_widget_safe(sloth, r);
}

Sloth_Function void
sloth_cmp_space_h(Sloth_Ctx* sloth, Sloth_Size size)
{
  sloth_cmp_space_axis(sloth, size, Sloth_Axis_X);
}

Sloth_Function void
sloth_cmp_space_v(Sloth_Ctx* sloth, Sloth_Size size)
{
  sloth_cmp_space_axis(sloth, size, Sloth_Axis_Y);
}


// Buttons

Sloth_Function Sloth_Bool
sloth_cmp_button_vd(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, va_list args)
{
  Sloth_Widget_Result wr = sloth_push_widget_v(sloth, desc, fmt, args);
  sloth_pop_widget(sloth);
  
  if (sloth_ids_equal(sloth->hot_widget, wr.widget->id))
  {
    wr.widget->style.color_outline = 0xFFFFFFFF;
  }
  
  Sloth_Bool result = wr.clicked;
  if (result) 
  {
    Sloth_U32 new_color_text = wr.widget->style.color_bg;
    wr.widget->style.color_bg   = wr.widget->style.color_text;
    wr.widget->style.color_text = new_color_text;
  }
  return result;
}

Sloth_Function Sloth_Bool
sloth_cmp_button_v(Sloth_Ctx* sloth, Sloth_U32 color_bg, Sloth_U32 color_text, Sloth_U32 color_outline, char* fmt, va_list args)
{
  Sloth_Widget_Desc desc = {
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
      .color_bg   = color_bg,
      .color_text = color_text,
      .color_outline = color_outline,
      .outline_thickness = 1,
      .text_style = Sloth_TextStyle_Align_Center,
    },
  };
  return sloth_cmp_button_vd(sloth, desc, fmt, args);
}

Sloth_Function Sloth_Bool
sloth_cmp_button_fd(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Bool result = sloth_cmp_button_vd(sloth, desc, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Bool
sloth_cmp_button_f(Sloth_Ctx* sloth, Sloth_U32 color_bg, Sloth_U32 color_text, Sloth_U32 color_outline, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Bool result = sloth_cmp_button_v(sloth, color_bg, color_text, color_outline, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Bool
sloth_cmp_toggle_v(Sloth_Ctx* sloth, Sloth_Bool state, Sloth_U32 color_bg, Sloth_U32 color_text, Sloth_U32 color_outline, char* fmt, va_list args)
{
  Sloth_Widget_Desc desc = {
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
      .color_bg   = state ? color_bg   : color_text,
      .color_text = state ? color_text : color_bg,
      .color_outline = color_outline,
      .outline_thickness = 1,
      .text_style = Sloth_TextStyle_Align_Center,
    },
  };
  Sloth_Widget_Result wr = sloth_push_widget_v(sloth, desc, fmt, args);
  sloth_pop_widget(sloth);
  
  if (sloth_ids_equal(sloth->hot_widget, wr.widget->id))
  {
    wr.widget->style.color_outline = 0xFFFFFFFF;
  }
  
  Sloth_Bool result = wr.clicked ? !state : state;
  return result;
}

Sloth_Function Sloth_Bool
sloth_cmp_toggle_f(Sloth_Ctx* sloth, Sloth_U32 state, Sloth_U32 color_bg, Sloth_U32 color_text, Sloth_U32 color_outline, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Bool result = sloth_cmp_toggle_v(sloth, state, color_bg, color_text, color_outline, fmt, args);
  va_end(args);
  return result;
}

// Sliders

typedef struct Sloth_Slider_Result Sloth_Slider_Result;
struct Sloth_Slider_Result
{
  Sloth_Widget_Result widget_result;
  Sloth_V2 value;
};

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_v(Sloth_Ctx* sloth, 
  Sloth_V2 range_min, Sloth_V2 range_max,
  Sloth_V2 value,
  Sloth_U8 x_root, Sloth_U8 y_root,
  Sloth_Widget_Desc desc,
  char* fmt, va_list args)
{
  // Slider ID
  Sloth_ID_Result idr = sloth_make_id_v(fmt, args);
  
  // @: Get the appropriate value if it exists, using value otherwise  
  Sloth_V2* value_cached = sloth_persistent_value_get(sloth, idr.id, Sloth_V2, &value);
  
  Sloth_Slider_Result result;
  result.value = *value_cached;
  
  // Unlerp from value to percentage of the range
  Sloth_R32 range_x = (range_max.x - range_min.x);
  Sloth_R32 range_y = (range_max.y - range_min.y);
  Sloth_R32 pct_x = range_x != 0 ? (result.value.x - range_min.x) / (range_max.x - range_min.x) : 0;
  Sloth_R32 pct_y = range_y != 0 ? (result.value.y - range_min.y) / (range_max.y - range_min.y) : 0;
  
  // Modify the input description
  Sloth_Layout_Position position = SLOTH_ZII;
  position.kind = Sloth_LayoutPosition_FixedInParent;
  position.at.E[Sloth_Axis_X].E[x_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_x);
  position.at.E[Sloth_Axis_Y].E[y_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_y);
  desc.layout.position = position;
  desc.input.flags |= Sloth_WidgetInput_Draggable;
  
  result.widget_result = sloth_push_widget_id(sloth, desc, idr.id);
  sloth_widget_allocate_text(sloth, result.widget_result, idr.display_len);
  sloth_widget_text_to_glyphs_append(sloth, result.widget_result, result.widget_result.widget->style.font, idr.formatted, idr.display_len);
  sloth_pop_widget(sloth);
  
  // NOTE(PS): These values are meaningless except when the widget
  // was just released or is being held down. But doing the calculation
  // here means we only have to calculate in one place
  Sloth_V2 dopp = result.widget_result.drag_offset_percent_parent;
  if (x_root == 1) dopp.x *= -1;
  if (y_root == 1) dopp.y *= -1;
  Sloth_R32 value_dx = sloth_lerp_r32(range_min.x, dopp.x, range_max.x);
  Sloth_R32 value_dy = sloth_lerp_r32(range_min.y, dopp.y, range_max.y);
  Sloth_R32 value_x0 = result.value.x + value_dx;
  Sloth_R32 value_y0 = result.value.y + value_dy;
  Sloth_R32 value_x = Sloth_Clamp(range_min.x, value_x0, range_max.x);
  Sloth_R32 value_y = Sloth_Clamp(range_min.y, value_y0, range_max.y);
  Sloth_V2 new_value;
  new_value.x = value_x;
  new_value.y = value_y;
  
  // Get the pct where the drag handle should be drawn
  dopp.x = range_x != 0 ? ((value_x - result.value.x) - range_min.x) / range_x : 0;
  dopp.y = range_y != 0 ? ((value_y - result.value.y) - range_min.y) / range_y : 0;
  Sloth_R32 pct_x1 = pct_x + dopp.x;
  Sloth_R32 pct_y1 = pct_y + dopp.y;
  
  result.value = new_value;    
  
  // If the widget is being dragged, draw a new widget at the drag position
  if (result.widget_result.held) 
  {
    result.widget_result.widget->style.draw_flags = Sloth_Draw_None;
    
    Sloth_Layout_Position drag_position = desc.layout.position;
    drag_position.at.E[Sloth_Axis_X].E[x_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_x1);
    drag_position.at.E[Sloth_Axis_Y].E[y_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_y1);
    
    Sloth_Widget_Desc drag_desc = desc;
    drag_desc.layout.position = drag_position;
    drag_desc.input.flags = Sloth_WidgetInput_DoNotCaptureMouse;
    
    // NOTE: The drag slider id is constructed from the id of its parent
    // so it should always be unique, provided the user creates a unique
    // id for its parent
    sloth_push_widget_f(sloth, drag_desc, "###slider_moving_%d", idr.id.value);
    sloth_pop_widget(sloth);
  }
  
  // If the widget was just released, update the value output
  if (result.widget_result.released)
  {
    // @: Update the appropriate value
    sloth_persistent_value_set(sloth, idr.id, Sloth_V2, &new_value);
    
    // Update the handle so it doesn't jump
    position.at.E[Sloth_Axis_X].E[x_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_x1);
    position.at.E[Sloth_Axis_Y].E[y_root] = SLOTH_SIZE_PERCENT_OF_PARENT(pct_y1);
    result.widget_result.widget->layout.position = position;
  }
  
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_f(Sloth_Ctx* sloth, 
  Sloth_V2 range_min, Sloth_V2 range_max,
  Sloth_V2 value,
  Sloth_U8 x_root, Sloth_U8 y_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_slider_v(sloth, range_min, range_max, value, x_root, y_root, desc, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_single_axis_v(Sloth_Ctx* sloth, Sloth_U8 axis,
  Sloth_R32 range_min, Sloth_R32 range_max,
  Sloth_R32 value,
  Sloth_U8 axis_root,
  Sloth_Widget_Desc desc,
  char* fmt, va_list args)
{
  Sloth_U8 axis_other = sloth_other_axis(axis);
  
  Sloth_V2 range_min_v2, range_max_v2, value_v2;
  range_min_v2.E[axis]       = range_min;  
  range_max_v2.E[axis]       = range_max;
  value_v2.E[axis]           = value;
  range_min_v2.E[axis_other] = 0;
  range_max_v2.E[axis_other] = 0;
  value_v2.E[axis_other]     = 0;
  
  Sloth_U8 axis_roots[2];
  axis_roots[axis]       = axis_root;
  axis_roots[axis_other] = 0;
  
  Sloth_Slider_Result result = sloth_cmp_slider_v(
      sloth, 
    range_min_v2, range_max_v2, 
    value_v2,
    axis_roots[Sloth_Axis_X], axis_roots[Sloth_Axis_Y],
    desc, fmt, args
  );
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_single_axis_f(Sloth_Ctx* sloth, Sloth_U8 axis,
  Sloth_R32 range_min, Sloth_R32 range_max,
  Sloth_R32 value,
  Sloth_U8 axis_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_slider_single_axis_v(
      sloth, axis, range_min, range_max, value, axis_root, desc, fmt, args
  );
  va_end(args);
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_x_f(Sloth_Ctx* sloth, 
  Sloth_R32 range_min, Sloth_R32 range_max,
  Sloth_R32 value,
  Sloth_U8 x_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_slider_single_axis_v(
      sloth, Sloth_Axis_X, range_min, range_max, value, x_root, desc, fmt, args
  );
  va_end(args);
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_slider_y_f(Sloth_Ctx* sloth, 
  Sloth_R32 range_min, Sloth_R32 range_max,
  Sloth_R32 value,
  Sloth_U8 y_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_slider_single_axis_v(
      sloth, Sloth_Axis_Y, range_min, range_max, value, y_root, desc, fmt, args
  );
  va_end(args);
  return result;
}

// Scroll Bars
// Scroll bars all work in the range 0 to 1

Sloth_Function Sloth_Slider_Result
sloth_cmp_scrollbar_axis_v(Sloth_Ctx* sloth,
  Sloth_U8 axis,
  Sloth_R32 value,
  Sloth_U8 axis_root,
  Sloth_Widget_Desc desc,
  char* fmt, va_list args)
{
  Sloth_Slider_Result result;
  
  Sloth_U8 other_axis = sloth_other_axis(axis);
  desc.layout.size[axis] = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  desc.layout.size[other_axis] = SLOTH_SIZE_PIXELS(8);
  desc.input.flags = Sloth_WidgetInput_DoNotCaptureMouse;
  
  Sloth_Widget_Result bg = sloth_push_widget_v(
      sloth, desc, fmt, args
  );
  {
    Sloth_Widget_Desc handle_desc = SLOTH_ZII;
    handle_desc.layout.width         = SLOTH_SIZE_PIXELS(8);
    handle_desc.layout.height        = SLOTH_SIZE_PIXELS(8);
    handle_desc.layout.position.kind = Sloth_LayoutPosition_FixedInParent;
    handle_desc.style.color_bg       = (0xFFFFFFFF - desc.style.color_bg) | 0x000000FF;
    result = sloth_cmp_slider_single_axis_f(sloth, axis, 0, 1, value, axis_root, handle_desc, 
      "###scrollbar_handle_%u", bg.widget->id.value);
  }
  sloth_pop_widget(sloth);
  
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_scrollbar_x_f(Sloth_Ctx* sloth,
  Sloth_R32 value,
  Sloth_U8 axis_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_scrollbar_axis_v(sloth, Sloth_Axis_X, value, axis_root, desc, fmt, args);
  va_end(args);
  return result;
}

Sloth_Function Sloth_Slider_Result
sloth_cmp_scrollbar_y_f(Sloth_Ctx* sloth,
  Sloth_R32 value,
  Sloth_U8 axis_root,
  Sloth_Widget_Desc desc,
  char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Slider_Result result = sloth_cmp_scrollbar_axis_v(sloth, Sloth_Axis_Y, value, axis_root, desc, fmt, args);
  va_end(args);
  return result;
}

// Scroll Area
// NOTE: This works by beginning the area and closing the area manually
// Any children of the area get their position adjusted during the close
// function

Sloth_Function Sloth_Widget_Result
sloth_cmp_push_scroll_area_f(Sloth_Ctx* sloth, Sloth_Widget_Desc desc, char* fmt, ...)
{
  va_list args; va_start(args, fmt);
  Sloth_Widget_Result result = sloth_push_widget_v(sloth, desc, fmt, args);
  Sloth_ID id = result.widget->id;
  va_end(args);
  
  Sloth_Widget_Desc y_scroll_area_desc = SLOTH_ZII;
  y_scroll_area_desc.style.color_bg = 0x550000FF;
  y_scroll_area_desc.layout.width = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  y_scroll_area_desc.layout.height = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  y_scroll_area_desc.input.flags = Sloth_WidgetInput_DoNotCaptureMouse;
  Sloth_Widget_Result y_scroll_area = sloth_push_widget_f(sloth, y_scroll_area_desc, "###y_scroll_area_%d", id.value);
  
  Sloth_Widget_Desc content_container_desc = SLOTH_ZII;
  content_container_desc.layout.width = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  content_container_desc.layout.height = SLOTH_SIZE_PERCENT_OF_PARENT(1);
  content_container_desc.input.flags = Sloth_WidgetInput_DoNotCaptureMouse;
  Sloth_Widget_Result content_container = sloth_push_widget_f(sloth, content_container_desc, "###content_container_%d", id.value);
  Sloth_Widget_Cached* cached = sloth_get_cached_data_for_id(sloth, content_container.widget->id);
  
  return result;
}

Sloth_Function void
sloth_cmp_pop_scroll_area(Sloth_Ctx* sloth, Sloth_Widget_Result* result)
{
  Sloth_ID id = result->widget->id;
  Sloth_Widget* content_container = result->widget->child_first->child_first;
  Sloth_ID content_container_id = content_container->id;
  Sloth_Widget_Cached* cached = sloth_get_cached_data_for_id(sloth, content_container_id);
  sloth_pop_widget(sloth); // content container
  
  Sloth_Widget_Desc base_scrollbar_desc = SLOTH_ZII;
  base_scrollbar_desc.style.color_bg = 0x000000FF;
  base_scrollbar_desc.layout.position.kind = Sloth_LayoutPosition_FixedInParent;
  
  // Y Scrollbar
  Sloth_Widget_Desc y_scrollbar_desc = base_scrollbar_desc;  
  y_scrollbar_desc.layout.position.right = SLOTH_SIZE_PIXELS(0);
  y_scrollbar_desc.layout.position.top   = SLOTH_SIZE_PIXELS(0);
  Sloth_Slider_Result ry = sloth_cmp_scrollbar_y_f(sloth, -cached->child_offset.y, 0, y_scrollbar_desc, "###y_scrollbar_handle_%d", id.value);
  cached->child_offset.y = -ry.value.y;
  
  sloth_pop_widget(sloth); // y scroll area
  
  // X Scrollbar
  Sloth_Widget_Desc x_scrollbar_desc = base_scrollbar_desc;  
  x_scrollbar_desc.layout.position.left   = SLOTH_SIZE_PIXELS(0);
  x_scrollbar_desc.layout.position.bottom = SLOTH_SIZE_PIXELS(0);
  Sloth_Slider_Result rx = sloth_cmp_scrollbar_x_f(sloth, cached->child_offset.x, 0, x_scrollbar_desc, "###x_scrollbar_%d", id.value);
  cached->child_offset.x = -rx.value.x;
  
  sloth_pop_widget(sloth); // the scroll area widget
}

///////////////////////////////////////////////////////
// FONT ATLAS HELPERS

#ifdef SLOTH_STBTT_ATLAS

#ifndef __STB_INCLUDE_STB_TRUETYPE_H__
#  error "You must include stb_truetype.h (and link to it, if not implementing in this compilation unit).
#endif

typedef struct Sloth_Stbtt_Font Sloth_Stbtt_Font;
struct Sloth_Stbtt_Font
{
  stbtt_fontinfo font;
  Sloth_R32 scale;
};

Sloth_Function Sloth_U8*
sloth_stbtt_font_init(Sloth_Ctx* sloth, Sloth_Font* font, Sloth_U8* font_memory, Sloth_U32 font_memory_size, Sloth_U32 font_index, Sloth_R32 pixel_height)
{
  Sloth_Stbtt_Font* result = (Sloth_Stbtt_Font*)sloth_realloc(0, 0, sizeof(Sloth_Stbtt_Font));
  sloth_zero_struct_(result);
  
  stbtt_InitFont(&result->font, font_memory, stbtt_GetFontOffsetForIndex(font_memory, font_index));
  result->scale = stbtt_ScaleForPixelHeight(&result->font, pixel_height);
  
  Sloth_S32 ascent, descent, line_gap;
  stbtt_GetFontVMetrics(&result->font, &ascent, &descent, &line_gap);
  font->metrics.line_height = (ascent - descent + line_gap) * result->scale;
  font->metrics.to_baseline = ascent * result->scale;
  
  return (Sloth_U8*)result;
}

Sloth_Function Sloth_Glyph_ID
sloth_stbtt_register_glyph(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 codepoint)
{
  Sloth_Font* font = sloth_font_get_(sloth, font_id);
  Sloth_Stbtt_Font* stb_font = (Sloth_Stbtt_Font*)font->renderer_data;
  
  Sloth_Glyph_Desc gd = SLOTH_ZII;
  gd.family = font->weights[font_id.weight_index].glyph_family;
  gd.id = codepoint;
  gd.format = Sloth_GlyphData_Alpha8;
  
  Sloth_S32 advance, lsb, x0, y0, x1, y1;
  Sloth_U8* bitmap = stbtt_GetCodepointBitmapSubpixel(&stb_font->font, 
    0, 
    stb_font->scale, 
    0, 
    0, 
    codepoint, 
    (Sloth_S32*)&gd.src_width, 
    (Sloth_S32*)&gd.src_height, 
    0, 
    0);
  stbtt_GetCodepointHMetrics(&stb_font->font, codepoint, &advance, &lsb);
  stbtt_GetCodepointBitmapBoxSubpixel(&stb_font->font, 
    codepoint, 
    stb_font->scale, 
    stb_font->scale, 
    0, 
    0, 
    &x0, 
    &y0, 
    &x1, 
    &y1);
  gd.dst_width  = gd.src_width / sloth->screen_dpi_scale;
  gd.dst_height = gd.src_height / sloth->screen_dpi_scale;
  gd.id = codepoint;
  gd.stride = gd.src_width;
  gd.data = bitmap;
  gd.cursor_to_glyph_start_xoff = stb_font->scale * (Sloth_R32)lsb;
  gd.cursor_to_next_glyph = stb_font->scale * (Sloth_R32)advance;
  gd.baseline_offset_y = y0;
  
  Sloth_Glyph_ID result = sloth_register_glyph(sloth, gd);
  
  // TODO(PS): It might be better if we don't free the bitmap each time
  // but use a persistent backbuffer that gets saved in something like
  // a Sloth_Stbtt_Ctx thing?
  stbtt_FreeBitmap(bitmap, 0);
  
  return result;
}

Sloth_Function void
sloth_stbtt_init(Sloth_Ctx* sloth)
{
  sloth->font_renderer_load_font = sloth_stbtt_font_init;
  sloth->font_renderer_register_glyph = sloth_stbtt_register_glyph;
}

#endif // SLOTH_STBTT_ATLAS

#ifdef SLOTH_FREETYPE_ATLAS

typedef struct Sloth_FT2_Ctx Sloth_FT2_Ctx;
struct Sloth_FT2_Ctx
{
  FT_Library ft;
};

typedef struct Sloth_FT2_Font Sloth_FT2_Font;
struct Sloth_FT2_Font
{
  FT_Face ft_face;
};


Sloth_Function Sloth_U8*
sloth_ft2_font_init(Sloth_Ctx* sloth, Sloth_Font* font, Sloth_U8* font_memory, Sloth_U32 font_memory_size, Sloth_U32 font_index, Sloth_R32 pixel_height)
{
  FT_Error error = 0;
  Sloth_FT2_Ctx* ft_ctx = (Sloth_FT2_Ctx*)sloth->font_renderer_data;
  if (ft_ctx == 0)
  {
    ft_ctx = (Sloth_FT2_Ctx*)sloth_realloc(0, 0, sizeof(Sloth_FT2_Ctx));
    error = FT_Init_FreeType(&ft_ctx->ft);
    sloth_assert(!error);
    sloth->font_renderer_data = (Sloth_U8*)ft_ctx;
  }
  
  Sloth_FT2_Font* result = (Sloth_FT2_Font*)sloth_realloc(0, 0, sizeof(Sloth_FT2_Font));
  error = FT_New_Memory_Face(ft_ctx->ft, font_memory, font_memory_size, font_index, &result->ft_face);
  sloth_assert(!error);
  
  error = FT_Set_Pixel_Sizes(result->ft_face, 0, pixel_height * sloth->screen_dpi_scale);
  Sloth_U32 line_height = (result->ft_face->size->metrics.ascender - result->ft_face->size->metrics.descender);  
  line_height = line_height >> 6;
  font->metrics.line_height = (Sloth_R32)line_height;
  Sloth_U32 to_baseline = (Sloth_U32)result->ft_face->size->metrics.ascender;
  to_baseline = to_baseline >> 6;
  font->metrics.to_baseline = (Sloth_R32)to_baseline;
  
  return (Sloth_U8*)result;
}

// TODO(PS): This currently doesn't do anything with different font weights
Sloth_Function Sloth_Glyph_ID
sloth_ft2_register_glyph(Sloth_Ctx* sloth, Sloth_Font_ID font_id, Sloth_U32 codepoint)
{
  Sloth_FT2_Ctx* ft_ctx = (Sloth_FT2_Ctx*)sloth->font_renderer_data;
  sloth_assert(ft_ctx != 0);
  
  Sloth_Font* font = sloth_font_get_(sloth, font_id);
  Sloth_FT2_Font* ft_font = (Sloth_FT2_Font*)font->renderer_data;
  
  FT_Error error = FT_Set_Pixel_Sizes(ft_font->ft_face, 0, font->metrics.pixel_height * sloth->screen_dpi_scale);
  
  FT_UInt glyph_index = FT_Get_Char_Index(ft_font->ft_face, codepoint);
  if (glyph_index == 0) {
    printf("Error: Undefined Character Code - %u\n", codepoint);
    sloth_invalid_code_path;
  }
  
  FT_Int32 load_flags = 0;
  error = FT_Load_Glyph(ft_font->ft_face, glyph_index, load_flags);
  sloth_assert(!error);
  
  // Ensure the glyph is rendered
  if (ft_font->ft_face->glyph->format != FT_GLYPH_FORMAT_BITMAP)
  {
    error = FT_Render_Glyph(ft_font->ft_face->glyph, FT_RENDER_MODE_NORMAL);
    sloth_assert(!error);
  }
  
  FT_GlyphSlot slot = ft_font->ft_face->glyph;
  Sloth_Glyph_Desc gd = SLOTH_ZII;
  gd.family = font->weights[font_id.weight_index].glyph_family;
  gd.id = codepoint;
  gd.format = Sloth_GlyphData_Alpha8;
  gd.src_width  = slot->bitmap.width;
  gd.src_height = slot->bitmap.rows;
  gd.dst_width  = slot->bitmap.width;
  gd.dst_height = slot->bitmap.rows;
  gd.stride = slot->bitmap.pitch;
  gd.data   = slot->bitmap.buffer;
  gd.cursor_to_glyph_start_xoff = slot->bitmap_left;
  gd.cursor_to_next_glyph = (slot->advance.x >> 6); 
  gd.baseline_offset_y = -slot->bitmap_top;
  
  Sloth_Glyph_ID result = sloth_register_glyph(sloth, gd);
  return result;
}

Sloth_Function void
sloth_ft2_init(Sloth_Ctx* sloth)
{
  sloth->font_renderer_load_font = sloth_ft2_font_init;
  sloth->font_renderer_register_glyph = sloth_ft2_register_glyph;
}

#endif

///////////////////////////////////////////////////////
// RENDERING IMPLEMENTATIONS

#ifdef SLOTH_SOKOL_RENDERER

#ifndef SOKOL_GFX_INCLUDED
#  error "The sloth.h sokol renderer backend requires sokol_gfx.h to be included first."
#endif

#include "./sloth_sokol_shader.glsl.h"

typedef struct Sloth_Sokol_Texture Sloth_Sokol_Texture;
struct Sloth_Sokol_Texture
{
  sg_image id;
  Sloth_U32 dim;
};

typedef struct Sloth_Sokol_Data Sloth_Sokol_Data;
struct Sloth_Sokol_Data
{
  sg_bindings* pass_bindings;
  Sloth_Sokol_Texture* atlas_textures;
  Sloth_U32 atlas_textures_cap;
  
  sg_pass_action pass_action;
  sg_pipeline pip;
  
  Sloth_U32 bind_quad_cap;
};

Sloth_Function void
sloth_render_sokol_buffers_create(Sloth_Sokol_Data* sd, sg_bindings* pass_bind, Sloth_U32 quads)
{
  if (pass_bind->vertex_buffers[0].id != 0) 
  {
    sg_destroy_buffer(pass_bind->vertex_buffers[0]);
    pass_bind->vertex_buffers[0].id = 0;
  }
  if (pass_bind->index_buffer.id != 0) 
  {
    sg_destroy_buffer(pass_bind->index_buffer);
    pass_bind->index_buffer.id = 0;
  }
  
  sg_buffer_desc vbd = SLOTH_ZII;
  vbd.usage = SG_USAGE_STREAM;
  vbd.data.size = quads * 4 * sizeof(Sloth_R32);
  vbd.label = "sloth sokol vertices";
  
  sg_buffer_desc ibd = SLOTH_ZII;
  ibd.usage = SG_USAGE_STREAM;
  ibd.type = SG_BUFFERTYPE_INDEXBUFFER;
  ibd.data.size = quads * 6 * sizeof(Sloth_U32);
  ibd.label = "sloth sokol indices";
  
  pass_bind->vertex_buffers[0] = sg_make_buffer(&vbd);
  pass_bind->index_buffer = sg_make_buffer(&ibd);
  
  // TODO(PS): this needs to be per sg_bindings
  // maybe just modify sg_bindings? Or wrap it in a Sloth_Sokol_Bindings
  sd->bind_quad_cap = quads;
}

Sloth_Function void
sloth_renderer_sokol_atlas_updated(Sloth_Ctx* sloth, Sloth_U32 atlas_index)
{
  Sloth_Sokol_Data* sd = (Sloth_Sokol_Data*)sloth->render_data;
  sloth_assert(sd != 0);
  
  // Resize texture array if needed
  if (atlas_index >= sd->atlas_textures_cap)
  {
    Sloth_U32 new_cap = sd->atlas_textures_cap * 2;
    if (new_cap == 0) new_cap = SLOTH_GLYPH_ATLASES_MIN_CAP;
    
    Sloth_U32 old_size = sizeof(sg_bindings) * sd->atlas_textures_cap;
    Sloth_U32 new_size = sizeof(sg_bindings) * new_cap;
    sd->pass_bindings = (sg_bindings*)sloth_realloc(sd->pass_bindings, old_size, new_size);
    for (Sloth_U32 i = sd->atlas_textures_cap; i < new_cap; i++) {
      sloth_zero_struct_(&sd->pass_bindings[i]);
      sloth_render_sokol_buffers_create(sd, sd->pass_bindings + i, 1024);
    }
    sd->atlas_textures_cap = new_cap;
  }
  
  Sloth_Glyph_Store store = sloth->glyph_store;
  Sloth_Glyph_Atlas* atlas = sloth->glyph_atlases + atlas_index;
  Sloth_U32 atlas_dim = atlas->dim;
  sg_bindings* pass_bind = sd->pass_bindings + atlas_index;
  
  // Grow the existing texture if needed
  if (pass_bind->fs_images[SLOT_tex].id == 0 || atlas->dirty_state == Sloth_GlyphAtlas_Dirty_Grow) 
  {
    // create 
    sg_filter filter = SG_FILTER_NEAREST; // SG_FILTER_LINEAR
    sg_image_desc atlas_texture_desc = {
      .width  = atlas_dim,
      .height = atlas_dim,
      .usage = SG_USAGE_DYNAMIC,
      .min_filter = filter,
      .mag_filter = filter,
      .label = "sloth atlas texture",
    };
    sg_image new_atlas_texture = sg_make_image(&atlas_texture_desc);
    
    // Destroy an existing texture if there is one
    sg_image old_atlas_texture = pass_bind->fs_images[SLOT_tex];
    if (old_atlas_texture.id != 0) sg_destroy_image(old_atlas_texture);
    
    pass_bind->fs_images[SLOT_tex] = new_atlas_texture;
    atlas->dirty_state = Sloth_GlyphAtlas_Dirty_UpdateData;
  }
  
  if (atlas->dirty_state == Sloth_GlyphAtlas_Dirty_UpdateData)
  {
    // TODO(PS): This wont compile in c++
    // just update the data of the existing texture since
    // the new data still fits
    sg_image_data data = {
      .subimage[0][0] = {
        .ptr = (const char*)atlas->data,
        .size = atlas_dim * atlas_dim * sizeof(Sloth_U32),
      },
    };
    
    sg_update_image(pass_bind->fs_images[SLOT_tex], &data);
  }
}

Sloth_Function void
sloth_sokol_frame_prepare(Sloth_Ctx* sloth, Sloth_Frame_Desc* fd)
{
  fd->screen_dim = (Sloth_V2){
    .x = sapp_widthf(),
    .y = sapp_heightf(),
  };
  fd->high_dpi = sapp_high_dpi();
  fd->dpi_scale = sapp_dpi_scale();
  sloth_frame_prepare(sloth, *fd);
}

Sloth_Function void
sloth_sokol_event(Sloth_Frame_Desc* fd, const sapp_event* event)
{
  switch (event->type)
  {
    case SAPP_EVENTTYPE_MOUSE_MOVE:
    {
      fd->mouse_pos.x = event->mouse_x;
      fd->mouse_pos.y = event->mouse_y;
    } break;
    
    case SAPP_EVENTTYPE_MOUSE_DOWN:
    {
      fd->mouse_button_l |= Sloth_MouseState_IsDown;
    } break;
    
    case SAPP_EVENTTYPE_MOUSE_UP:
    {
      fd->mouse_button_l = Sloth_MouseState_WasDown;
    } break;
    
    default: {} break;
  }
}

Sloth_Function void
sloth_render_sokol_(Sloth_Ctx* sloth, Sloth_U32 pass_index, Sloth_U32 width, Sloth_U32 height, Sloth_R32 dpi_scale)
{
  SLOTH_PROFILE_BEGIN;
  Sloth_Sokol_Data* sd = (Sloth_Sokol_Data*)sloth->render_data;
  Sloth_VIBuffer* vibuf = sloth->vibuffers + pass_index;
  if (vibuf->verts_len == 0 || vibuf->indices_len == 0) return;
  
  sg_bindings* pass_bind = sd->pass_bindings + pass_index;
  
  // TODO(PS): probably make this togglable at runtime?
#define DRAW_ATLAS_OVER_ALL 0
#if DRAW_ATLAS_OVER_ALL
  Sloth_Glyph_Atlas* atlas = sloth->glyph_atlases + pass_index;
  Sloth_Rect tex_bounds;
  tex_bounds.value_min.x = 10;
  tex_bounds.value_min.y = 10;
  tex_bounds.value_max.x = 10 + atlas->dim;
  tex_bounds.value_max.y = 10 + atlas->dim;
  Sloth_V2 uv_min, uv_max;
  uv_min.x = 0; uv_min.y = 0;
  uv_max.x = 1; uv_max.y = 1;
  sloth_render_quad_ptc(vibuf, tex_bounds, 0, uv_min, uv_max, 0xFFFFFFFF);
#endif
  
  // Update the bindings
  if (vibuf->verts_len > sd->bind_quad_cap * 4) {
    Sloth_U32 new_cap = sd->bind_quad_cap * 2;
    while (new_cap * 4 < vibuf->verts_len) new_cap *= 2;
    sloth_render_sokol_buffers_create(sd, pass_bind, new_cap);
  }
  
  // TODO: Check if we are about to push a range bigger than the 
  // buffer's size. If we are, we need to create a new buffer and
  // free the old one
  sg_range vertex_range;
  vertex_range.ptr = (const void*)vibuf->verts;
  vertex_range.size = vibuf->verts_len * sizeof(Sloth_R32);
  sg_update_buffer(pass_bind->vertex_buffers[0], (const sg_range*)&vertex_range);
  
  sg_range index_range;
  index_range.ptr = (const void*)vibuf->indices;
  index_range.size = vibuf->indices_len * sizeof(Sloth_U32);
  sg_update_buffer(pass_bind->index_buffer, (const sg_range*)&index_range);
  
  // Draw the Frame
  if (vibuf->indices_len > 0)
  {
    sg_apply_pipeline(sd->pip);
    sg_apply_bindings(pass_bind);
    
    // Calculate Orthographic Projection Matrix
    Sloth_R32 left = 0;
    Sloth_R32 right = width;
    Sloth_R32 bottom = height;
    Sloth_R32 top = 0;
    Sloth_R32 near = -sloth->z_depth_min;
    Sloth_R32 far = -sloth->z_depth_max;
    sloth_viz_vs_params_t sloth_viz_vs_params2 = SLOTH_ZII;
    sloth_viz_vs_params2.mvp[0][0] = 2.0f / (right - left);
    sloth_viz_vs_params2.mvp[1][1] = 2.0f / (top - bottom);
    sloth_viz_vs_params2.mvp[2][2] = 2.0f / (near - far);
    sloth_viz_vs_params2.mvp[3][0] = (left + right) / (left - right);
    sloth_viz_vs_params2.mvp[3][1] = (bottom + top) / (bottom - top);
    sloth_viz_vs_params2.mvp[3][2] = (far + near) / (near - far);
    sloth_viz_vs_params2.mvp[3][3] = 1;
    
    sg_apply_uniforms(SG_SHADERSTAGE_VS, 
      SLOT_sloth_viz_vs_params, 
      &SG_RANGE(sloth_viz_vs_params2));
    
    sg_draw(0, vibuf->indices_len, 1);
  }
}

Sloth_Function void
sloth_render_sokol(Sloth_Ctx* sloth, Sloth_U32 pass_index)
{
  SLOTH_PROFILE_BEGIN;
  return sloth_render_sokol_(sloth, pass_index, sloth->screen_dim.x, sloth->screen_dim.y, sloth->screen_dpi_scale);
}

Sloth_Function void
sloth_sokol_init(Sloth_Ctx* sloth)
{
  SLOTH_PROFILE_BEGIN;
  sloth->render_data = sloth_realloc(sloth->render_data, 0, sizeof(Sloth_Sokol_Data));
  sloth->screen_dpi_scale = sapp_dpi_scale();
  sloth->renderer_atlas_updated = sloth_renderer_sokol_atlas_updated;
  sloth->renderer_render = sloth_render_sokol;
  
  Sloth_Sokol_Data* sd = (Sloth_Sokol_Data*)sloth->render_data;
  sloth_zero_struct_(sd);
  
  sg_pass_action pass_action = SLOTH_ZII;
  pass_action.colors[0].action = SG_ACTION_CLEAR;
  pass_action.colors[0].value.r = 0;
  pass_action.colors[0].value.g = 0;
  pass_action.colors[0].value.b = 0;
  pass_action.colors[0].value.a = 1;
  sd->pass_action = pass_action;
  
  sg_pipeline_desc pd = SLOTH_ZII;
  pd.shader = sg_make_shader(sloth_viz_shader_desc(sg_query_backend()));
  pd.index_type = SG_INDEXTYPE_UINT32;
  pd.layout.attrs[ATTR_sloth_viz_vs_position].format = SG_VERTEXFORMAT_FLOAT3;
  pd.layout.attrs[ATTR_sloth_viz_vs_uv].format       = SG_VERTEXFORMAT_FLOAT2;
  pd.layout.attrs[ATTR_sloth_viz_vs_color].format    = SG_VERTEXFORMAT_FLOAT4;
  pd.label = "sloth sokol pipeline";
  pd.colors[0].blend.enabled = true;
  pd.colors[0].blend.src_factor_rgb = SG_BLENDFACTOR_SRC_ALPHA;
  pd.colors[0].blend.dst_factor_rgb = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  pd.colors[0].blend.src_factor_alpha = SG_BLENDFACTOR_SRC_ALPHA;
  pd.colors[0].blend.dst_factor_alpha = SG_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
  pd.depth.compare = SG_COMPAREFUNC_LESS_EQUAL,
  pd.depth.write_enabled = true;
  sd->pip = sg_make_pipeline(&pd);
  
  if (sloth->z_depth_min == 0 && sloth->z_depth_max == 0)
  {
    sloth->z_depth_min = -1;
    sloth->z_depth_max = 100;
  }
}

#endif // SLOTH_SOKOL_RENDERER
#endif // SLOTH_IMPLEMENTATION

/*
Copyright 2022 Peter Slattery

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
