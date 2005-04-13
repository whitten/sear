dnl Initialise autoconf

AC_INIT([sear], [0.5.0])
AC_CONFIG_SRCDIR([src/main.cpp])
AC_CONFIG_HEADER([config.h])

###################################
# TODO remove unrequired checks when eris is released as it pulls in most libs
# We should check for eris and varconf within the same check
##################################

dnl Detect the canonical host and target build environment
AC_CANONICAL_HOST
AC_CANONICAL_TARGET

AM_INIT_AUTOMAKE([check-news])

dnl Need libtool
AM_BINRELOC
AM_PROG_LIBTOOL
dnl AC_PROG_LIBTOOL

AC_CHECK_TOOL(WINDRES, windres)
AM_CONDITIONAL(BUILD_RC_O, test x$ac_cv_prog_WINDRES != x)

SEAR_LIBS=

if test $cross_compiling == no; then
  CPPFLAGS="$CPPFLAGS -I/usr/X11R6/include"
  CXXFLAGS="$CXXFLAGS -Wall"
  SEAR_LIBS="$SEAR_LIBS -L/usr/lib -L/usr/X11R6/lib -L/usr/local/lib"
fi

#CXXFLAGS="$CXXFLAGS -pedantic -ansi"

dnl Test for C++ compiler

AC_PROG_CXX

AC_LANG_CPLUSPLUS
AC_REQUIRE_CPP
AC_PROG_INSTALL
AC_HEADER_STDC

AC_ARG_ENABLE(debug,
  [  --enable-debug	enable debug information [default=no]],
  [
    if test "$enableval" = "yes"; then
      CXXFLAGS="$CXXFLAGS -ggdb -O0 -DDEBUG"
    else
      CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
    fi
  ],[
    CXXFLAGS="$CXXFLAGS -O3 -DNDEBUG"
  ]
)

CXXFLAGS="$CXXFLAGS -DATLAS_ELEMENT_TYPEDEF_PRIVATE=1"

AC_ARG_ENABLE(mmgr,
  [  --enable-mmgr	enable memory checker [default=no]],
  [
    if test "$enableval" = "yes"; then
      use_mmgr="true"
      CXXFLAGS="$CXXFLAGS -DUSE_MMGR=1"
    else
      use_mmgr="false"
    fi
  ],[
    use_mmgr="false"
  ]
)
AM_CONDITIONAL(MMGR, test x"$use_mmgr" = xtrue)


STATIC_LIBSTDCPP=
STATIC_LIBGCC=

AC_ARG_ENABLE(static_build,
  [  --enable-static-build	enable static link of some libs [default=no]],
  [
    if test "$enableval" = "yes"; then
      link_static="true"
      STATIC_LIBSTDCPP=`$CXX -print-file-name=libstdc++.a`
      STATIC_LIBGCC=`$CXX -print-file-name=libgcc.a`
    else
      link_static="false"
    fi
  ],[
    link_static="false"
  ]
)
AM_CONDITIONAL(BUILD_STATIC, test x"$link_static" = xtrue)

AC_LANG_SAVE
AC_LANG_C

dnl Check SDL
SDL_VERSION=1.2.2
AM_PATH_SDL($SDL_VERSION, [
  CPPFLAGS="$CPPFLAGS $SDL_CFLAGS"
  SEAR_LIBS="$SEAR_LIBS $SDL_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find SDL or insufficent version found. Please goto http://www.libsdl.org/ and get at least version $SDL_VERSION)
])
AC_SUBST(SDL_VERSION)

if test $cross_compiling == no; then
  AC_CHECK_LIB(SDL_image,IMG_ReadXPMFromArray, [
    SEAR_LIBS="$SEAR_LIBS -lSDL_image"
  ], [
    AC_MSG_ERROR(Couldn't find SDL_image or wrong version found. Please goto http://www.libsdl.org/projects/SDL_image and get at least version 1.2.1)
])
  AC_CHECK_LIB(SDL_mixer,Mix_OpenAudio, [
    SEAR_LIBS="$SEAR_LIBS -lSDL_mixer"
  ], [
    AC_MSG_ERROR(Couldn't find SDL_sound. Please goto http://www.libsdl.org/projects/SDL_mixer/)
])
else
   AC_CHECK_HEADER(SDL/SDL_image.h,
       [found_sdl_image=yes break],
       [found_sdl_image=no])

   if test $found_sdl_image == yes; then
       SEAR_LIBS="$SEAR_LIBS -lSDL_image"
   else
       AC_MSG_ERROR(Error could not find SDL_image)
   fi

   AC_CHECK_HEADER(SDL/SDL_mixer.h,
       [found_sdl_mixer=yes break],
       [found_sdl_mixer=no])

   if test $found_sdl_mixer == yes; then
       SEAR_LIBS="$SEAR_LIBS -lSDL_mixer"
   else
       AC_MSG_ERROR(Error could not find SDL_mixer)
   fi
fi

AC_LANG_RESTORE

AC_CHECK_HEADER(sstream, , AC_MSG_ERROR(Couldn't find sstream header))

AC_TRY_RUN([
#include <string>
#include <sstream>

int main(int argc, char**argv) {
  std::stringstream ss;
  std::string test_string = "1.05";
  float f = 0.0f;
  ss << test_string;
  ss >> f;
  if (f != 1.05f) return 0;
  return 1;
} 
], [
  AC_DEFINE(NEEDS_SSTREAM_WORKAROUND, 1, [We need the stringstream workaround])
  AC_MSG_WARN(Using sstream workaround.)
],[
# action if false
UNSUSED=0
],[
# action if cross-compile
UNSUSED=0
])

MERCATOR_VERSION=0.2.2
VARCONF_VERSION=0.6.1
ERIS_VERSION=1.3.4
SAGE_VERSION=0.1.1
LIBMODELFILE=0.1.91
PKG_CHECK_MODULES(PKGCHECKS, mercator-0.2 >= $MERCATOR_VERSION varconf-1.0 >= $VARCONF_VERSION eris-1.3 >= $ERIS_VERSION sage >= $SAGE_VERSION libmd3-0.2 >= $LIBMODELFILE,
	[
		CPPFLAGS="$CPPFLAGS $PKGCHECKS_CFLAGS"
		SEAR_LIBS="$SEAR_LIBS $PKGCHECKS_LIBS"
	],
	AC_MSG_ERROR(Couldn't find some WorldForge libs. Please goto ftp://ftp.worldforge.org/pub/worldforge/libs/)
)

AC_SUBST(MERCATOR_VERSION)
AC_SUBST(VARCONF_VERSION)
AC_SUBST(ERIS_VERSION)
AC_SUBST(SAGE_VERSION)

dnl Check Lib 3ds

LIB3DS_VERSION=1.2.0
AC_SUBST(LIB3DS_VERSION)
AM_PATH_LIB3DS($LIB3DS_VERSION, [
  CPPFLAGS="$CPPFLAGS $LIB3DS_CFLAGS"
  SEAR_LIBS="$SEAR_LIBS $LIB3DS_LIBS"
],
[
  AC_MSG_ERROR(Couldn't find lib3ds or insufficient version found. Please goto http://lib3ds.sourceforge.net and get at least version $LIB3DS_VERSION)
])


dnl Check other libs

if test $cross_compiling == no; then
  AC_CHECK_LIB(GL,glViewport, [
    SEAR_LIBS="$SEAR_LIBS -lGL"
  ],[
    AC_MSG_ERROR(Couldn't find OpenGL)
])

  AC_CHECK_LIB(GLU,gluPerspective, [
    SEAR_LIBS="$SEAR_LIBS -lGLU"
  ],[
    AC_MSG_ERROR(Couldn't find GLU)
])

  AC_CHECK_LIB(cal3d,main, [
    SEAR_LIBS="$SEAR_LIBS -lcal3d"
  ], [
    AC_MSG_ERROR(Couldn't find Cal3D. Please goto http://sourceforge.net/projects/cal3d/)
])

 else
   AC_CHECK_HEADER(cal3d/cal3d.h,
       [found_cal3d=yes break],
       [found_cal3d=no])

   if test $found_cal3d == yes; then
       SEAR_LIBS="$SEAR_LIBS -lcal3d"
   else
       AC_MSG_ERROR(Error could not find Cal3D)
   fi
   AC_CHECK_HEADER(GL/gl.h,
       [found_gl=yes break],
       [found_gl=no])

 if test $found_gl == yes; then
       SEAR_LIBS="$SEAR_LIBS -lopengl32"
   else
       AC_MSG_ERROR(Error could not find OpenGL)
   fi

   AC_CHECK_HEADER(GL/glu.h,
       [found_glu=yes break],
       [found_glu=no])

   if test $found_glu == yes; then
       SEAR_LIBS="$SEAR_LIBS -lglu32"
   else
       AC_MSG_ERROR(Error could not find GLU)
   fi
fi

if test "x$prefix" != xNONE; then
    PREFIX=$prefix
  else
    PREFIX=$ac_default_prefix
fi


AC_DEFINE_UNQUOTED(INSTALLDIR,"$PREFIX",[Install prefix])

AC_CHECK_HEADERS(sstream strstream, [ found_string_stream=yes break ],
                                    [ found_string_stream=no ])
if test $found_string_stream == no ; then 
    AC_MSG_ERROR(Couldn't find C++ string <-> stream conversion method.)
fi

dnl SDL adds itself to LIBS
LIBS=

AC_SUBST(SEAR_LIBS)
AC_SUBST(STATIC_LIBSTDCPP)
AC_SUBST(STATIC_LIBGCC)

dnl Generate files
AC_OUTPUT([
	Makefile
	interfaces/Makefile
	common/Makefile
	src/Makefile
	loaders/Makefile
	loaders/cal3d/Makefile
	renderers/Makefile
	environment/Makefile
	data/Makefile
	scripts/Makefile
	gui/Makefile
	sear.spec
	sear-media.spec
	sear-updater.spec
	Sear.iss
	src/sear
],[chmod +x src/sear])