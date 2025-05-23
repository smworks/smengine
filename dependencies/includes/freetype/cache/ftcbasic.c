/***************************************************************************/
/*                                                                         */
/*  ftcbasic.c                                                             */
/*                                                                         */
/*    The FreeType basic cache interface (body).                           */
/*                                                                         */
/*  Copyright 2003-2007, 2009-2011, 2013 by                                */
/*  David Turner, Robert Wilhelm, and Werner Lemberg.                      */
/*                                                                         */
/*  This file is part of the FreeType project, and may only be used,       */
/*  modified, and distributed under the terms of the FreeType project      */
/*  license, LICENSE.TXT.  By continuing to use, modify, or distribute     */
/*  this file you indicate that you have read the license and              */
/*  understand and accept it fully.                                        */
/*                                                                         */
/***************************************************************************/


#include "../ft2build.h"
#include "../ftobjs.h"
#include "../ftdebug.h"
#include FT_CACHE_H
#include "ftcglyph.h"
#include "ftcimage.h"
#include "ftcsbits.h"

#include "ftccback.h"
#include "ftcerror.h"

#define FT_COMPONENT  trace_cache


  /*
   *  Basic Families
   *
   */
  typedef struct  FTC_BasicAttrRec_
  {
    FTC_ScalerRec  scaler;
    FT_UInt        load_flags;

  } FTC_BasicAttrRec, *FTC_BasicAttrs;

#define FTC_BASIC_ATTR_COMPARE( a, b )                                 \
          FT_BOOL( FTC_SCALER_COMPARE( &(a)->scaler, &(b)->scaler ) && \
                   (a)->load_flags == (b)->load_flags               )

#define FTC_BASIC_ATTR_HASH( a )                                   \
          ( FTC_SCALER_HASH( &(a)->scaler ) + 31*(a)->load_flags )


  typedef struct  FTC_BasicQueryRec_
  {
    FTC_GQueryRec     gquery;
    FTC_BasicAttrRec  attrs;

  } FTC_BasicQueryRec, *FTC_BasicQuery;


  typedef struct  FTC_BasicFamilyRec_
  {
    FTC_FamilyRec     family;
    FTC_BasicAttrRec  attrs;

  } FTC_BasicFamilyRec, *FTC_BasicFamily;


  FT_CALLBACK_DEF( FT_Bool )
  ftc_basic_family_compare( FTC_MruNode  ftcfamily,
                            FT_Pointer   ftcquery )
  {
    FTC_BasicFamily  family = (FTC_BasicFamily)ftcfamily;
    FTC_BasicQuery   query  = (FTC_BasicQuery)ftcquery;


    return FTC_BASIC_ATTR_COMPARE( &family->attrs, &query->attrs );
  }


  FT_CALLBACK_DEF( FT_Error )
  ftc_basic_family_init( FTC_MruNode  ftcfamily,
                         FT_Pointer   ftcquery,
                         FT_Pointer   ftccache )
  {
    FTC_BasicFamily  family = (FTC_BasicFamily)ftcfamily;
    FTC_BasicQuery   query  = (FTC_BasicQuery)ftcquery;
    FTC_Cache        cache  = (FTC_Cache)ftccache;


    FTC_Family_Init( FTC_FAMILY( family ), cache );
    family->attrs = query->attrs;
    return 0;
  }


  FT_CALLBACK_DEF( FT_UInt )
  ftc_basic_family_get_count( FTC_Family   ftcfamily,
                              FTC_Manager  manager )
  {
    FTC_BasicFamily  family = (FTC_BasicFamily)ftcfamily;
    FT_Error         error;
    FT_Face          face;
    FT_UInt          result = 0;


    error = FTC_Manager_LookupFace( manager, family->attrs.scaler.face_id,
                                    &face );

    if ( error || !face )
      return result;

    if ( (FT_ULong)face->num_glyphs > FT_UINT_MAX || 0 > face->num_glyphs )
    {
      FT_TRACE1(( "ftc_basic_family_get_count: too large number of glyphs " ));
      FT_TRACE1(( "in this face, truncated\n", face->num_glyphs ));
    }

    if ( !error )
      result = (FT_UInt)face->num_glyphs;

    return result;
  }


  FT_CALLBACK_DEF( FT_Error )
  ftc_basic_family_load_bitmap( FTC_Family   ftcfamily,
                                FT_UInt      gindex,
                                FTC_Manager  manager,
                                FT_Face     *aface )
  {
    FTC_BasicFamily  family = (FTC_BasicFamily)ftcfamily;
    FT_Error         error;
    FT_Size          size;


    error = FTC_Manager_LookupSize( manager, &family->attrs.scaler, &size );
    if ( !error )
    {
      FT_Face  face = size->face;


      error = FT_Load_Glyph( face, gindex,
                             family->attrs.load_flags | FT_LOAD_RENDER );
      if ( !error )
        *aface = face;
    }

    return error;
  }


  FT_CALLBACK_DEF( FT_Error )
  ftc_basic_family_load_glyph( FTC_Family  ftcfamily,
                               FT_UInt     gindex,
                               FTC_Cache   cache,
                               FT_Glyph   *aglyph )
  {
    FTC_BasicFamily  family = (FTC_BasicFamily)ftcfamily;
    FT_Error         error;
    FTC_Scaler       scaler = &family->attrs.scaler;
    FT_Face          face;
    FT_Size          size;


    /* we will now load the glyph image */
    error = FTC_Manager_LookupSize( cache->manager,
                                    scaler,
                                    &size );
    if ( !error )
    {
      face = size->face;

      error = FT_Load_Glyph( face, gindex, family->attrs.load_flags );
      if ( !error )
      {
        if ( face->glyph->format == FT_GLYPH_FORMAT_BITMAP  ||
             face->glyph->format == FT_GLYPH_FORMAT_OUTLINE )
        {
          /* ok, copy it */
          FT_Glyph  glyph;


          error = FT_Get_Glyph( face->glyph, &glyph );
          if ( !error )
          {
            *aglyph = glyph;
            goto Exit;
          }
        }
        else
          error = FT_THROW( Invalid_Argument );
      }
    }

  Exit:
    return error;
  }


  FT_CALLBACK_DEF( FT_Bool )
  ftc_basic_gnode_compare_faceid( FTC_Node    ftcgnode,
                                  FT_Pointer  ftcface_id,
                                  FTC_Cache   cache,
                                  FT_Bool*    list_changed )
  {
    FTC_GNode        gnode   = (FTC_GNode)ftcgnode;
    FTC_FaceID       face_id = (FTC_FaceID)ftcface_id;
    FTC_BasicFamily  family  = (FTC_BasicFamily)gnode->family;
    FT_Bool          result;


    if ( list_changed )
      *list_changed = FALSE;
    result = FT_BOOL( family->attrs.scaler.face_id == face_id );
    if ( result )
    {
      /* we must call this function to avoid this node from appearing
       * in later lookups with the same face_id!
       */
      FTC_GNode_UnselectFamily( gnode, cache );
    }
    return result;
  }


 /*
  *
  * basic image cache
  *
  */

  FT_CALLBACK_TABLE_DEF
  const FTC_IFamilyClassRec  ftc_basic_image_family_class =
  {
    {
      sizeof ( FTC_BasicFamilyRec ),
      ftc_basic_family_compare,
      ftc_basic_family_init,
      0,                        /* FTC_MruNode_ResetFunc */
      0                         /* FTC_MruNode_DoneFunc  */
    },
    ftc_basic_family_load_glyph
  };


  FT_CALLBACK_TABLE_DEF
  const FTC_GCacheClassRec  ftc_basic_image_cache_class =
  {
    {
      ftc_inode_new,
      ftc_inode_weight,
      ftc_gnode_compare,
      ftc_basic_gnode_compare_faceid,
      ftc_inode_free,

      sizeof ( FTC_GCacheRec ),
      ftc_gcache_init,
      ftc_gcache_done
    },
    (FTC_MruListClass)&ftc_basic_image_family_class
  };


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_ImageCache_New( FTC_Manager      manager,
                      FTC_ImageCache  *acache )
  {
    return FTC_GCache_New( manager, &ftc_basic_image_cache_class,
                           (FTC_GCache*)acache );
  }


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_ImageCache_Lookup( FTC_ImageCache  cache,
                         FTC_ImageType   type,
                         FT_UInt         gindex,
                         FT_Glyph       *aglyph,
                         FTC_Node       *anode )
  {
    FTC_BasicQueryRec  query;
    FTC_Node           node = 0; /* make compiler happy */
    FT_Error           error;
    FT_PtrDist         hash;


    /* some argument checks are delayed to FTC_Cache_Lookup */
    if ( !aglyph )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    *aglyph = NULL;
    if ( anode )
      *anode  = NULL;

    {
      if ( (FT_ULong)(type->flags - FT_INT_MIN) > FT_UINT_MAX )
      {
        FT_TRACE1(( "FTC_ImageCache_Lookup: higher bits in load_flags" ));
        FT_TRACE1(( "0x%x are dropped\n", (type->flags & ~((FT_ULong)FT_UINT_MAX)) ));
      }

      query.attrs.scaler.face_id = type->face_id;
      query.attrs.scaler.width   = type->width;
      query.attrs.scaler.height  = type->height;
      query.attrs.load_flags     = (FT_UInt)type->flags;
    }

    query.attrs.scaler.pixel = 1;
    query.attrs.scaler.x_res = 0;  /* make compilers happy */
    query.attrs.scaler.y_res = 0;

    hash = FTC_BASIC_ATTR_HASH( &query.attrs ) + gindex;

#if 1  /* inlining is about 50% faster! */
    FTC_GCACHE_LOOKUP_CMP( cache,
                           ftc_basic_family_compare,
                           FTC_GNode_Compare,
                           hash, gindex,
                           &query,
                           node,
                           error );
#else
    error = FTC_GCache_Lookup( FTC_GCACHE( cache ),
                               hash, gindex,
                               FTC_GQUERY( &query ),
                               &node );
#endif
    if ( !error )
    {
      *aglyph = FTC_INODE( node )->glyph;

      if ( anode )
      {
        *anode = node;
        node->ref_count++;
      }
    }

  Exit:
    return error;
  }


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_ImageCache_LookupScaler( FTC_ImageCache  cache,
                               FTC_Scaler      scaler,
                               FT_ULong        load_flags,
                               FT_UInt         gindex,
                               FT_Glyph       *aglyph,
                               FTC_Node       *anode )
  {
    FTC_BasicQueryRec  query;
    FTC_Node           node = 0; /* make compiler happy */
    FT_Error           error;
    FT_PtrDist         hash;


    /* some argument checks are delayed to FTC_Cache_Lookup */
    if ( !aglyph || !scaler )
    {
      error = FT_THROW( Invalid_Argument );
      goto Exit;
    }

    *aglyph = NULL;
    if ( anode )
      *anode  = NULL;

    /* FT_Load_Glyph(), FT_Load_Char() take FT_UInt flags */
    if ( load_flags > FT_UINT_MAX )
    {
      FT_TRACE1(( "FTC_ImageCache_LookupScaler: higher bits in load_flags" ));
      FT_TRACE1(( "0x%x are dropped\n", (load_flags & ~((FT_ULong)FT_UINT_MAX)) ));
    }

    query.attrs.scaler     = scaler[0];
    query.attrs.load_flags = (FT_UInt)load_flags;

    hash = FTC_BASIC_ATTR_HASH( &query.attrs ) + gindex;

    FTC_GCACHE_LOOKUP_CMP( cache,
                           ftc_basic_family_compare,
                           FTC_GNode_Compare,
                           hash, gindex,
                           &query,
                           node,
                           error );
    if ( !error )
    {
      *aglyph = FTC_INODE( node )->glyph;

      if ( anode )
      {
        *anode = node;
        node->ref_count++;
      }
    }

  Exit:
    return error;
  }


  /*
   *
   * basic small bitmap cache
   *
   */

  FT_CALLBACK_TABLE_DEF
  const FTC_SFamilyClassRec  ftc_basic_sbit_family_class =
  {
    {
      sizeof ( FTC_BasicFamilyRec ),
      ftc_basic_family_compare,
      ftc_basic_family_init,
      0,                            /* FTC_MruNode_ResetFunc */
      0                             /* FTC_MruNode_DoneFunc  */
    },
    ftc_basic_family_get_count,
    ftc_basic_family_load_bitmap
  };


  FT_CALLBACK_TABLE_DEF
  const FTC_GCacheClassRec  ftc_basic_sbit_cache_class =
  {
    {
      ftc_snode_new,
      ftc_snode_weight,
      ftc_snode_compare,
      ftc_basic_gnode_compare_faceid,
      ftc_snode_free,

      sizeof ( FTC_GCacheRec ),
      ftc_gcache_init,
      ftc_gcache_done
    },
    (FTC_MruListClass)&ftc_basic_sbit_family_class
  };


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_SBitCache_New( FTC_Manager     manager,
                     FTC_SBitCache  *acache )
  {
    return FTC_GCache_New( manager, &ftc_basic_sbit_cache_class,
                           (FTC_GCache*)acache );
  }


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_SBitCache_Lookup( FTC_SBitCache  cache,
                        FTC_ImageType  type,
                        FT_UInt        gindex,
                        FTC_SBit      *ansbit,
                        FTC_Node      *anode )
  {
    FT_Error           error;
    FTC_BasicQueryRec  query;
    FTC_Node           node = 0; /* make compiler happy */
    FT_PtrDist         hash;


    if ( anode )
      *anode = NULL;

    /* other argument checks delayed to FTC_Cache_Lookup */
    if ( !ansbit )
      return FT_THROW( Invalid_Argument );

    *ansbit = NULL;

    {
      if ( (FT_ULong)(type->flags - FT_INT_MIN) > FT_UINT_MAX )
      {
        FT_TRACE1(( "FTC_ImageCache_Lookup: higher bits in load_flags" ));
        FT_TRACE1(( "0x%x are dropped\n", (type->flags & ~((FT_ULong)FT_UINT_MAX)) ));
      }

      query.attrs.scaler.face_id = type->face_id;
      query.attrs.scaler.width   = type->width;
      query.attrs.scaler.height  = type->height;
      query.attrs.load_flags     = (FT_UInt)type->flags;
    }

    query.attrs.scaler.pixel = 1;
    query.attrs.scaler.x_res = 0;  /* make compilers happy */
    query.attrs.scaler.y_res = 0;

    /* beware, the hash must be the same for all glyph ranges! */
    hash = FTC_BASIC_ATTR_HASH( &query.attrs ) +
           gindex / FTC_SBIT_ITEMS_PER_NODE;

#if 1  /* inlining is about 50% faster! */
    FTC_GCACHE_LOOKUP_CMP( cache,
                           ftc_basic_family_compare,
                           FTC_SNode_Compare,
                           hash, gindex,
                           &query,
                           node,
                           error );
#else
    error = FTC_GCache_Lookup( FTC_GCACHE( cache ),
                               hash,
                               gindex,
                               FTC_GQUERY( &query ),
                               &node );
#endif
    if ( error )
      goto Exit;

    *ansbit = FTC_SNODE( node )->sbits +
              ( gindex - FTC_GNODE( node )->gindex );

    if ( anode )
    {
      *anode = node;
      node->ref_count++;
    }

  Exit:
    return error;
  }


  /* documentation is in ftcache.h */

  FT_EXPORT_DEF( FT_Error )
  FTC_SBitCache_LookupScaler( FTC_SBitCache  cache,
                              FTC_Scaler     scaler,
                              FT_ULong       load_flags,
                              FT_UInt        gindex,
                              FTC_SBit      *ansbit,
                              FTC_Node      *anode )
  {
    FT_Error           error;
    FTC_BasicQueryRec  query;
    FTC_Node           node = 0; /* make compiler happy */
    FT_PtrDist         hash;


    if ( anode )
        *anode = NULL;

    /* other argument checks delayed to FTC_Cache_Lookup */
    if ( !ansbit || !scaler )
        return FT_THROW( Invalid_Argument );

    *ansbit = NULL;

    /* FT_Load_Glyph(), FT_Load_Char() take FT_UInt flags */
    if ( load_flags > FT_UINT_MAX )
    {
      FT_TRACE1(( "FTC_ImageCache_LookupScaler: higher bits in load_flags" ));
      FT_TRACE1(( "0x%x are dropped\n", (load_flags & ~((FT_ULong)FT_UINT_MAX)) ));
    }

    query.attrs.scaler     = scaler[0];
    query.attrs.load_flags = (FT_UInt)load_flags;

    /* beware, the hash must be the same for all glyph ranges! */
    hash = FTC_BASIC_ATTR_HASH( &query.attrs ) +
             gindex / FTC_SBIT_ITEMS_PER_NODE;

    FTC_GCACHE_LOOKUP_CMP( cache,
                           ftc_basic_family_compare,
                           FTC_SNode_Compare,
                           hash, gindex,
                           &query,
                           node,
                           error );
    if ( error )
      goto Exit;

    *ansbit = FTC_SNODE( node )->sbits +
              ( gindex - FTC_GNODE( node )->gindex );

    if ( anode )
    {
      *anode = node;
      node->ref_count++;
    }

  Exit:
    return error;
  }


/* END */
