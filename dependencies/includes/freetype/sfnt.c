/***************************************************************************/
/*                                                                         */
/*  sfnt.c                                                                 */
/*                                                                         */
/*    Single object library component.                                     */
/*                                                                         */
/*  Copyright 1996-2006, 2013 by                                           */
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
#include "sfnt/sfntpic.c"
#include "sfnt/ttload.c"
#include "sfnt/ttmtx.c"
#include "sfnt/ttcmap.c"
#include "sfnt/ttkern.c"
#include "sfnt/sfobjs.c"
#include "sfnt/sfdriver.c"

#ifdef TT_CONFIG_OPTION_EMBEDDED_BITMAPS
#include "sfnt/pngshim.c"
#include "sfnt/ttsbit.c"
#endif

#ifdef TT_CONFIG_OPTION_POSTSCRIPT_NAMES
#include "sfnt/ttpost.c"
#endif

#ifdef TT_CONFIG_OPTION_BDF
#include "sfnt/ttbdf.c"
#endif

/* END */
