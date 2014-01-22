/***************************************************************************/
/*                                                                         */
/*  ftbase.c                                                               */
/*                                                                         */
/*    Single object library component (body only).                         */
/*                                                                         */
/*  Copyright 1996-2001, 2002, 2003, 2004, 2006, 2007, 2008, 2009 by       */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "ft2build.h"

#define  FT_MAKE_OPTION_SINGLE_OBJECT

#include "base/ftpic.c"
#include "base/basepic.c"
#include "base/ftadvanc.c"
#include "base/ftcalc.c"
#include "base/ftdbgmem.c"
#include "base/ftgloadr.c"
#include "base/ftobjs.c"
#include "base/ftoutln.c"
#include "base/ftrfork.c"
#include "base/ftsnames.c"
#include "base/ftstream.c"
#include "base/fttrigon.c"
#include "base/ftutil.c"

#ifdef FT_MACINTOSH
#include "base/ftmac.c"
#endif

/* END */
