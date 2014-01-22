/***************************************************************************/
/*                                                                         */
/*  cff.c                                                                  */
/*                                                                         */
/*    FreeType OpenType driver component (body only).                      */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2013 by                                     */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#define FT_MAKE_OPTION_SINGLE_OBJECT

#include "ft2build.h"

#include "cff/cffpic.c"
#include "cff/cffdrivr.c"
#include "cff/cffparse.c"
#include "cff/cffload.c"
#include "cff/cffobjs.c"
#include "cff/cffgload.c"
#include "cff/cffcmap.c"

#include "cff/cf2arrst.c"
#include "cff/cf2blues.c"
#include "cff/cf2error.c"
#include "cff/cf2font.c"
#include "cff/cf2ft.c"
#include "cff/cf2hints.c"
#include "cff/cf2intrp.c"
#include "cff/cf2read.c"
#include "cff/cf2stack.c"

/* END */
