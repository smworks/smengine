/***************************************************************************/
/*                                                                         */
/*  ftrfork.h                                                              */
/*                                                                         */
/*    Embedded resource forks accessor (specification).                    */
/*                                                                         */
/*  Copyright 2004, 2006, 2007, 2012 by                                    */
/*  Masatake YAMATO and Redhat K.K.                                        */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/

/***************************************************************************/
/* Development of the code in this file is support of                      */
/* Information-technology Promotion Agency, Japan.                         */
/***************************************************************************/


#ifndef __FTRFORK_H__
#define __FTRFORK_H__


#include "ft2build.h"
#include "ftobjs.h"


FT_BEGIN_HEADER


  /* Number of guessing rules supported in `FT_Raccess_Guess'.            */
  /* Don't forget to increment the number if you add a new guessing rule. */
#define FT_RACCESS_N_RULES  9


  /* A structure to describe a reference in a resource by its resource ID */
  /* and internal offset.  The `POST' resource expects to be concatenated */
  /* by the order of resource IDs instead of its appearance in the file.  */

  typedef struct  FT_RFork_Ref_
  {
    FT_UShort  res_id;
    FT_ULong   offset;

  } FT_RFork_Ref;

#ifdef FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK
  typedef FT_Error
  (*ft_raccess_guess_func)( FT_Library  library,
                            FT_Stream   stream,
                            char       *base_file_name,
                            char      **result_file_name,
                            FT_Long    *result_offset );

  typedef enum  FT_RFork_Rule_ {
    FT_RFork_Rule_invalid = -2,
    FT_RFork_Rule_uknown, /* -1 */
    FT_RFork_Rule_apple_double,
    FT_RFork_Rule_apple_single,
    FT_RFork_Rule_darwin_ufs_export,
    FT_RFork_Rule_darwin_newvfs,
    FT_RFork_Rule_darwin_hfsplus,
    FT_RFork_Rule_vfat,
    FT_RFork_Rule_linux_cap,
    FT_RFork_Rule_linux_double,
    FT_RFork_Rule_linux_netatalk
  } FT_RFork_Rule;

  /* For fast translation between rule index and rule type,
   * the macros FT_RFORK_xxx should be kept consistent with
   * the raccess_guess_funcs table
   */
  typedef struct ft_raccess_guess_rec_ {
    ft_raccess_guess_func  func;
    FT_RFork_Rule          type;
  } ft_raccess_guess_rec;

#ifndef FT_CONFIG_OPTION_PIC

  /* this array is a storage in non-PIC mode, so ; is needed in END */
#define CONST_FT_RFORK_RULE_ARRAY_BEGIN( name, type )  \
          const type name[] = {
#define CONST_FT_RFORK_RULE_ARRAY_ENTRY( func_suffix, type_suffix )  \
          { raccess_guess_ ## func_suffix,                           \
            FT_RFork_Rule_ ## type_suffix },
#define CONST_FT_RFORK_RULE_ARRAY_END  };

#else /* FT_CONFIG_OPTION_PIC */

  /* this array is a function in PIC mode, so no ; is needed in END */
#define CONST_FT_RFORK_RULE_ARRAY_BEGIN( name, type )  \
          void                                         \
          FT_Init_ ## name( type*  storage )           \
          {                                            \
            type*  local = storage;                    \
                                                       \
                                                       \
            int  i = 0;
#define CONST_FT_RFORK_RULE_ARRAY_ENTRY( func_suffix, type_suffix )  \
          local[i].func = raccess_guess_ ## func_suffix;             \
          local[i].type = FT_RFork_Rule_ ## type_suffix;             \
          i++;
#define CONST_FT_RFORK_RULE_ARRAY_END  }

#endif /* FT_CONFIG_OPTION_PIC */

#endif /* FT_CONFIG_OPTION_GUESSING_EMBEDDED_RFORK */


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_Raccess_Guess                                                   */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Guess a file name and offset where the actual resource fork is     */
  /*    stored.  The macro FT_RACCESS_N_RULES holds the number of          */
  /*    guessing rules;  the guessed result for the Nth rule is            */
  /*    represented as a triplet: a new file name (new_names[N]), a file   */
  /*    offset (offsets[N]), and an error code (errors[N]).                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    library ::                                                         */
  /*      A FreeType library instance.                                     */
  /*                                                                       */
  /*    stream ::                                                          */
  /*      A file stream containing the resource fork.                      */
  /*                                                                       */
  /*    base_name ::                                                       */
  /*      The (base) file name of the resource fork used for some          */
  /*      guessing rules.                                                  */
  /*                                                                       */
  /* <Output>                                                              */
  /*    new_names ::                                                       */
  /*      An array of guessed file names in which the resource forks may   */
  /*      exist.  If `new_names[N]' is NULL, the guessed file name is      */
  /*      equal to `base_name'.                                            */
  /*                                                                       */
  /*    offsets ::                                                         */
  /*      An array of guessed file offsets.  `offsets[N]' holds the file   */
  /*      offset of the possible start of the resource fork in file        */
  /*      `new_names[N]'.                                                  */
  /*                                                                       */
  /*    errors ::                                                          */
  /*      An array of FreeType error codes.  `errors[N]' is the error      */
  /*      code of Nth guessing rule function.  If `errors[N]' is not       */
  /*      FT_Err_Ok, `new_names[N]' and `offsets[N]' are meaningless.      */
  /*                                                                       */
  FT_BASE( void )
  FT_Raccess_Guess( FT_Library  library,
                    FT_Stream   stream,
                    char*       base_name,
                    char**      new_names,
                    FT_Long*    offsets,
                    FT_Error*   errors );


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_Raccess_Get_HeaderInfo                                          */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Get the information from the header of resource fork.  The         */
  /*    information includes the file offset where the resource map        */
  /*    starts, and the file offset where the resource data starts.        */
  /*    `FT_Raccess_Get_DataOffsets' requires these two data.              */
  /*                                                                       */
  /* <Input>                                                               */
  /*    library ::                                                         */
  /*      A FreeType library instance.                                     */
  /*                                                                       */
  /*    stream ::                                                          */
  /*      A file stream containing the resource fork.                      */
  /*                                                                       */
  /*    rfork_offset ::                                                    */
  /*      The file offset where the resource fork starts.                  */
  /*                                                                       */
  /* <Output>                                                              */
  /*    map_offset ::                                                      */
  /*      The file offset where the resource map starts.                   */
  /*                                                                       */
  /*    rdata_pos ::                                                       */
  /*      The file offset where the resource data starts.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  FT_Err_Ok means success.                     */
  /*                                                                       */
  FT_BASE( FT_Error )
  FT_Raccess_Get_HeaderInfo( FT_Library  library,
                             FT_Stream   stream,
                             FT_Long     rfork_offset,
                             FT_Long    *map_offset,
                             FT_Long    *rdata_pos );


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    FT_Raccess_Get_DataOffsets                                         */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Get the data offsets for a tag in a resource fork.  Offsets are    */
  /*    stored in an array because, in some cases, resources in a resource */
  /*    fork have the same tag.                                            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    library ::                                                         */
  /*      A FreeType library instance.                                     */
  /*                                                                       */
  /*    stream ::                                                          */
  /*      A file stream containing the resource fork.                      */
  /*                                                                       */
  /*    map_offset ::                                                      */
  /*      The file offset where the resource map starts.                   */
  /*                                                                       */
  /*    rdata_pos ::                                                       */
  /*      The file offset where the resource data starts.                  */
  /*                                                                       */
  /*    tag ::                                                             */
  /*      The resource tag.                                                */
  /*                                                                       */
  /* <Output>                                                              */
  /*    offsets ::                                                         */
  /*      The stream offsets for the resource data specified by `tag'.     */
  /*      This array is allocated by the function, so you have to call     */
  /*      @ft_mem_free after use.                                          */
  /*                                                                       */
  /*    count ::                                                           */
  /*      The length of offsets array.                                     */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  FT_Err_Ok means success.                     */
  /*                                                                       */
  /* <Note>                                                                */
  /*    Normally you should use `FT_Raccess_Get_HeaderInfo' to get the     */
  /*    value for `map_offset' and `rdata_pos'.                            */
  /*                                                                       */
  FT_BASE( FT_Error )
  FT_Raccess_Get_DataOffsets( FT_Library  library,
                              FT_Stream   stream,
                              FT_Long     map_offset,
                              FT_Long     rdata_pos,
                              FT_Long     tag,
                              FT_Long   **offsets,
                              FT_Long    *count );


FT_END_HEADER

#endif /* __FTRFORK_H__ */


/* END */
