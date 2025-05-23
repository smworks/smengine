/***************************************************************************/
/*                                                                         */
/*  t42drivr.h                                                             */
/*                                                                         */
/*    High-level Type 42 driver interface (specification).                 */
/*                                                                         */
/*  Copyright 2002 by Roberto Alameda.                                     */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#ifndef __T42DRIVR_H__
#define __T42DRIVR_H__


#include "../ft2build.h"
#include "../ftdriver.h"


FT_BEGIN_HEADER

#ifdef FT_CONFIG_OPTION_PIC
#error "this module does not support PIC yet"
#endif


  FT_EXPORT_VAR( const FT_Driver_ClassRec )  t42_driver_class;


FT_END_HEADER


#endif /* __T42DRIVR_H__ */


/* END */
