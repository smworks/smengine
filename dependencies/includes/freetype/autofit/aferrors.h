/***************************************************************************/
/*                                                                         */
/*  aferrors.h                                                             */
/*                                                                         */
/*    Autofitter error codes (specification only).                         */
/*                                                                         */
/*  Copyright 2005, 2012 by                                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


  /*************************************************************************/
  /*                                                                       */
  /* This file is used to define the Autofitter error enumeration          */
  /* constants.                                                            */
  /*                                                                       */
  /*************************************************************************/

#ifndef __AFERRORS_H__
#define __AFERRORS_H__

#include "../ftmoderr.h"

#undef __FTERRORS_H__

#undef  FT_ERR_PREFIX
#define FT_ERR_PREFIX  AF_Err_
#define FT_ERR_BASE    FT_Mod_Err_Autofit

#include "../fterrors.h"

#endif /* __AFERRORS_H__ */

/* END */
