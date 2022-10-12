# SLOTH 
A maxmimal immediate mode interface written in the style of stb-libs.

## General Notes to Expand On Later

### Renderer Agnostic
### Font Library Agnostic
### Asset System Agnostic
using callbacks, sloth allows the user to define how assets get loaded into the system and how they get rendered to screen. Sloth simply manages Texture IDs and returns to the renderer a vertex buffer tied to a particular texture for rendering.

### Expressive Layout and Styling
Unlike CSS, Sloth differentiates between layout and style. 
Layout is responsible for allocating a quad within which a widget will appear.
Style is responsible for drawing within that quad. 
- The goal is to eventually be able to supply a fragment shader to a particular widget which will be called on just that widget's quad. It's unclear how we will be able to pass custom information ie. textures to that shader at the moment.

### Animation
- we could provide a generic animation interface. An animation is just a struct like
struct Sloth_Animation_Target
{
  Sloth_U8* value_ptr;
  Sloth_Animation_Value_Kind value_kind; // one of R32, S32, etc.
  
  // union container for all animatable properties
  Sloth_Animation_Value value_start;
  Sloth_Animation_Value value_end; 
};

struct Sloth_Animation
{
  Sloth_R32 time_start;
  Sloth_R32 time_end;
  Sloth_Animation_Curve curve;
  Sloth_Animation_Target targets[8];
};
This way, we could specify animations on groups of properties using the same curve/timing information.

- We need a way to manage these animations that still functions within the immediate mode interface. Calling code could look like

Sloth_Widget_Result r = sloth_push_widget_f(sloth, ...);
sloth_install_animation(sloth, r, animation_desc);
sloth_pop_widget(sloth);

and the animation gets tracked with the Sloth_Widget_Cached data. 
