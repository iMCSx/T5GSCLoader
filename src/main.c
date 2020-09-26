#include <sys/prx.h>
#include <stdio.h>

#include "t5.h"
#include "utils.h"
#include <sys/timer.h>
#include <sys/ppu_thread.h>

SYS_MODULE_INFO(T5GSCLoader, 0, 1, 1);
SYS_MODULE_START(start);

void launcher()
{
    sys_prx_get_module_list_t pInfo;
    pInfo.max = 25;
    sys_prx_id_t ids[pInfo.max];
    pInfo.idlist = ids;
    pInfo.size = sizeof(pInfo);

    while (pInfo.count < 18)
    {
        sys_prx_get_module_list(0, &pInfo);
        sys_timer_sleep(1);
    }

    if (init_game() == 0)
    {
        printf(T5INFO "GSC Loader ready.");
    }

    sys_ppu_thread_exit(0);
}

int start(void)
{
    printf("\n********************************************");
    printf("           T5 GSC Loader by iMCSx           ");
    printf("********************************************\n");
    printf(T5INFO "Waiting modules...");

    // Create a thread that wait eboot's modules are getting loaded to be sure that imports opd are resolved.
    sys_ppu_thread_t id;
    sys_ppu_thread_create(&id, launcher, 0, 1000, 0x8000, 0, "[GSC Loader] launcher");
    return SYS_PRX_RESIDENT;
}
