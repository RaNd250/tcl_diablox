

#include "sec_boot_lib.h"

#define MOD                                "ASF.DEV"

extern SECURE_INFO                         sec_info;

char* part2pl (char* part_name)
{

    if(TRUE == sec_usif_enabled())
    {
        return usif2pl(part_name);
    }
    else
    {
        return mtd2pl(part_name);
    }
}

char* pl2part (char* part_name)
{

    if(TRUE == sec_usif_enabled())
    {
        return pl2usif(part_name);
    }
    else
    {
        return pl2mtd(part_name);
    }
}

char* get_android_name(void)
{
    return pl2part(mtd2pl(MTD_ANDSYSIMG));
}

char* get_secro_name(void)
{
    return pl2part(mtd2pl(MTD_SECRO));
}

