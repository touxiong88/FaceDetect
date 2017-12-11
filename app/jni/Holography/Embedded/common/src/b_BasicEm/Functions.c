/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 * 
 * MediaTek Inc. (C) 2010. All rights reserved.
 * 
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* ---- includes ----------------------------------------------------------- */

#include "b_BasicEm/Functions.h"
#include "b_BasicEm/Context.h"

/* ---- related objects  --------------------------------------------------- */

/* ---- typedefs ----------------------------------------------------------- */

/* ---- constants ---------------------------------------------------------- */

/* ---- globals   ---------------------------------------------------------- */

/* ------------------------------------------------------------------------- */

/* ========================================================================= */
/*                                                                           */
/* ---- \ghd{ external functions } ----------------------------------------- */
/*                                                                           */
/* ========================================================================= */

/* ------------------------------------------------------------------------- */

uint16 bbs_swapBytes( uint16 valA )
{
	return ( ( valA >> 8 ) & 0x00FF ) | ( ( valA << 8 ) & 0xFF00 );	
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memWrite32( const void* ptrA, 
					   uint16* memPtrA )
{
	uint32 valL = *( uint32* )ptrA;
	
	#ifdef HW_BIG_ENDIAN
		*memPtrA++ = bbs_swapBytes( ( uint16 )( ( valL >> 0  ) & 0xFFFF ) );
		*memPtrA++ = bbs_swapBytes( ( uint16 )( ( valL >> 16 ) & 0xFFFF ) );
	#else
		*memPtrA++ = ( valL >> 0  ) & 0xFFFF;
		*memPtrA++ = ( valL >> 16 ) & 0xFFFF;
	#endif
	

	return bbs_SIZEOF16( uint32 );
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memRead32( void* ptrA, 
					  const uint16* memPtrA )
{
	uint32 valL = 0;
	
	#ifdef HW_BIG_ENDIAN
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 0  );
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 16 );
	#else
		valL |= ( ( uint32 )*memPtrA++ << 0  );
		valL |= ( ( uint32 )*memPtrA++ << 16 );
	#endif

	*( uint32* )ptrA = valL;

	return bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memPeek32( const uint16* memPtrA )
{
	uint32 valL = 0;

	#ifdef HW_BIG_ENDIAN
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 0  );
		valL |= ( ( uint32 )bbs_swapBytes( *memPtrA++ ) << 16 );
	#else
		valL |= ( ( uint32 )*memPtrA++ << 0  );
		valL |= ( ( uint32 )*memPtrA++ << 16 );
	#endif

	return valL;
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memWrite16( const void* ptrA, 
					   uint16* memPtrA )
{
	#ifdef HW_BIG_ENDIAN
		*memPtrA++ = bbs_swapBytes( *( uint16* )ptrA );
	#else
		*memPtrA++ = *( uint16* )ptrA;
	#endif
	return bbs_SIZEOF16( uint16 );
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memRead16( void* ptrA, 
					  const uint16* memPtrA )
{
	#ifdef HW_BIG_ENDIAN
		*( uint16* )ptrA = bbs_swapBytes( *memPtrA++ );
	#else
		*( uint16* )ptrA = *memPtrA++;
	#endif

	return bbs_SIZEOF16( uint16 );
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memWrite32Arr( struct bbs_Context* cpA,
						  const void* ptrA, 
						  uint32 sizeA, uint16* memPtrA )
{
	uint32 iL;
	const uint32* srcL = ( uint32* )ptrA;

	if( bbs_Context_error( cpA ) ) return 0;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memWrite32( srcL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint32 ); 
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memRead32Arr( struct bbs_Context* cpA,
						 void* ptrA, 
						 uint32 sizeA, 
						 const uint16* memPtrA )
{
	uint32 iL;
	uint32* dstL = ( uint32* )ptrA;

	if( bbs_Context_error( cpA ) ) return 0;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memRead32( dstL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint32 ); 
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memWrite16Arr( struct bbs_Context* cpA,
						  const void* ptrA, 
						  uint32 sizeA, 
						  uint16* memPtrA )
{
	uint32 iL;
	const uint16* srcL = ( uint16* )ptrA;

	if( bbs_Context_error( cpA ) ) return 0;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memWrite16( srcL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint16 ); 
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memRead16Arr( struct bbs_Context* cpA,
						 void* ptrA, 
						 uint32 sizeA, 
						 const uint16* memPtrA )
{
	uint32 iL;
	uint16* dstL = ( uint16* )ptrA;

	if( bbs_Context_error( cpA ) ) return 0;

	for( iL = 0; iL < sizeA; iL++ )
	{
		memPtrA += bbs_memRead16( dstL++, memPtrA );
	}

	return sizeA * bbs_SIZEOF16( uint16 ); 
}

/* ------------------------------------------------------------------------- */

uint32 bbs_memWriteUInt32( uint32 valA, 
						   uint16* memPtrA )
{
	#ifdef HW_BIG_ENDIAN
		*memPtrA++ = bbs_swapBytes( ( uint16 )( ( valA >> 0  ) & 0xFFFF ) );
		*memPtrA++ = bbs_swapBytes( ( uint16 )( ( valA >> 16 ) & 0xFFFF ) );
	#else
		*memPtrA++ = ( valA >> 0  ) & 0xFFFF;
		*memPtrA++ = ( valA >> 16 ) & 0xFFFF;
	#endif

	return bbs_SIZEOF16( valA );
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memWriteUInt16( uint16 valA, 
						   uint16* memPtrA )
{
	#ifdef HW_BIG_ENDIAN
		*memPtrA++ = bbs_swapBytes( valA );
	#else
		*memPtrA++ = valA;
	#endif

	return bbs_SIZEOF16( valA );
}
   
/* ------------------------------------------------------------------------- */

uint32 bbs_memReadVersion32( struct bbs_Context* cpA,
							 uint32* versionPtrA, 
							 uint32 refVersionA, 
							 const uint16* memPtrA )
{
	if( bbs_Context_error( cpA ) ) return 0;

	bbs_memRead32( versionPtrA, memPtrA );
	if( *versionPtrA > refVersionA )
	{
		bbs_ERR0( bbs_ERR_WRONG_VERSION, "uint32 bbs_memReadVersion32( .... ):\n"
			       "Data format is newer than software or corrupt\n" );
	}
	return bbs_SIZEOF16( uint32 );
}

/* ------------------------------------------------------------------------- */

uint16 bbs_memCheckSum16( const uint16* memPtrA, uint32 sizeA )
{
	uint32 iL;
	uint16 sumL = 0;
	for( iL = 0; iL < sizeA; iL++ )
	{
		#ifdef HW_BIG_ENDIAN
			sumL += bbs_swapBytes( memPtrA[ iL ] );
		#else
			sumL += memPtrA[ iL ];
		#endif
	}

	return sumL;
}

/* ------------------------------------------------------------------------- */

