// This file may be redistributed and modified only under the terms of
// the GNU General Public License (See COPYING for details).
// Copyright (C) 2001 - 2005 Simon Goodall, University of Southampton

// $Id: GL.cpp,v 1.123 2005-05-16 21:15:55 simon Exp $

#include <SDL/SDL.h>

#ifdef __APPLE__
    #include <SDL_image/SDL_image.h>
#else
    #include <SDL/SDL_image.h>
#endif


#include <sage/sage.h>
#include <sage/GL.h>
#include <sage/GLU.h>

#include <unistd.h>

#include <varconf/Config.h>
#include <wfmath/quaternion.h>
#include <wfmath/vector.h>
#include <Eris/Entity.h>

#include "RenderSystem.h"

#include "common/Log.h"
#include "common/Utility.h"

#include "src/Calendar.h"
#include "Camera.h"
#include "src/Console.h"
#include "src/Exception.h"
#include "Frustum.h"
#include "Graphics.h"
#include "loaders/ModelSystem.h"
#include "loaders/Model.h"
#include "loaders/ModelHandler.h"
#include "loaders/ModelRecord.h"
#include "loaders/ObjectRecord.h"
#include "src/System.h"
#include "src/WorldEntity.h"

#include "GL.h"

#include "environment/Environment.h"
#include "common/Mesh.h"

#include "src/sear_icon.xpm"

#ifdef USE_MMGR
  #include "common/mmgr.h"
#endif

  // Consts
  static const int sleep_time = 5000;

  static const std::string font_texture = "ui_font";
  static const std::string TEXTURE_splash_texture = "ui_splash_screen";
  
  // Config key strings
  
  static const std::string KEY_use_textures = "render_use_textures";
  static const std::string KEY_use_lighting = "render_use_lighting";
  static const std::string KEY_show_fps = "render_show_fps";
  static const std::string KEY_use_stencil = "render_use_stencil";

  static const std::string KEY_character_light_kc = "character_light_kc";
  static const std::string KEY_character_light_kl = "character_light_kl";
  static const std::string KEY_character_light_kq = "character_light_kq";
  
  static const std::string KEY_character_light_ambient_red = "character_light_ambient_red";
  static const std::string KEY_character_light_ambient_blue = "character_light_ambient_blue";
  static const std::string KEY_character_light_ambient_green = "character_light_ambient_green";
  static const std::string KEY_character_light_ambient_alpha = "character_light_ambient_alpha";

  static const std::string KEY_character_light_diffuse_red = "character_light_diffuse_red";
  static const std::string KEY_character_light_diffuse_blue = "character_light_diffuse_blue";
  static const std::string KEY_character_light_diffuse_green = "character_light_diffuse_green";
  static const std::string KEY_character_light_diffuse_alpha = "character_light_diffuse_alpha";
  
  static const std::string KEY_character_light_specular_red = "character_light_specular_red";
  static const std::string KEY_character_light_specular_blue = "character_light_specular_blue";
  static const std::string KEY_character_light_specular_green = "character_light_specular_green";
  static const std::string KEY_character_light_specular_alpha = "character_light_specular_alpha";
  
  static const std::string KEY_sun_light_kc = "sun_light_kc";
  static const std::string KEY_sun_light_kl = "sun_light_kl";
  static const std::string KEY_sun_light_kq = "sun_light_kq";
  
  static const std::string KEY_sun_light_specular_red = "sun_light_specular_red";
  static const std::string KEY_sun_light_specular_blue = "sun_light_specular_blue";
  static const std::string KEY_sun_light_specular_green = "sun_light_specular_green";
  static const std::string KEY_sun_light_specular_alpha = "sun_light_specular_alpha";

  static const std::string KEY_lower_frame_rate_bound = "lower_frame_rate_bound";
  static const std::string KEY_upper_frame_rate_bound = "upper_frame_rate_bound";
  
  static const std::string KEY_speech_offset_x = "speech_offset_x";
  static const std::string KEY_speech_offset_y = "speech_offset_y";
  static const std::string KEY_speech_offset_z = "speech_offset_z";
  
  static const std::string KEY_fog_start = "fog_start";
  static const std::string KEY_fog_end = "fog_end";
	
  static const std::string KEY_near_clip = "near_clip";
  static const std::string KEY_far_clip_dist = "far_clip_dist";
  static const std::string KEY_texture_scale = "texture_scale";
  
  // Default config values
  static const float DEFAULT_character_light_kc = 0.5f;
  static const float DEFAULT_character_light_kl = 0.2f;
  static const float DEFAULT_character_light_kq = 0.15f;

  static const float DEFAULT_character_light_ambient_red = 0.0f;
  static const float DEFAULT_character_light_ambient_green = 0.0f;
  static const float DEFAULT_character_light_ambient_blue = 0.0f;
  static const float DEFAULT_character_light_ambient_alpha = 0.0f;
  
  static const float DEFAULT_character_light_diffuse_red = 1.0f;
  static const float DEFAULT_character_light_diffuse_green = 1.0f;
  static const float DEFAULT_character_light_diffuse_blue = 0.9f;
  static const float DEFAULT_character_light_diffuse_alpha = 0.0f;

  static const float DEFAULT_character_light_specular_red = 0.0f;
  static const float DEFAULT_character_light_specular_green = 0.0f;
  static const float DEFAULT_character_light_specular_blue = 0.0f;
  static const float DEFAULT_character_light_specular_alpha = 0.0f;
  
  static const float DEFAULT_sun_light_kc = 1.0f;
  static const float DEFAULT_sun_light_kl = 0.0f;
  static const float DEFAULT_sun_light_kq = 0.0f;

  static const float DEFAULT_sun_light_specular_red = 0.0f;
  static const float DEFAULT_sun_light_specular_green = 0.0f;
  static const float DEFAULT_sun_light_specular_blue = 0.0f;
  static const float DEFAULT_sun_light_specular_alpha = 0.0f;

  static const float DEFAULT_use_textures = true;
  static const float DEFAULT_use_lighting = true;
  static const float DEFAULT_show_fps = true;
  static const float DEFAULT_use_stencil = true;

  static const float DEFAULT_lower_frame_rate_bound = 25.0f;
  static const float DEFAULT_upper_frame_rate_bound = 30.0f;

  static const float DEFAULT_speech_offset_x = 0.0f;
  static const float DEFAULT_speech_offset_y = 0.0f;
  static const float DEFAULT_speech_offset_z = 0.0f;

  static const float DEFAULT_fog_start = 100.0f;
  static const float DEFAULT_fog_end = 150.0f;
  static const float DEFAULT_near_clip = 0.1f;
  static const float DEFAULT_far_clip_dist = 1000.0f;
  static const float DEFAULT_texture_scale = 10.0f;

static bool use_ext_compiled_vertex_array = false;
static bool use_multitexturing = false;
static std::string FONT = "font";
static std::string UI = "ui";
static std::string SPLASH = "splash";
static std::string DEFAULT = "default";
static std::string DEFAULT_TEXTURE = "default_texture";
static std::string DEFAULT_FONT = "default_font";
static std::string HALO = "halo";
static std::string MASK = "_mask";
static std::string RENDER = "render";
	
static GLfloat halo_colour[4] = {1.0f, 0.0f, 1.0f, 1.0f};
static GLfloat activeNameColour[] = { 1.0f, 0.75f, 0.2f, 1.0f};
static GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
static GLfloat black[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//static GLfloat red[] =   { 1.0f, 0.0f, 0.0f, 1.0f };
static GLfloat yellow[] =  { 0.0f, 1.0f, 1.0f, 1.0f };
static GLfloat blue[] =  { 0.0f, 0.0f, 1.0f, 1.0f };
static GLfloat blackLight[]    = { 0.0f,  0.0f, 0.0f, 1.0f };


#ifdef DEBUG
static const bool debug = true;
#else
static const bool debug = false;
#endif

namespace Sear {

bool GL::createWindow(unsigned int width, unsigned int height, bool fullscreen) {
  m_graphics = RenderSystem::getInstance().getGraphics();
  // Destroy the existing window
  if (m_screen != NULL) destroyWindow();
  
  if (debug) std::cout << "GL: Creating Window" << std::endl;
  // Set new window size etc..
  m_width = width;
  m_height = height;
  m_fullscreen = fullscreen;

  // TODO check return values
  SDL_InitSubSystem(SDL_INIT_VIDEO);
  SDL_EnableUNICODE(1);


  //Request Open GL window attributes
  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5 );
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16 );
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
  // Only request stencil if specified
  if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1 );
  }


  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
    return false;
  }                                                                                
  /* Check is there are any modes available */
  if (debug) {
    // TODO is list leaked?
    SDL_Rect **videoModes = SDL_ListModes(NULL, SDL_FULLSCREEN|SDL_HWSURFACE);
    if (videoModes == 0) {
      printf("No modes available!\n");
    } else if (videoModes == (SDL_Rect **)-1) {
      printf("All resolutions available.\n");
    } else{
      /* Print valid modes */
      printf("Available Modes\n");
      for(int i=0; videoModes[i]; ++i) {
        float aspect = (float)videoModes[i]->w / (float)videoModes[i]->h;
        printf("  %d x %d -- %f\n", videoModes[i]->w, videoModes[i]->h, aspect);
      }
    }
    fflush(stdout);
  }
  //Create Window
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  int bpp = info->vfmt->BitsPerPixel;
  if (m_fullscreen) flags |= SDL_FULLSCREEN;
  if (!(m_width && m_height)) {
    std::cerr << "Invalid resolution: " << m_width << " x " << m_height << std::endl;
    return false;
  }
  if (debug) std::cout << "Setting video to " << m_width << " x " << m_height << std::endl;

   // TODO:these are probably leaked, however freeing them often causes a segfault!
  SDL_Surface *icon = IMG_ReadXPMFromArray(sear_icon_xpm);
  SDL_WM_SetIcon(icon, NULL);
  SDL_FreeSurface(icon);


  //Is this the correct way to free a window?
  m_screen = SDL_SetVideoMode(m_width, m_height, bpp, flags);
  if (m_screen == NULL ) {
    std::cerr << "Unable to set " << m_width << " x " << m_height << " video: " << SDL_GetError() << std::endl;
    return false;
  }
  // Check OpenGL flags
  int value;
  if (debug) {
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &value);
    Log::writeLog(std::string("Red Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &value);
    Log::writeLog(std::string("Blue Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &value);
    Log::writeLog(std::string("Green Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_DEPTH_SIZE, &value);
    Log::writeLog(std::string("Depth Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
    Log::writeLog(std::string("Stencil Size: ") + string_fmt(value), Log::LOG_DEFAULT);
    SDL_GL_GetAttribute(SDL_GL_DOUBLEBUFFER, &value);
    Log::writeLog(std::string("Double Buffer: ") + string_fmt(value), Log::LOG_DEFAULT);
  }

  SDL_GL_GetAttribute(SDL_GL_STENCIL_SIZE, &value);
                                                                                
    std::string vendor = string_fmt(glGetString(GL_VENDOR));
    std::string renderer = string_fmt(glGetString(GL_RENDERER));
    // TODO - CHECK OPENGL VERSION
    std::string version = string_fmt(glGetString(GL_VERSION));
    std::string extensions = string_fmt(glGetString(GL_EXTENSIONS));
                                                                                
//  if (debug) {
    Log::writeLog(std::string("GL_VENDER: ") + vendor, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_RENDERER: ") + renderer, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_VERSION: ") + version, Log::LOG_DEFAULT);
    Log::writeLog(std::string("GL_EXTENSIONS: ") + extensions, Log::LOG_DEFAULT);
//  }
                                                                                
  // These will be empty if there was a problem initialising the driver
  if (vendor.empty() || renderer.empty()) {
    std::cerr << "Error with OpenGL system" << std::endl;
    return false;
  }
                                                                                
  //RenderSystem::getInstance().getStateManager()->initGL();
                                                                                
  initLighting();
  // TODO: initialisation need to go into system?
  setupStates();
  if (debug) checkError();
  glLineWidth(4.0f);
                                                                                
  //TODO: this needs to go into the set viewport method
  //Check for divide by 0
  if (height == 0) height = 1;
//  glLineWidth(2.0f);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f); // Colour used to clear window
  glClearDepth(1.0); // Enables Clearing Of The Depth Buffer
  glClear(GL_DEPTH_BUFFER_BIT);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
 // glDisable(GL_DITHER);
                                                                                
  setViewMode(PERSPECTIVE);
  setupExtensions();

//  initFont();

  buildColourSet();
  if (debug) std::cout << "Window created" << std::endl << std::flush;
  return true;
}
void GL::destroyWindow() {
  RenderSystem::getInstance().invalidate();
  if (m_screen) {
    SDL_FreeSurface(m_screen);
    m_screen = NULL;
  }
  SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void GL::toggleFullscreen() {
  m_fullscreen = !m_fullscreen;
  // If fullscreen fails, create a new window with the fullscreen flag (un)set
  if (!SDL_WM_ToggleFullScreen(m_screen)) {
    destroyWindow();
    RenderSystem::getInstance().invalidate();
    Environment::getInstance().invalidate();
    createWindow(m_width, m_height, m_fullscreen);
    RenderSystem::getInstance().invalidate();
    Environment::getInstance().invalidate();
  }
}
void GL::checkError() {
  GLenum err = glGetError();
  std::string msg;
  switch (err) {
    case GL_NO_ERROR: break;
    case GL_INVALID_ENUM: msg = "GL Error: Invalid enum!"; break;
    case GL_INVALID_VALUE: msg = "GL Error: Invalid value!"; break;
    case GL_INVALID_OPERATION: msg = "GL Error: Invalid operation!"; break;
    case GL_STACK_OVERFLOW: msg = "GL Error: Stack overflow!"; break;
    case GL_STACK_UNDERFLOW: msg = "GL Error: Stack Underflow!"; break;
    case GL_OUT_OF_MEMORY: msg = "GL Error: Out of memory!"; break;
    default: msg = std::string("GL Error: Unknown Error: ") +  string_fmt(err); break;
  }
  if (!msg.empty()) std::cerr << msg << std::endl;
}   


void GL::setupExtensions() {
  sage_init();
//  sage_ext[GL_ARB_VERTEX_BUFFER_OBJECT] = false;
 
  m_use_sgis_generate_mipmap = sage_ext[GL_SGIS_GENERATE_MIPMAP];
  use_multitexturing = sage_ext[GL_ARB_MULTITEXTURE];

  if (use_multitexturing) {
    if (debug) std::cout << "Using arb_multitexture" << std::endl;
  }

  if (use_ext_compiled_vertex_array) {
    if (debug) std::cout << "Using use_ext_compiled_vertex_array" << std::endl;
  }
}
                                                                                

inline WorldEntity *GL::getSelectedID(unsigned int i) {

  if (i >= NUM_COLOURS) return NULL; // Check for array out of bounds
  return m_entityArray[i]; // Return stored entity pointer
}

void GL::nextColour(WorldEntity *we) {
  m_entityArray[m_colour_index] = we; // Store entity in array slot
  glColor3ubv(m_colourArray[m_colour_index]); // Set colour from appropriate index
  ++m_colour_index; // Increment counter for next pass
}

void GL::selectTerrainColour(WorldEntity * we) {
  nextColour(we);
}

void GL::buildColourSet() {
  // Get the bits info from OpenGL
  glGetIntegerv (GL_RED_BITS, &m_redBits);
  glGetIntegerv (GL_GREEN_BITS, &m_greenBits);
  glGetIntegerv (GL_BLUE_BITS, &m_blueBits);

  m_redBits = m_greenBits = m_blueBits = 3;

  // Create masks
  m_redMask = makeMask(m_redBits);
  m_greenMask = makeMask(m_greenBits);
  m_blueMask = makeMask(m_blueBits);
  // Calculate shifts
  m_redShift =   m_greenBits + m_blueBits;
  m_greenShift =  m_blueBits;
  m_blueShift = 0;
  
  // Pre-Calculate each colour and store in array
  unsigned int shift;
  for (unsigned int indx = 0; indx < NUM_COLOURS; ++indx) {
    // Get value from 0 -> 2^num_bits
    GLubyte red = (indx & (m_redMask << m_redShift)) >> (m_redShift);
    GLubyte green = (indx & (m_greenMask << m_greenShift)) >> (m_greenShift);
    GLubyte blue = (indx & (m_blueMask << m_blueShift)) >> (m_blueShift);
    // Bit shift to MSB format
    // Warning, there is an overflow here if shift is negative
    //  Should only happen if num bits > num bits in glubyte
    shift = 8 - m_redBits;
    if (shift > 0) red <<= shift;
    else if (shift < 0) red >>= shift;
    shift = 8 - m_greenBits;
    if (shift > 0) green <<= shift;
    else if (shift < 0) green >>= shift;
    shift = 8 - m_blueBits;
    if (shift > 0) blue <<= shift;
    else if (shift < 0) blue >>= shift;
//    GLubyte red = (indx & (m_redMask << m_redShift)) << (8 - m_redShift - m_redBits);
//    GLubyte green = (indx & (m_greenMask << m_greenShift)) << (8 - m_greenShift-m_greenBits);
//    GLubyte blue = (indx & (m_blueMask << m_blueShift)) << (8 - m_blueShift - m_blueBits);
    m_colourArray[indx][0] = red;
    m_colourArray[indx][1] = green;
    m_colourArray[indx][2] = blue;
  }
}

GL::GL() :
  m_fontInitialised(false),
  m_width(0), m_height(0),
  m_fullscreen(false),
  m_screen(NULL),
  m_system(System::instance()),
  m_graphics(NULL),
  m_fov(RENDER_FOV),
  m_near_clip(RENDER_NEAR_CLIP),
  m_far_clip_dist(100.0f),
  m_base(0),
  m_font_id(-1),
  m_splash_id(-1),
  m_activeEntity(NULL),
  m_x_pos(0), m_y_pos(0),
  m_speech_offset_x(0.0f),
  m_speech_offset_y(0.0f),
  m_speech_offset_z(0.0f),
  m_fog_start(100.0f),
  m_fog_end(150.0f),
  m_light_level(1.0f),
  m_colour_index(0),
  m_redBits(0), m_greenBits(0), m_blueBits(0),
  m_redMask(0), m_greenMask(0), m_blueMask(0),
  m_redShift(0), m_greenShift(0), m_blueShift(0),
  m_use_sgis_generate_mipmap(false),
  m_multi_texture_mode(false),
  m_initialised(false)
{
  memset(m_entityArray, 0, NUM_COLOURS * sizeof(WorldEntity*));
}


GL::~GL() {
  assert(m_initialised == false);
//  if (m_initialised) shutdown();
}

void GL::shutdown() {
  assert(m_initialised == true);
//  if (!m_initialised) return;
  if (debug) std::cout << "GL: Shutdown" << std::endl;

  invalidate();

  m_initialised = false;
}

void GL::init() {
  assert(m_initialised == false);
//  if (m_initialised) shutdown();
  if (debug) std::cout << "GL: Initialise" << std::endl;
  // Most of this should be elsewhere
  System::instance()->getGeneral().sigsv.connect(SigC::slot(*this, &GL::varconf_callback));

  m_initialised = true;
}

void GL::invalidate() {
  // Clear font display list
  shutdownFont();
  setupStates();
  //
}

void GL::initLighting() {
  if (debug) Log::writeLog("Render: initialising lighting", Log::LOG_DEFAULT);
  float gambient[4] = {0.1f, 0.1f,0.1f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT,gambient);
  // Light values and coordinates
           
  // Setup and enable light 0
  glLightfv(GL_LIGHT0, GL_AMBIENT, m_lights[LIGHT_CHARACTER].ambient);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, m_lights[LIGHT_CHARACTER].diffuse);
  glLightfv(GL_LIGHT0, GL_SPECULAR, m_lights[LIGHT_CHARACTER].specular);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_constant);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_linear);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, m_lights[LIGHT_CHARACTER].attenuation_quadratic);
  glEnable(GL_LIGHT0);
  
  glLightfv(GL_LIGHT1, GL_AMBIENT, blackLight);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, blackLight);
  glLightfv(GL_LIGHT1, GL_SPECULAR, blackLight);
  
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, m_lights[LIGHT_SUN].attenuation_constant);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, m_lights[LIGHT_SUN].attenuation_linear);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, m_lights[LIGHT_SUN].attenuation_quadratic);
  glEnable(GL_LIGHT1);
}

void GL::initFont() {
  float cx; // Holds Our X Character Coord
  float cy; // Holds Our Y Character Coord
  if (debug) Log::writeLog("Render: Initailising Fonts", Log::LOG_DEFAULT);
  m_base=glGenLists(256); // Creating 256 Display Lists
  m_font_id = RenderSystem::getInstance().requestTexture(DEFAULT_FONT);
  RenderSystem::getInstance().switchTexture(m_font_id);
  for (int loop=0; loop<256; ++loop) {
    cx=(float)(loop%16)/16.0f; // X Position Of Current Character
    cy=(float)(loop/16)/16.0f; // Y Position Of Current Character
    glNewList(m_base+loop,GL_COMPILE); // Start Building A List
    glBegin(GL_QUADS); // Use A Quad For Each Character
      glTexCoord2f(cx,1-cy-0.0625f); // Texture Coord (Bottom Left)
      glVertex2i(0,0); // Vertex Coord (Bottom Left)
      glTexCoord2f(cx,1-cy); // Texture Coord (Top Left)
      glVertex2i(0,16); // Vertex Coord (Top Left)
      glTexCoord2f(cx+0.0625f,1-cy); // Texture Coord (Top Right)
      glVertex2i(16,16); // Vertex Coord (Top Right)
      glTexCoord2f(cx+0.0625f,1-cy-0.0625f); // Texture Coord (Bottom Right)
      glVertex2i(16,0);       // Vertex Coord (Bottom Right)
    glEnd(); // Done Building Our Quad (Character)
    glTranslated(10,0,0); // Move To The Right Of The Character
    glEndList(); // Done Building The Display List
  }// Loop Until All 256 Are Built
  m_fontInitialised = true;
}

void GL::shutdownFont() {
  if (debug) Log::writeLog("Render: Shutting down fonts", Log::LOG_DEFAULT);
  glDeleteLists(m_base,256); // Delete All 256 Display Lists
  m_fontInitialised = false;
}

void GL::print(int x, int y, const char * string, int set) {
  if (!m_fontInitialised) initFont();
  if (set > 1) set = 1;
  RenderSystem::getInstance().switchTexture(m_font_id);
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Projection Matrix
  glOrtho(0, m_width, 0 , m_height, -1, 1); // Set Up An Ortho Screen
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPushMatrix();
  glLoadIdentity(); // Reset The Modelview Matrix
  glTranslated(x,y,0); // Position The Text (0,0 - Bottom Left)
  glListBase(m_base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
  glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
  glPopMatrix(); // Restore The Old Projection Matrix
}

void GL::print3D(const char *string, int set) {
  if (!m_fontInitialised) initFont();
  if (set > 1) set = 1;
  RenderSystem::getInstance().switchTexture(m_font_id);
  glPushMatrix();
  glListBase(m_base-32+(128*set)); // Choose The Font Set (0 or 1)
  glCallLists(strlen(string),GL_BYTE,string); // Write The Text To The Screen
  glPopMatrix(); // Restore The Old Projection Matrix
}

inline void GL::newLine() {
  glTranslatef(0.0f,  ( FONT_HEIGHT) , 0.0f);
}

void GL::drawTextRect(int x, int y, int width, int height, int texture) {
  RenderSystem::getInstance().switchTexture(texture);
  setViewMode(ORTHOGRAPHIC);
  // TODO: make into arrays?
  glBegin(GL_QUADS);
    glTexCoord2i(0, 0);
    glVertex2i(x, y);
    glTexCoord2i(0, 1);
    glVertex2i(x, y + height);
    glTexCoord2i(1, 1);
    glVertex2i(x + width, y + height);
    glTexCoord2i(1, 0);
    glVertex2i(x + width, y);
  glEnd();
  setViewMode(PERSPECTIVE);
}

void GL::procEvent(int x, int y) {
  // No need to perform checks until we are in the game world
  if (!m_system->checkState(SYS_IN_WORLD)) return;
  GLubyte i[3];
  glClear(GL_COLOR_BUFFER_BIT);
  RenderSystem::getInstance().drawScene(true, 0);
  m_x_pos = x;
  y = m_height - y;
  m_y_pos = y;
  glReadPixels(x, y, 1, 1, GL_RGB , GL_UNSIGNED_BYTE, &i);

// TODO pre-cache 8 - bits?
  
  GLubyte red = i[0] >> (8 - m_redBits);// & m_redMask;
  GLubyte green = i[1] >> (8 - m_greenBits);// & m_greenMask;
  GLubyte blue = i[2] >> (8 - m_blueBits);// & m_blueMask;

  unsigned int ic = red;
  ic <<= m_redBits;
  ic += green;
  ic <<= m_greenBits;
  ic += blue;
  //selected_id = getSelectedID(ic);
  WorldEntity *selected_entity = getSelectedID(ic);
  if (selected_entity != m_activeEntity) {
    m_activeEntity = selected_entity;
    if (debug && m_activeEntity) Log::writeLog(std::string("ActiveID: ") + m_activeEntity->getId(), Log::LOG_DEFAULT);
  }
}

//TODO should be in general render class
void GL::readConfig(varconf::Config &config) {
  if (debug) std::cout << "GL: readConfig" << std::endl;
  varconf::Variable temp;
  if (debug) Log::writeLog("Loading Renderer Config", Log::LOG_DEFAULT);

  // Setup character light source
  if (config.findItem(RENDER, KEY_character_light_kc)) {
    temp = config.getItem(RENDER, KEY_character_light_kc);
    m_lights[LIGHT_CHARACTER].attenuation_constant = (!temp.is_double()) ? (DEFAULT_character_light_kc) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].attenuation_constant = DEFAULT_character_light_kc;
  }
  if (config.findItem(RENDER, KEY_character_light_kl)) {
    temp = config.getItem(RENDER, KEY_character_light_kl);
    m_lights[LIGHT_CHARACTER].attenuation_linear = (!temp.is_double()) ? (DEFAULT_character_light_kl) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].attenuation_linear = DEFAULT_character_light_kl;
  }
  if (config.findItem(RENDER, KEY_character_light_kq)) {
    temp = config.getItem(RENDER, KEY_character_light_kq);
    m_lights[LIGHT_CHARACTER].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_character_light_kq) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].attenuation_quadratic = DEFAULT_character_light_kq;
  }
  if (config.findItem(RENDER, KEY_character_light_ambient_red)) {
    temp = config.getItem(RENDER, KEY_character_light_ambient_red);
    m_lights[LIGHT_CHARACTER].ambient[0] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_red) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].ambient[0] = DEFAULT_character_light_ambient_red;
  }
  if (config.findItem(RENDER, KEY_character_light_ambient_green)) {
    temp = config.getItem(RENDER, KEY_character_light_ambient_green);
    m_lights[LIGHT_CHARACTER].ambient[1] = (float)((!temp.is_double()) ? (DEFAULT_character_light_ambient_green) : ((double)(temp)));
  } else {
    m_lights[LIGHT_CHARACTER].ambient[1] = DEFAULT_character_light_ambient_green;
  }
  if (config.findItem(RENDER, KEY_character_light_ambient_blue)) {
    temp = config.getItem(RENDER, KEY_character_light_ambient_blue);
    m_lights[LIGHT_CHARACTER].ambient[2] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_blue) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].ambient[2] = DEFAULT_character_light_ambient_blue;
  }
  if (config.findItem(RENDER, KEY_character_light_ambient_alpha)) {
    temp = config.getItem(RENDER, KEY_character_light_ambient_alpha);
    m_lights[LIGHT_CHARACTER].ambient[3] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_alpha) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].ambient[3] = DEFAULT_character_light_ambient_alpha;
  }

  if (config.findItem(RENDER, KEY_character_light_diffuse_red)) {
    temp = config.getItem(RENDER, KEY_character_light_diffuse_red);
    m_lights[LIGHT_CHARACTER].diffuse[0] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_red) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].diffuse[0] = DEFAULT_character_light_diffuse_red;
  }
  if (config.findItem(RENDER, KEY_character_light_diffuse_green)) {
    temp = config.getItem(RENDER, KEY_character_light_diffuse_green);
    m_lights[LIGHT_CHARACTER].diffuse[1] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_green) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].diffuse[1] = DEFAULT_character_light_diffuse_green;
  }
  if (config.findItem(RENDER, KEY_character_light_diffuse_blue)) {
    temp = config.getItem(RENDER, KEY_character_light_diffuse_blue);
    m_lights[LIGHT_CHARACTER].diffuse[2] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_blue) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].diffuse[2] = DEFAULT_character_light_diffuse_blue;
  }
  if (config.findItem(RENDER, KEY_character_light_diffuse_alpha)) {
    temp = config.getItem(RENDER, KEY_character_light_diffuse_alpha);
    m_lights[LIGHT_CHARACTER].diffuse[3] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_alpha) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].diffuse[3] = DEFAULT_character_light_diffuse_alpha;
  }

  if (config.findItem(RENDER, KEY_character_light_specular_red)) {
    temp = config.getItem(RENDER, KEY_character_light_specular_red);
    m_lights[LIGHT_CHARACTER].specular[0] = (!temp.is_double()) ? (DEFAULT_character_light_specular_red) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].specular[0] = DEFAULT_character_light_specular_red;
  }
  if (config.findItem(RENDER, KEY_character_light_specular_green)) {
    temp = config.getItem(RENDER, KEY_character_light_specular_green);
    m_lights[LIGHT_CHARACTER].specular[1] = (!temp.is_double()) ? (DEFAULT_character_light_specular_green) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].specular[1] = DEFAULT_character_light_specular_green;
  }
  if (config.findItem(RENDER, KEY_character_light_specular_blue)) {
    temp = config.getItem(RENDER, KEY_character_light_specular_blue);
    m_lights[LIGHT_CHARACTER].specular[2] = (!temp.is_double()) ? (DEFAULT_character_light_specular_blue) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].specular[2] = DEFAULT_character_light_specular_blue;
  }
  if (config.findItem(RENDER, KEY_character_light_specular_alpha)) {
    temp = config.getItem(RENDER, KEY_character_light_specular_alpha);
    m_lights[LIGHT_CHARACTER].specular[3] = (!temp.is_double()) ? (DEFAULT_character_light_specular_alpha) : ((double)(temp));
  } else {
    m_lights[LIGHT_CHARACTER].specular[3] = DEFAULT_character_light_specular_alpha;
  }
  //Setup Sun light source
  if (config.findItem(RENDER, KEY_sun_light_kc)) {
    temp = config.getItem(RENDER, KEY_sun_light_kc);
    m_lights[LIGHT_SUN].attenuation_constant = (!temp.is_double()) ? (DEFAULT_sun_light_kc) : ((double)(temp));
  } else {
    m_lights[LIGHT_SUN].attenuation_constant = DEFAULT_sun_light_kc;
  }
  if (config.findItem(RENDER, KEY_sun_light_kc)) {
    temp = config.getItem(RENDER, KEY_sun_light_kl);
    m_lights[LIGHT_SUN].attenuation_linear = (!temp.is_double()) ? (DEFAULT_sun_light_kl) : ((double)(temp));
  } else {
    m_lights[LIGHT_SUN].attenuation_linear = DEFAULT_sun_light_kl;
  }
  if (config.findItem(RENDER, KEY_sun_light_kc)) {
    temp = config.getItem(RENDER, KEY_sun_light_kq);
    m_lights[LIGHT_SUN].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_sun_light_kq) : ((double)(temp));
  } else {
    m_lights[LIGHT_SUN].attenuation_quadratic = DEFAULT_sun_light_kq;
  }

  // Setup render states
  if (config.findItem(RENDER, KEY_use_textures)) {
    temp = config.getItem(RENDER, KEY_use_textures);
    RenderSystem::getInstance().setState(RenderSystem::RENDER_TEXTURES, ((!temp.is_bool()) ? (DEFAULT_use_textures) : ((bool)(temp))));
  } else {
    RenderSystem::getInstance().setState(RenderSystem::RENDER_TEXTURES, DEFAULT_use_textures);
  }
  if (config.findItem(RENDER, KEY_use_lighting)) {
    temp = config.getItem(RENDER, KEY_use_lighting);
    RenderSystem::getInstance().setState(RenderSystem::RENDER_LIGHTING, ((!temp.is_bool()) ? (DEFAULT_use_lighting) : ((bool)(temp))));
  } else {
    RenderSystem::getInstance().setState(RenderSystem::RENDER_LIGHTING, DEFAULT_use_lighting);
  }
  if (config.findItem(RENDER, KEY_use_stencil)) {
    temp = config.getItem(RENDER, KEY_use_stencil);
    RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, ((!temp.is_bool()) ? (DEFAULT_use_stencil) : ((bool)(temp))));
  } else {
    RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, DEFAULT_use_stencil);
  }

  // Setup the speech offsets
  if(config.findItem(RENDER, KEY_speech_offset_x)) {
    temp = config.getItem(RENDER, KEY_speech_offset_x);
    m_speech_offset_x = (!temp.is_double()) ? (DEFAULT_speech_offset_x) : ((double)(temp));
  } else {
    m_speech_offset_x = DEFAULT_speech_offset_x;
  }
  if(config.findItem(RENDER, KEY_speech_offset_y)) {
    temp = config.getItem(RENDER, KEY_speech_offset_y);
    m_speech_offset_y = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
  } else {
    m_speech_offset_y = DEFAULT_speech_offset_y;
  }
  if(config.findItem(RENDER, KEY_speech_offset_z)) {
    temp = config.getItem(RENDER, KEY_speech_offset_z);
    m_speech_offset_z = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
  } else {
    m_speech_offset_z = DEFAULT_speech_offset_y;
  }

  if (config.findItem(RENDER, KEY_fog_start)) {
    temp = config.getItem(RENDER, KEY_fog_start);
    m_fog_start = (!temp.is_double()) ? (DEFAULT_fog_start) : ((double)(temp));
  } else {
    m_fog_start = DEFAULT_fog_start;
  }
  if (config.findItem(RENDER, KEY_fog_end)) {
    temp = config.getItem(RENDER, KEY_fog_end);
    m_fog_end = (!temp.is_double()) ? (DEFAULT_fog_end) : ((double)(temp));
  } else {
    m_fog_end = DEFAULT_fog_end;
  }

  if (config.findItem(RENDER, KEY_near_clip)) {
    temp = config.getItem(RENDER, KEY_near_clip);
    m_near_clip = (!temp.is_double()) ? (DEFAULT_near_clip) : ((double)(temp));
  } else {
    m_near_clip = DEFAULT_near_clip;
  }

  if (config.findItem(RENDER, KEY_far_clip_dist)) {
    temp = config.getItem(RENDER, KEY_far_clip_dist);
    m_far_clip_dist = (!temp.is_double()) ? (DEFAULT_far_clip_dist) : ((double)(temp));
  } else {
    m_far_clip_dist = DEFAULT_far_clip_dist;
  }
}

void GL::writeConfig(varconf::Config &config) {
  
  // Save character light source
  config.setItem(RENDER, KEY_character_light_kc, m_lights[LIGHT_CHARACTER].attenuation_constant);
  config.setItem(RENDER, KEY_character_light_kl, m_lights[LIGHT_CHARACTER].attenuation_linear);
  config.setItem(RENDER, KEY_character_light_kq, m_lights[LIGHT_CHARACTER].attenuation_quadratic);

  config.setItem(RENDER, KEY_character_light_ambient_red, m_lights[LIGHT_CHARACTER].ambient[0]);
  config.setItem(RENDER, KEY_character_light_ambient_green, m_lights[LIGHT_CHARACTER].ambient[1]);
  config.setItem(RENDER, KEY_character_light_ambient_blue, m_lights[LIGHT_CHARACTER].ambient[2]);
  config.setItem(RENDER, KEY_character_light_ambient_alpha, m_lights[LIGHT_CHARACTER].ambient[3]);

  config.setItem(RENDER, KEY_character_light_diffuse_red, m_lights[LIGHT_CHARACTER].diffuse[0]);
  config.setItem(RENDER, KEY_character_light_diffuse_green, m_lights[LIGHT_CHARACTER].diffuse[1]);
  config.setItem(RENDER, KEY_character_light_diffuse_blue, m_lights[LIGHT_CHARACTER].diffuse[2]);
  config.setItem(RENDER, KEY_character_light_diffuse_alpha, m_lights[LIGHT_CHARACTER].diffuse[3]);

  config.setItem(RENDER, KEY_character_light_specular_red, m_lights[LIGHT_CHARACTER].specular[0]);
  config.setItem(RENDER, KEY_character_light_specular_green, m_lights[LIGHT_CHARACTER].specular[1]);
  config.setItem(RENDER, KEY_character_light_specular_blue, m_lights[LIGHT_CHARACTER].specular[2]);
  config.setItem(RENDER, KEY_character_light_specular_alpha, m_lights[LIGHT_CHARACTER].specular[3]);
  
  // Save Sun light source
  config.setItem(RENDER, KEY_sun_light_kc, m_lights[LIGHT_SUN].attenuation_constant);
  config.setItem(RENDER, KEY_sun_light_kl, m_lights[LIGHT_SUN].attenuation_linear);
  config.setItem(RENDER, KEY_sun_light_kq, m_lights[LIGHT_SUN].attenuation_quadratic);

  // Save render states
  config.setItem(RENDER, KEY_use_textures, RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES));
  config.setItem(RENDER, KEY_use_lighting, RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING));
  config.setItem(RENDER, KEY_use_stencil, RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL));
  
  // Save the speech offsets
  config.setItem(RENDER, KEY_speech_offset_x, m_speech_offset_x);
  config.setItem(RENDER, KEY_speech_offset_y, m_speech_offset_y);
  config.setItem(RENDER, KEY_speech_offset_z, m_speech_offset_z);

  config.setItem(RENDER, KEY_fog_start, m_fog_start);
  config.setItem(RENDER, KEY_fog_end, m_fog_end);
  config.setItem(RENDER, KEY_near_clip, m_near_clip);
  config.setItem(RENDER, KEY_far_clip_dist, m_far_clip_dist);
}  

void GL::setupStates() {
  // TODO: should this be in the init?
  glDepthFunc(GL_LEQUAL);
  glAlphaFunc(GL_GREATER, 0.1f);
  glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  glFogi(GL_FOG_MODE, GL_LINEAR);
  GLfloat fog_colour[] = {0.50f, 0.50f, 0.50f, 0.50f};
  glFogfv(GL_FOG_COLOR, fog_colour);
  glFogf(GL_FOG_START, m_fog_start);
  glFogf(GL_FOG_END, m_fog_end);
  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glEnableClientState(GL_VERTEX_ARRAY);
}

inline void GL::translateObject(float x, float y, float z) {
  glTranslatef(x, y, z);
}

inline void GL::rotate(float angle, float x, float y, float z) {
  glRotatef(angle, x, y, z);
}

//void GL::rotateObject(WorldEntity *we, int type) {
void GL::rotateObject(ObjectRecord *object_record, ModelRecord *model_record) {
//  if (!we) return; // THROW ERROR;
  switch (model_record->rotation_style) {
    case Graphics::ROS_NONE: return; break;
    case Graphics::ROS_POSITION: {
       WFMath::Point<3> pos = object_record->position;
       glRotatef(pos.x() + pos.y() + pos.z(), 0.0f, 0.0f, 1.0f);
       break;
    }       
    case Graphics::ROS_NORMAL: {
      applyQuaternion(object_record->orient);
      break;
    }
    case Graphics::ROS_BILLBOARD: // Same as HALO, but does not rotate with camera elevation
    case Graphics::ROS_HALO: {
      float rotation_matrix[4][4];
      WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
      orient2 /= m_graphics->getCameraOrientation();
      QuatToMatrix(orient2, rotation_matrix); //Get the rotation matrix for base rotation
      glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
      break;
    }
  }
}

inline void GL::scaleObject(float scale) {
  glScalef(scale, scale, scale);
}

void GL::setViewMode(int type) {
//  Perspective
  glViewport(0, 0, m_width, m_height);
  switch (type) {
    case PERSPECTIVE: {
      if (m_height == 0) m_height = 1;
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity(); // Reset The Projection Matrix
  
      // Calculate The Aspect Ratio Of The Window
      gluPerspective(m_fov,(GLfloat)m_width/(GLfloat)m_height, m_near_clip, m_far_clip_dist);
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      break;
    }
    case ORTHOGRAPHIC: {
      glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
      glLoadIdentity(); // Reset The Projection Matrix
      glOrtho(0, m_width, 0 , m_height, -1, 1); // Set Up An Ortho Screen
      glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
      glLoadIdentity();
      break;
    }			    
    case ISOMETRIC: {
      break;
    }			    
  }	
}
// TODO put into material manager and use display lists to retrieve them
void GL::setMaterial(float *ambient, float *diffuse, float *specular, float shininess, float *emissive) {
  // TODO: set up missing values
  if (ambient)           glMaterialfv (GL_FRONT, GL_AMBIENT,   ambient);
  if (diffuse)           glMaterialfv (GL_FRONT, GL_DIFFUSE,   diffuse);
  if (specular)          glMaterialfv (GL_FRONT, GL_SPECULAR,  specular);
  if (shininess >= 0.0f) glMaterialf  (GL_FRONT, GL_SHININESS, shininess);
  if (emissive)          glMaterialfv (GL_FRONT, GL_EMISSION,  emissive);
  else                   glMaterialfv (GL_FRONT, GL_EMISSION,  black);
}

void GL::renderArrays(unsigned int type, unsigned int offset, unsigned int number_of_points, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) {
  if (!use_multitexturing) multitexture = false;
  // TODO: Reduce ClientState switches
  bool textures = RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES);
  bool lighting = RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING);
 
  if (!vertex_data) {
    Log::writeLog("No Vertex Data", Log::LOG_ERROR);
    return; //throw Exception(""); 
  }
  glVertexPointer(3, GL_FLOAT, 0, (float*)vertex_data);
  if (textures && texture_data) {
    if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
    } else {	    
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, (float*)normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  switch (type) {
    case (Graphics::RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (Graphics::RES_POINT): glDrawArrays(GL_POINT, offset, number_of_points); break;
    case (Graphics::RES_LINES): glDrawArrays(GL_LINES, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLES): glDrawArrays(GL_TRIANGLES, offset, number_of_points); break;
    case (Graphics::RES_QUADS): glDrawArrays(GL_QUADS, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLE_FAN): glDrawArrays(GL_TRIANGLE_FAN, offset, number_of_points); break;
    case (Graphics::RES_TRIANGLE_STRIP): glDrawArrays(GL_TRIANGLE_STRIP, offset, number_of_points); break;
    case (Graphics::RES_QUAD_STRIP): glDrawArrays(GL_QUAD_STRIP, offset, number_of_points); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
 
  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) {
    if (multitexture)  {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
}

void GL::renderElements(unsigned int type, unsigned int number_of_points, int *faces_data, Vertex_3 *vertex_data, Texel *texture_data, Normal *normal_data, bool multitexture) {
  if (!use_multitexturing) multitexture = false;
  // TODO: Reduce ClientState switches
  bool textures = RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES);
  bool lighting = RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING);
 
  if (!vertex_data) return; //throw Exception(""); 
  glVertexPointer(3, GL_FLOAT, 0, (float*)vertex_data);

  if (textures && texture_data) {
     if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
    } else {	    
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)texture_data);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
  if (lighting && normal_data) {
    glNormalPointer(GL_FLOAT, 0, (float*)normal_data);
    glEnableClientState(GL_NORMAL_ARRAY);
  }
  if (use_ext_compiled_vertex_array) glLockArraysEXT(0, number_of_points);
  switch (type) {
    case (Graphics::RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (Graphics::RES_POINT): glDrawElements(GL_POINT, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_LINES): glDrawElements(GL_LINES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLES): glDrawElements(GL_TRIANGLES, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_QUADS): glDrawElements(GL_QUADS, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLE_FAN): glDrawElements(GL_TRIANGLE_FAN, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_TRIANGLE_STRIP): glDrawElements(GL_TRIANGLE_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    case (Graphics::RES_QUAD_STRIP): glDrawElements(GL_QUAD_STRIP, number_of_points, GL_UNSIGNED_INT, faces_data); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
  if (use_ext_compiled_vertex_array) glUnlockArraysEXT();

  if (lighting && normal_data) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && texture_data) {
    if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
}

void GL::drawQueue(QueueMap &queue, bool select_mode, float time_elapsed) {
//  static MoelHandler *model_handler = m_system->getModelHandler();
  for (QueueMap::const_iterator I = queue.begin(); I != queue.end(); I++) {
    // Change state for this queue
    RenderSystem::getInstance().switchState(I->first);
    for (Queue::const_iterator J = I->second.begin(); J != I->second.end(); ++J) {

      ObjectRecord *object_record = J->first;
      ModelRecord *model_record = ModelSystem::getInstance().getModel(this, object_record, J->second, object_record->entity);
      if (!model_record) {
//        std::cerr << "No model record!" << std::endl;	      
        continue;
      }
      Model *model = model_record->model;
      // Get model
      if (!model) {  // ERROR GETTING MODEL
	Log::writeLog("Trying to render NULL model", Log::LOG_ERROR);
        continue;
      }
      glPushMatrix();

      // Translate Model
      WFMath::Point<3> pos = object_record->position;//we->getAbsPos();
      translateObject(pos.x(), pos.y(), pos.z() );
      // Correct model position
      translateObject(model_record->offset_x, model_record->offset_y, model_record->offset_z);

      // Correct model rotation
      glRotatef(model_record->rotate_z, 0.0f, 0.0f, 1.0f);   
      // Rotate Model
      rotateObject(object_record, model_record);

      // Scale Object
      float scale = model_record->scale;
      // Do not perform scaling if it is to zero or has no effect
      if (scale != 0.0f && scale != 1.0f) glScalef(scale, scale, scale);

      if (model_record->scale_bbox && object_record->entity->hasBBox()) {
        WFMath::AxisBox<3> bbox = object_record->entity->getBBox();
        float x_scale = bbox.highCorner().x() - bbox.lowCorner().x();
        float y_scale = bbox.highCorner().y() - bbox.lowCorner().y();
        float z_scale = bbox.highCorner().z() - bbox.lowCorner().z();
        glScalef(x_scale, y_scale, z_scale);
      }

      // Update Model
      if (!select_mode) { // Only needs to be done once a frame
        model->update(time_elapsed);
        model->setLastTime(System::instance()->getTimef());
      }      
      // Draw Model
      if (select_mode) {
        nextColour(object_record->entity);
	model->render(true);
      } else {
        if (object_record->entity == m_activeEntity) {
          m_active_name = object_record->name;
	  drawOutline(model_record);
	}
	else model->render(false);
      }
      glPopMatrix();
    }
  }
}

void GL::drawNameQueue(MessageList &list) {
  glColor4fv(blue);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(FONT));
  for (MessageList::const_iterator I = list.begin(); I != list.end(); ++I) {
    WorldEntity *we = (WorldEntity*)*I;
    glPushMatrix();
    WFMath::Point<3> pos = we->getAbsPos();
    glTranslatef(pos.x(), pos.y(), pos.z());
    WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
    orient2 /= m_graphics->getCameraOrientation(); 
    applyQuaternion(orient2);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    glTranslatef(m_speech_offset_x, m_speech_offset_y, m_speech_offset_z);
    print3D(we->getName().c_str(), 0);
    we->renderMessages();
    glPopMatrix();
  }
}
void GL::drawMessageQueue(MessageList &list) {
  glColor4fv(yellow);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(FONT));
  for (MessageList::const_iterator I = list.begin(); I != list.end(); ++I) {
    WorldEntity *we = (WorldEntity*)*I;
    glPushMatrix();
    WFMath::Point<3> pos = we->getAbsPos();
    glTranslatef(pos.x(), pos.y(), pos.z());
    WFMath::Quaternion  orient2 = WFMath::Quaternion(1.0f, 0.0f, 0.0f, 0.0f); // Initial Camera rotation
    orient2 /= m_graphics->getCameraOrientation(); 
    applyQuaternion(orient2);
    glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
    glScalef(0.025f, 0.025f, 0.025f);
    glTranslatef(m_speech_offset_x, m_speech_offset_y, m_speech_offset_z);
    we->renderMessages();
    glPopMatrix();
  }
}
 
inline float GL::distFromNear(float x, float y, float z) {
  return Frustum::distFromNear(m_frustum, x, y, z);
}
	
inline int GL::patchInFrustum(const WFMath::AxisBox<3> & bbox) {
  return Frustum::patchInFrustum(m_frustum, bbox);
}

void GL::drawOutline(ModelRecord *model_record) {
  StateID cur_state = RenderSystem::getInstance().getCurrentState();
  Model *model = model_record->model;
  bool use_stencil = RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL) && model_record->outline;
  if (use_stencil) { // Using Stencil Buffer
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, -1, 1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glPushMatrix();
    model->render(false);
    glPopMatrix();
    //TODO hard code halo in static const variable
    RenderSystem::getInstance().switchState(model_record->select_state);
    glStencilFunc(GL_NOTEQUAL, -1, 1);
    glColor4fv(halo_colour);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    model->render(true);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_STENCIL_TEST);
    glColor4fv(white);
  } else { // Just use solid colour on object 
    RenderSystem::getInstance().switchState(model_record->select_state);
    glColor4fv(halo_colour);  
    model->render(true);
    glColor4fv(white);
  }
  RenderSystem::getInstance().switchState(cur_state); // Restore state
}


inline void GL::store() { glPushMatrix(); }
inline void GL::restore() { glPopMatrix(); }

inline void GL::beginFrame() {
  // TODO into display list
  m_active_name = "";
  if (RenderSystem::getInstance().getState(RenderSystem::RENDER_STENCIL)) {
    glClearStencil(1);
    glClear(GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear The Screen And The Depth Buffer
  } else {
    glClear(GL_DEPTH_BUFFER_BIT);
  }
// TODO remove
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity(); // Reset The View
  //Rotate Coordinate System so Z points upwards and Y points into the screen. 
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
}

inline void GL::endFrame(bool select_mode) {
//  glFlush();
  if (!select_mode) SDL_GL_SwapBuffers();
  if (debug) checkError();
}
  
void GL::drawSplashScreen() {
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(SPLASH));
  #ifndef _WIN32
    // TODO Need to find a win32 version
    usleep(sleep_time);
  #endif
  setViewMode(ORTHOGRAPHIC);
  
  glColor4fv(white);

  if (m_splash_id == -1) m_splash_id = RenderSystem::getInstance().requestTexture(TEXTURE_splash_texture);
  RenderSystem::getInstance().switchTexture(m_splash_id);

  // TODO into vertex array?
  glBegin(GL_QUADS); 
    glTexCoord2i(0, 0); glVertex2f(0.0f, 0.0f);
    glTexCoord2i(0, 1); glVertex2f(0.0f, m_height);
    glTexCoord2i(1, 1); glVertex2f(m_width, m_height);
    glTexCoord2i(1, 0); glVertex2f(m_width, 0.0f);
  glEnd(); 
  setViewMode(PERSPECTIVE);
}
  
inline void GL::applyQuaternion(const WFMath::Quaternion & quaternion) {
  float rotation_matrix[4][4];
  QuatToMatrix(quaternion, rotation_matrix); //Get the rotation matrix for base rotation
  glMultMatrixf(&rotation_matrix[0][0]); //Apply rotation matrix
}
  
void GL::applyCharacterLighting(float x, float y, float z) {
  float ps[] = {x, y, z, 1.0f};
  glLightfv(GL_LIGHT0,GL_POSITION, ps);
}


void GL::applyLighting() {
  Calendar *calendar = System::instance()->getCalendar();
  float tim = calendar->getTimeInArea();
  float dawn_time = calendar->getDawnStart();
  float day_time = calendar->getDayStart();
  float dusk_time = calendar->getDuskStart();
  float night_time = calendar->getNightStart();
  
  float modifier = calendar->getSecondsPerMinute() * calendar->getMinutesPerHour();
  
  static GLfloat fog_colour[4];// = {0.50f, 0.50f, 0.50f, 0.50f};
  switch (calendar->getTimeArea()) {
    case Calendar::INVALID: break;
    case Calendar::DAWN: {
      m_light_level = tim / ((day_time - dawn_time) * modifier);
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DAY: {
      m_light_level = 1.0f;
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::DUSK: {
      m_light_level = 1.0f - (tim / ((night_time - dusk_time) * modifier));
      float pos_mod = tim / ((night_time - dawn_time) * modifier);
      m_lights[LIGHT_SUN].position[0] = -200.0f * (pos_mod - 0.5f);
      break;
    }
    case Calendar::NIGHT: {
      m_light_level = 0.0f;
      break;
    }
  }
  if (m_light_level < 0.15f) m_light_level = 0.15f; 
  fog_colour[0] = fog_colour[1] = fog_colour[2] = fog_colour[3] = 0.5f * m_light_level;
//  glFogfv(GL_FOG_COLOR, fog_colour);
  float sun_pos[] = {m_lights[LIGHT_SUN].position[0], 0.0f, 100.0f, 1.0f};
  m_lights[LIGHT_SUN].ambient[0] = m_lights[LIGHT_SUN].ambient[1] = m_lights[LIGHT_SUN].ambient[2] = m_light_level * 0.5f;
  m_lights[LIGHT_SUN].diffuse[0] = m_lights[LIGHT_SUN].diffuse[1] = m_lights[LIGHT_SUN].diffuse[2] = m_light_level;
  glLightfv(GL_LIGHT1,GL_POSITION,sun_pos);
  glLightfv(GL_LIGHT1, GL_AMBIENT, m_lights[LIGHT_SUN].ambient);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, m_lights[LIGHT_SUN].diffuse);
}

inline void GL::resetSelection() {
  resetColours();
}

inline void GL::renderActiveName() {
  if (m_active_name.empty()) return;
  glColor4fv(activeNameColour);
  RenderSystem::getInstance().switchState(RenderSystem::getInstance().requestState(FONT));
  print(m_x_pos, m_y_pos, m_active_name.c_str(), 1);
}

inline void GL::getFrustum(float frust[6][4]) {
  float  proj[16];
  float  modl[16];
  /* Get the current PROJECTION matrix from OpenGraphics */
  glGetFloatv(GL_PROJECTION_MATRIX, proj );
  /* Get the current MODELVIEW matrix from OpenGraphics */
  glGetFloatv(GL_MODELVIEW_MATRIX, modl );
  Frustum::getFrustum(frust, proj, modl);
  // Copy m_frustum - local copy plus one from graphics object
  for (int i = 0; i < 6; ++i) {
    for (int j = 0; j < 4; ++j) {
      m_frustum[i][j] = frust[i][j];
    }
  }
}

void GL::varconf_callback(const std::string &section, const std::string &key, varconf::Config &config) {
  varconf::Variable temp;
  if (section == RENDER) {
    if (key == KEY_character_light_kc) {
      temp = config.getItem(RENDER, KEY_character_light_kc);
      m_lights[LIGHT_CHARACTER].attenuation_constant = (!temp.is_double()) ? (DEFAULT_character_light_kc) : ((double)(temp));
    }
    else if (key ==  KEY_character_light_kl) {
      temp = config.getItem(RENDER, KEY_character_light_kl);
      m_lights[LIGHT_CHARACTER].attenuation_linear = (!temp.is_double()) ? (DEFAULT_character_light_kl) : ((double)(temp));
    }
    else if (key == KEY_character_light_kq) {
      temp = config.getItem(RENDER, KEY_character_light_kq);
      m_lights[LIGHT_CHARACTER].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_character_light_kq) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_red) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_red);
      m_lights[LIGHT_CHARACTER].ambient[0] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_green) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_green);
      m_lights[LIGHT_CHARACTER].ambient[1] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_blue) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_blue);
      m_lights[LIGHT_CHARACTER].ambient[2] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_ambient_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_ambient_alpha);
      m_lights[LIGHT_CHARACTER].ambient[3] = (!temp.is_double()) ? (DEFAULT_character_light_ambient_alpha) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_red) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_red);
      m_lights[LIGHT_CHARACTER].diffuse[0] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_green) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_green);
      m_lights[LIGHT_CHARACTER].diffuse[1] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_blue) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_blue);
      m_lights[LIGHT_CHARACTER].diffuse[2] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_diffuse_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_diffuse_alpha);
      m_lights[LIGHT_CHARACTER].diffuse[3] = (!temp.is_double()) ? (DEFAULT_character_light_diffuse_alpha) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_red) {
      temp = config.getItem(RENDER, KEY_character_light_specular_red);
      m_lights[LIGHT_CHARACTER].specular[0] = (!temp.is_double()) ? (DEFAULT_character_light_specular_red) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_green) {
      temp = config.getItem(RENDER, KEY_character_light_specular_green);
      m_lights[LIGHT_CHARACTER].specular[1] = (!temp.is_double()) ? (DEFAULT_character_light_specular_green) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_blue) {
      temp = config.getItem(RENDER, KEY_character_light_specular_blue);
      m_lights[LIGHT_CHARACTER].specular[2] = (!temp.is_double()) ? (DEFAULT_character_light_specular_blue) : ((double)(temp));
    }
    else if (key == KEY_character_light_specular_alpha) {
      temp = config.getItem(RENDER, KEY_character_light_specular_alpha);
      m_lights[LIGHT_CHARACTER].specular[3] = (!temp.is_double()) ? (DEFAULT_character_light_specular_alpha) : ((double)(temp));
    }
    //Setup Sun light source
    else if (key == KEY_sun_light_kc) {
      temp = config.getItem(RENDER, KEY_sun_light_kc);
      m_lights[LIGHT_SUN].attenuation_constant = (!temp.is_double()) ? (DEFAULT_sun_light_kc) : ((double)(temp));
    }
    else if (key == KEY_sun_light_kl) {
      temp = config.getItem(RENDER, KEY_sun_light_kl);
      m_lights[LIGHT_SUN].attenuation_linear = (!temp.is_double()) ? (DEFAULT_sun_light_kl) : ((double)(temp));
    }
    else if (key == KEY_sun_light_kq) {
      temp = config.getItem(RENDER, KEY_sun_light_kq);
      m_lights[LIGHT_SUN].attenuation_quadratic = (!temp.is_double()) ? (DEFAULT_sun_light_kq) : ((double)(temp));
    }
    // Setup render states
    else if (key == KEY_use_textures) {
      temp = config.getItem(RENDER, KEY_use_textures);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_TEXTURES, ((!temp.is_bool()) ? (DEFAULT_use_textures) : ((bool)(temp))));
    }
    else if (key == KEY_use_lighting) {
      temp = config.getItem(RENDER, KEY_use_lighting);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_LIGHTING, ((!temp.is_bool()) ? (DEFAULT_use_lighting) : ((bool)(temp))));
    }
    else if (key == KEY_use_stencil) {
      temp = config.getItem(RENDER, KEY_use_stencil);
      RenderSystem::getInstance().setState(RenderSystem::RENDER_STENCIL, ((!temp.is_bool()) ? (DEFAULT_use_stencil) : ((bool)(temp))));  
    }
    // Setup the speech offsets
    else if (key == KEY_speech_offset_x) {
      temp = config.getItem(RENDER, KEY_speech_offset_x);
      m_speech_offset_x = (!temp.is_double()) ? (DEFAULT_speech_offset_x) : ((double)(temp));
    }
    else if (key == KEY_speech_offset_y) {
      temp = config.getItem(RENDER, KEY_speech_offset_y);
      m_speech_offset_y = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
    }
    else if (key == KEY_speech_offset_z) {
      temp = config.getItem(RENDER, KEY_speech_offset_z);
      m_speech_offset_z = (!temp.is_double()) ? (DEFAULT_speech_offset_y) : ((double)(temp));
    }
    else if (key == KEY_fog_start) {
      temp = config.getItem(RENDER, KEY_fog_start);
      m_fog_start = (!temp.is_double()) ? (DEFAULT_fog_start) : ((double)(temp));
    }
    else if (key == KEY_fog_end) {
      temp = config.getItem(RENDER, KEY_fog_end);
      m_fog_end = (!temp.is_double()) ? (DEFAULT_fog_end) : ((double)(temp));
    }
    else if (key == KEY_near_clip) {
      temp = config.getItem(RENDER, KEY_near_clip);
      m_near_clip = (!temp.is_double()) ? (DEFAULT_near_clip) : ((double)(temp));
    }
    else if (key == KEY_far_clip_dist) {
      temp = config.getItem(RENDER, KEY_far_clip_dist);
      m_far_clip_dist = (!temp.is_double()) ? (DEFAULT_far_clip_dist) : ((double)(temp));
    }
  }
}

std::string GL::getActiveID() {
  return (m_activeEntity) ? (m_activeEntity->getId()) : ("");
} 
void GL::renderMeshArrays(Mesh &mesh, unsigned int offset, bool multitexture) {
  if (!use_multitexturing) multitexture = false;
  // TODO: Reduce ClientState switches
  bool textures = RenderSystem::getInstance().getState(RenderSystem::RENDER_TEXTURES);
  bool lighting = RenderSystem::getInstance().getState(RenderSystem::RENDER_LIGHTING);
 
  if (!mesh.vertex_array) {
    Log::writeLog("No Vertex Data", Log::LOG_ERROR);
    return; //throw Exception(""); 
  }

  if (mesh.vertex_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.vertex_vbo);
  glVertexPointer(3, GL_FLOAT, 0, (float*)mesh.vertex_array);
  if (textures && mesh.tex_coord_array) {
    if (multitexture) {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      if (mesh.tex_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.tex_vbo);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)mesh.tex_coord_array);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
      if (mesh.tex_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.tex_vbo);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)mesh.tex_coord_array);
    } else {	    
      if (mesh.tex_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.tex_vbo);
      glTexCoordPointer(2, GL_FLOAT, 0, (float*)mesh.tex_coord_array);
      glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
  if (lighting && mesh.normal_array) {
    if (mesh.normal_vbo) glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.normal_vbo);
    glNormalPointer(GL_FLOAT, 0, (float*)mesh.normal_array);
    glEnableClientState(GL_NORMAL_ARRAY);
  }

  switch (mesh.data_type) {
    case (Graphics::RES_INVALID): Log::writeLog("Trying to render INVALID type", Log::LOG_ERROR); break;
    case (Graphics::RES_POINT): glDrawArrays(GL_POINT, offset, mesh.number_of_points); break;
    case (Graphics::RES_LINES): glDrawArrays(GL_LINES, offset, mesh.number_of_points); break;
    case (Graphics::RES_TRIANGLES): glDrawArrays(GL_TRIANGLES, offset, mesh.number_of_points); break;
    case (Graphics::RES_QUADS): glDrawArrays(GL_QUADS, offset, mesh.number_of_points); break;
    case (Graphics::RES_TRIANGLE_FAN): glDrawArrays(GL_TRIANGLE_FAN, offset, mesh.number_of_points); break;
    case (Graphics::RES_TRIANGLE_STRIP): glDrawArrays(GL_TRIANGLE_STRIP, offset, mesh.number_of_points); break;
    case (Graphics::RES_QUAD_STRIP): glDrawArrays(GL_QUAD_STRIP, offset, mesh.number_of_points); break;
    default: Log::writeLog("Unknown type", Log::LOG_ERROR); break;
  }
 
  if (lighting && mesh.normal_array) glDisableClientState(GL_NORMAL_ARRAY);
  if (textures && mesh.tex_coord_array) {
    if (multitexture)  {
      glClientActiveTextureARB(GL_TEXTURE1_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
      glClientActiveTextureARB(GL_TEXTURE0_ARB);
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    } else {
      glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }
  }
}

void GL::vboMesh(Mesh &mesh) {
  if (sage_ext[GL_ARB_vertex_buffer_object]) {
    if (mesh.vertex_array) {
      glGenBuffersARB(1, &mesh.vertex_vbo);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.vertex_vbo);
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh.number_of_points * 3 * sizeof(float), mesh.vertex_array, GL_STATIC_DRAW_ARB);
      //delete [] mesh.vertex_array;
      mesh.vertex_array = NULL;
    }
    if (mesh.tex_coord_array) {
      glGenBuffersARB(1, &mesh.tex_vbo);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.tex_vbo);
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh.number_of_points * 2 * sizeof(float), mesh.tex_coord_array, GL_STATIC_DRAW_ARB);
//      delete [] mesh.tex_coord_array;
      mesh.tex_coord_array = NULL;
    }
    if (mesh.normal_array) {
      glGenBuffersARB(1, &mesh.normal_vbo);
      glBindBufferARB(GL_ARRAY_BUFFER_ARB, mesh.normal_vbo);
      glBufferDataARB(GL_ARRAY_BUFFER_ARB, mesh.number_of_points * 3 * sizeof(float), mesh.normal_array, GL_STATIC_DRAW_ARB);
//      delete [] mesh.normal_array;
      mesh.normal_array = NULL;
    }
  } else {
    mesh.vertex_vbo = 0;
    mesh.tex_vbo = 0;
    mesh.normal_vbo = 0;
  }
}

void GL::cleanVBOMesh(Mesh &mesh) {

  if (mesh.vertex_vbo) {
    glDeleteBuffersARB(1, &mesh.vertex_vbo);
    mesh.vertex_vbo = 0;
  }
  if (mesh.tex_vbo) {
    glDeleteBuffersARB(1, &mesh.tex_vbo);
    mesh.tex_vbo = 0;
  }
  if (mesh.normal_vbo) {
    glDeleteBuffersARB(1, &mesh.normal_vbo);
    mesh.normal_vbo = 0;
  }
}

void GL::resize(int width, int height) {
  m_width = width;
  m_height = height;
  int flags = SDL_OPENGL | SDL_RESIZABLE;
  const SDL_VideoInfo *info = SDL_GetVideoInfo();
  if (!info) {
    Log::writeLog("Error quering video", Log::LOG_DEFAULT);
    return;// false;
  }
  int bpp = info->vfmt->BitsPerPixel;
  if (m_fullscreen) flags |= SDL_FULLSCREEN;
  if (debug) std::cout << "Setting video to " << m_width << " x " << m_height << std::endl;

  // This is probly being leaked.
  m_screen = SDL_SetVideoMode(m_width, m_height, bpp, flags);
  if (m_screen == NULL ) {
    std::cerr << "Unable to set " << m_width << " x " << m_height << " video: " << SDL_GetError() << std::endl;
    // Need a better error condition
    return;
  }
  // Have the textures been destroyed?
  if (!glIsTexture(1)) {
    Environment::getInstance().invalidate();
    createWindow(m_width, m_height, m_fullscreen);
    RenderSystem::getInstance().invalidate();
    Environment::getInstance().invalidate(); 
    return;
  }
  // Update view port
  setViewMode(PERSPECTIVE);
}

bool GL::getWorldCoords(int x, int y, float &wx, float &wy, float &wz) {
 // Taken from equator
  y = m_height - y;
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];

  float z = 1.f;
  glReadPixels (x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z);
  if (debug) printf("Screen Depth: %f\n", z);
  if (!(z < 1.f)) {
      return false;
  }

  setViewMode(PERSPECTIVE);
  glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  m_graphics->setCameraTransform();
  if (debug) printf("Screen Coord: %d %d %f\n", x, y, z);
  glGetIntegerv (GL_VIEWPORT, viewport);
  glGetDoublev (GL_MODELVIEW_MATRIX, mvmatrix);
  glGetDoublev (GL_PROJECTION_MATRIX, projmatrix);

  double tx, ty, tz;
  gluUnProject(x, y, z, mvmatrix, projmatrix, viewport, &tx, &ty, &tz);
  wx = tx;
  wy = ty;
  wz = tz;

  if (debug) printf("World Coord: %f %f %f\n", wx, wy, wz);
  return true;
}


} // namespace Sear
