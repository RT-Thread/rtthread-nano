/* FinSH config file */

#ifndef __MSH_CFG_H__
#define __MSH_CFG_H__

// <<< Use Configuration Wizard in Context Menu >>>
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_USING_MSH_ONLY
// <h>FinSH Configuration
// <o>the priority of finsh thread <1-30>
//  <i>the priority of finsh thread
//  <i>Default: 21
#define FINSH_THREAD_PRIORITY       21
// <o>the stack of finsh thread <1-4096>
//  <i>the stack of finsh thread
//  <i>Default: 4096  (4096Byte)
#define FINSH_THREAD_STACK_SIZE     512
// <c>using history
//  <i>the stack of finsh thread
//#define FINSH_USING_HISTORY
// </c>
#define FINSH_HISTORY_LINES         1

#define FINSH_USING_SYMTAB
// </h>

// <<< end of configuration section >>>
#endif

