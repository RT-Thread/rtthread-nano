/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2017-08-08     Yang        the first version
 * 2019-07-19     Magicoe      The first version for LPC55S6x
 */

#include <rtthread.h>
 

#ifdef RT_USING_DFS_ROMFS
#include <dfs_romfs.h>
#endif

#ifdef RT_USING_DFS_ROMFS
#define SD_ROOT     "/sdcard"
#else
#define SD_ROOT     "/"
#endif

int mnt_init(void)
{
#ifdef RT_USING_DFS_ROMFS
    /* initialize the device filesystem */
    dfs_init();

    dfs_romfs_init();

    /* mount rom file system */
	if (dfs_mount(RT_NULL, "/", "rom", 0, &(romfs_root)) == 0)
    {
        rt_kprintf("ROM file system initializated!\n");
    }
#endif

#ifdef RT_DFS_ELM_REENTRANT

    rt_device_t device = rt_device_find("sdcard0");
    
    if(device != NULL)
    {
        /* mount sd card fat partition 1 as root directory */
        if (dfs_mount("sdcard0", "/", "elm", 0, 0) == 0)
            rt_kprintf("File System initialized!\n");
        else
            rt_kprintf("File System init failed!\n");
    }
    else
    {
        rt_kprintf("SD card not exist!\n");
    }
#endif
    
    return 0;
}
INIT_ENV_EXPORT(mnt_init);
