/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2007 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

const char *yaffs_nand_c_version =
    "$Id: yaffs_nand.c,v 1.9 2008-05-05 07:58:58 charles Exp $";

#include "yaffs_nand.h"
#include "yaffs_tagscompat.h"
#include "yaffs_tagsvalidity.h"

#include "yaffs_getblockinfo.h"

#define RTK_DEBUG 0
#if RTK_DEBUG
      #define debug_nand(fmt, arg...)  printk(fmt, ##arg);
#else
      #define debug_nand(fmt, arg...)
#endif


int yaffs_ReadChunkWithTagsFromNAND(yaffs_Device * dev, int chunkInNAND,
					   __u8 * buffer, yaffs_ExtendedTags * tags)
{
debug_nand("---------[%s]----------\n", __FUNCTION__);
	int result;
	yaffs_ExtendedTags localTags;

	int realignedChunkInNAND = chunkInNAND - dev->chunkOffset;
	//printk("realignedChunkInNAND=%d, dev->chunkOffset=%d\n", 
			//realignedChunkInNAND, dev->chunkOffset);
debug_nand("[%s] buffer=%p\n", __FUNCTION__, buffer);
	/* If there are no tags provided, use local tags to get prioritised gc working */
	if(!tags)
		tags = &localTags;

	if (dev->readChunkWithTagsFromNAND)
		result = dev->readChunkWithTagsFromNAND(dev, realignedChunkInNAND, buffer,
						      tags);
	else
		result = yaffs_TagsCompatabilityReadChunkWithTagsFromNAND(dev,
									realignedChunkInNAND, buffer, tags);
									
	if(tags && tags->eccResult > YAFFS_ECC_RESULT_NO_ERROR){
		//yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, chunkInNAND/dev->nChunksPerBlock);	//Ken, org
		yaffs_BlockInfo *bi = yaffs_GetBlockInfo(dev, chunkInNAND >> dev->nChunksPerBlock_Shift);
		yaffs_HandleChunkError(dev,bi);
	}

	return result;
}


int yaffs_WriteChunkWithTagsToNAND(yaffs_Device * dev,
						   int chunkInNAND,
						   const __u8 * buffer,
						   yaffs_ExtendedTags * tags)
{
debug_nand("---------[%s]----------\n", __FUNCTION__);
	chunkInNAND -= dev->chunkOffset;

	if (tags) {
		tags->sequenceNumber = dev->sequenceNumber;
		tags->chunkUsed = 1;
		if (!yaffs_ValidateTags(tags)) {
			T(YAFFS_TRACE_ERROR,
			  (TSTR("Writing uninitialised tags" TENDSTR)));
			YBUG();
			printk("[%s]Writing uninitialised tags\n", __FUNCTION__);
		}
		T(YAFFS_TRACE_WRITE,
		  (TSTR("Writing chunk %d tags %d %d" TENDSTR), chunkInNAND,
		   tags->objectId, tags->chunkId));
	} else {
		T(YAFFS_TRACE_ERROR, (TSTR("Writing with no tags" TENDSTR)));
		YBUG();
	}

	if (dev->writeChunkWithTagsToNAND)
		return dev->writeChunkWithTagsToNAND(dev, chunkInNAND, buffer,
						     tags);
	else
		return yaffs_TagsCompatabilityWriteChunkWithTagsToNAND(dev,
								       chunkInNAND,
								       buffer,
								       tags);
}

int yaffs_MarkBlockBad(yaffs_Device * dev, int blockNo)
{
debug_nand("---------[%s]----------\n", __FUNCTION__);
	blockNo -= dev->blockOffset;

	if (dev->markNANDBlockBad)
		return dev->markNANDBlockBad(dev, blockNo);
	else
		return yaffs_TagsCompatabilityMarkNANDBlockBad(dev, blockNo);
}

int yaffs_QueryInitialBlockState(yaffs_Device * dev,
						 int blockNo,
						 yaffs_BlockState * state,
						 __u32 *sequenceNumber)
{
debug_nand("---------[%s]----------\n", __FUNCTION__);
	blockNo -= dev->blockOffset;

	if (dev->queryNANDBlock)
		return dev->queryNANDBlock(dev, blockNo, state, sequenceNumber);
	else
		return yaffs_TagsCompatabilityQueryNANDBlock(dev, blockNo,
							     state,
							     sequenceNumber);
}


int yaffs_EraseBlockInNAND(struct yaffs_DeviceStruct *dev,
				  int blockInNAND)
{
debug_nand("---------[%s]----------\n", __FUNCTION__);
	int result;

	blockInNAND -= dev->blockOffset;

	dev->nBlockErasures++;
	result = dev->eraseBlockInNAND(dev, blockInNAND);

	return result;
}

int yaffs_InitialiseNAND(struct yaffs_DeviceStruct *dev)
{
	debug_nand("---------[%s]----------\n", __FUNCTION__);
	return dev->initialiseNAND(dev);
}



