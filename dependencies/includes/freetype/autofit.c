/***************************************************************************/
/*                                                                         */
/*  autofit.c                                                              */
/*                                                                         */
/*    Auto-fitter module (body).                                           */
/*                                                                         */
/*  Copyright 2003-2007, 2011 by                                           */
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
#include "autofit/afpic.c"
#include "autofit/afangles.c"
#include "autofit/afglobal.c"
#include "autofit/afhints.c"

#include "autofit/afdummy.c"
#include "autofit/aflatin.c"
#ifdef FT_OPTION_AUTOFIT2
#include "autofit/aflatin2.c"
#endif
#include "autofit/afcjk.c"
#include "autofit/afindic.c"

#include "autofit/afloader.c"
#include "autofit/afmodule.c"

#ifdef AF_CONFIG_OPTION_USE_WARPER
#include "autofit/afwarp.c"
#endif

/* END */
