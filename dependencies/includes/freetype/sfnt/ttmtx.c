/***************************************************************************/
/*                                                                         */
/*  ttmtx.c                                                                */
/*                                                                         */
/*    Load the metrics tables common to TTF and OTF fonts (body).          */
/*                                                                         */
/*  Copyright 2006-2009, 2011-2013 by                                      */
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
#include "../ftdebug.h"
#include "../ftstream.h"
#include FT_TRUETYPE_TAGS_H
#include "ttmtx.h"

#include "sferrors.h"


  /*************************************************************************/
  /*                                                                       */
  /* The macro FT_COMPONENT is used in trace mode.  It is an implicit      */
  /* parameter of the FT_TRACE() and FT_ERROR() macros, used to print/log  */
  /* messages during execution.                                            */
  /*                                                                       */
#undef  FT_COMPONENT
#define FT_COMPONENT  trace_ttmtx


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_hmtx                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the `hmtx' or `vmtx' table into a face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face     :: A handle to the target face object.                    */
  /*                                                                       */
  /*    stream   :: The input stream.                                      */
  /*                                                                       */
  /*    vertical :: A boolean flag.  If set, load `vmtx'.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_hmtx( TT_Face    face,
                     FT_Stream  stream,
                     FT_Bool    vertical )
  {
    FT_Error   error;
    FT_ULong   tag, table_size;
    FT_ULong*  ptable_offset;
    FT_ULong*  ptable_size;


    if ( vertical )
    {
      tag           = TTAG_vmtx;
      ptable_offset = &face->vert_metrics_offset;
      ptable_size   = &face->vert_metrics_size;
    }
    else
    {
      tag           = TTAG_hmtx;
      ptable_offset = &face->horz_metrics_offset;
      ptable_size   = &face->horz_metrics_size;
    }

    error = face->goto_table( face, tag, stream, &table_size );
    if ( error )
      goto Fail;

    *ptable_size   = table_size;
    *ptable_offset = FT_STREAM_POS();

  Fail:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_load_hhea                                                  */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Load the `hhea' or 'vhea' table into a face object.                */
  /*                                                                       */
  /* <Input>                                                               */
  /*    face     :: A handle to the target face object.                    */
  /*                                                                       */
  /*    stream   :: The input stream.                                      */
  /*                                                                       */
  /*    vertical :: A boolean flag.  If set, load `vhea'.                  */
  /*                                                                       */
  /* <Return>                                                              */
  /*    FreeType error code.  0 means success.                             */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_load_hhea( TT_Face    face,
                     FT_Stream  stream,
                     FT_Bool    vertical )
  {
    FT_Error        error;
    TT_HoriHeader*  header;

    static const FT_Frame_Field  metrics_header_fields[] =
    {
#undef  FT_STRUCTURE
#define FT_STRUCTURE  TT_HoriHeader

      FT_FRAME_START( 36 ),
        FT_FRAME_ULONG ( Version ),
        FT_FRAME_SHORT ( Ascender ),
        FT_FRAME_SHORT ( Descender ),
        FT_FRAME_SHORT ( Line_Gap ),
        FT_FRAME_USHORT( advance_Width_Max ),
        FT_FRAME_SHORT ( min_Left_Side_Bearing ),
        FT_FRAME_SHORT ( min_Right_Side_Bearing ),
        FT_FRAME_SHORT ( xMax_Extent ),
        FT_FRAME_SHORT ( caret_Slope_Rise ),
        FT_FRAME_SHORT ( caret_Slope_Run ),
        FT_FRAME_SHORT ( caret_Offset ),
        FT_FRAME_SHORT ( Reserved[0] ),
        FT_FRAME_SHORT ( Reserved[1] ),
        FT_FRAME_SHORT ( Reserved[2] ),
        FT_FRAME_SHORT ( Reserved[3] ),
        FT_FRAME_SHORT ( metric_Data_Format ),
        FT_FRAME_USHORT( number_Of_HMetrics ),
      FT_FRAME_END
    };


    if ( vertical )
    {
      void  *v = &face->vertical;


      error = face->goto_table( face, TTAG_vhea, stream, 0 );
      if ( error )
        goto Fail;

      header = (TT_HoriHeader*)v;
    }
    else
    {
      error = face->goto_table( face, TTAG_hhea, stream, 0 );
      if ( error )
        goto Fail;

      header = &face->horizontal;
    }

    if ( FT_STREAM_READ_FIELDS( metrics_header_fields, header ) )
      goto Fail;

    FT_TRACE3(( "Ascender:          %5d\n", header->Ascender ));
    FT_TRACE3(( "Descender:         %5d\n", header->Descender ));
    FT_TRACE3(( "number_Of_Metrics: %5u\n", header->number_Of_HMetrics ));

    header->long_metrics  = NULL;
    header->short_metrics = NULL;

  Fail:
    return error;
  }


  /*************************************************************************/
  /*                                                                       */
  /* <Function>                                                            */
  /*    tt_face_get_metrics                                                */
  /*                                                                       */
  /* <Description>                                                         */
  /*    Returns the horizontal or vertical metrics in font units for a     */
  /*    given glyph.  The metrics are the left side bearing (resp. top     */
  /*    side bearing) and advance width (resp. advance height).            */
  /*                                                                       */
  /* <Input>                                                               */
  /*    header  :: A pointer to either the horizontal or vertical metrics  */
  /*               structure.                                              */
  /*                                                                       */
  /*    idx     :: The glyph index.                                        */
  /*                                                                       */
  /* <Output>                                                              */
  /*    bearing :: The bearing, either left side or top side.              */
  /*                                                                       */
  /*    advance :: The advance width resp. advance height.                 */
  /*                                                                       */
  FT_LOCAL_DEF( FT_Error )
  tt_face_get_metrics( TT_Face     face,
                       FT_Bool     vertical,
                       FT_UInt     gindex,
                       FT_Short   *abearing,
                       FT_UShort  *aadvance )
  {
    FT_Error        error;
    FT_Stream       stream = face->root.stream;
    TT_HoriHeader*  header;
    FT_ULong        table_pos, table_size, table_end;
    FT_UShort       k;


    if ( vertical )
    {
      void*  v = &face->vertical;


      header     = (TT_HoriHeader*)v;
      table_pos  = face->vert_metrics_offset;
      table_size = face->vert_metrics_size;
    }
    else
    {
      header     = &face->horizontal;
      table_pos  = face->horz_metrics_offset;
      table_size = face->horz_metrics_size;
    }

    table_end = table_pos + table_size;

    k = header->number_Of_HMetrics;

    if ( k > 0 )
    {
      if ( gindex < (FT_UInt)k )
      {
        table_pos += 4 * gindex;
        if ( table_pos + 4 > table_end )
          goto NoData;

        if ( FT_STREAM_SEEK( table_pos ) ||
             FT_READ_USHORT( *aadvance ) ||
             FT_READ_SHORT( *abearing )  )
          goto NoData;
      }
      else
      {
        table_pos += 4 * ( k - 1 );
        if ( table_pos + 4 > table_end )
          goto NoData;

        if ( FT_STREAM_SEEK( table_pos ) ||
             FT_READ_USHORT( *aadvance ) )
          goto NoData;

        table_pos += 4 + 2 * ( gindex - k );
        if ( table_pos + 2 > table_end )
          *abearing = 0;
        else
        {
          if ( !FT_STREAM_SEEK( table_pos ) )
            (void)FT_READ_SHORT( *abearing );
        }
      }
    }
    else
    {
    NoData:
      *abearing = 0;
      *aadvance = 0;
    }

    return FT_Err_Ok;
  }


/* END */
