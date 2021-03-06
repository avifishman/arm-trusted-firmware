/*
 * Copyright (c) 2016-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <common/bl_common.h>
#include <common/desc_image_load.h>
#include <plat/common/platform.h>

#include <plat_arm.h>

#pragma weak plat_flush_next_bl_params
#pragma weak plat_get_bl_image_load_info
#pragma weak plat_get_next_bl_params

static bl_params_t *next_bl_params_cpy_ptr;

/*******************************************************************************
 * This function flushes the data structures so that they are visible
 * in memory for the next BL image.
 ******************************************************************************/
void plat_flush_next_bl_params(void)
{
	assert(next_bl_params_cpy_ptr != NULL);

	flush_bl_params_desc_args(bl_mem_params_desc_ptr,
		bl_mem_params_desc_num,
		next_bl_params_cpy_ptr);
}

/*******************************************************************************
 * This function returns the list of loadable images.
 ******************************************************************************/
struct bl_load_info *plat_get_bl_image_load_info(void)
{
	return get_bl_load_info_from_mem_params_desc();
}

/*******************************************************************************
 * ARM helper function to return the list of executable images.Since the default
 * descriptors are allocated within BL2 RW memory, this prevents BL31/BL32
 * overlay of BL2 memory. Hence this function also copies the descriptors to a
 * pre-allocated memory indicated by ARM_BL2_MEM_DESC_BASE.
 ******************************************************************************/
struct bl_params *arm_get_next_bl_params(void)
{
	bl_mem_params_node_t *bl2_mem_params_descs_cpy
			= (bl_mem_params_node_t *)ARM_BL2_MEM_DESC_BASE;
	const bl_params_t *next_bl_params;

	next_bl_params_cpy_ptr =
		(bl_params_t *)(ARM_BL2_MEM_DESC_BASE +
		(bl_mem_params_desc_num * sizeof(bl_mem_params_node_t)));

	/*
	 * Copy the memory descriptors to ARM_BL2_MEM_DESC_BASE area.
	 */
	(void) memcpy(bl2_mem_params_descs_cpy, bl_mem_params_desc_ptr,
		(bl_mem_params_desc_num * sizeof(bl_mem_params_node_t)));

	/*
	 * Modify the global 'bl_mem_params_desc_ptr' to point to the
	 * copied location.
	 */
	bl_mem_params_desc_ptr = bl2_mem_params_descs_cpy;

	next_bl_params = get_next_bl_params_from_mem_params_desc();
	assert(next_bl_params != NULL);

	/*
	 * Copy 'next_bl_params' to the reserved location after the copied
	 * memory descriptors.
	 */
	(void) memcpy(next_bl_params_cpy_ptr, next_bl_params,
						(sizeof(bl_params_t)));

	populate_next_bl_params_config(next_bl_params_cpy_ptr);

	return next_bl_params_cpy_ptr;
}

/*******************************************************************************
 * This function returns the list of executable images
 ******************************************************************************/
struct bl_params *plat_get_next_bl_params(void)
{
	return arm_get_next_bl_params();
}

