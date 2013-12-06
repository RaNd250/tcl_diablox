

#include "sec_boot_lib.h"

#define MOD                         "ASF"

extern uchar                        sha1sum[];


void * mcpy(void *dest, const void *src, int  cnt)
{
    char *tmp = dest;
    const char *s = src;

    while (cnt--)
        *tmp++ = *s++;
    return dest;
}

int mcmp (const void *cs, const void *ct, int cnt)
{
    const uchar *su1, *su2;
    int res = 0;

    for (su1 = cs, su2 = ct; 0 < cnt; ++su1, ++su2, cnt--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

void dump_buf(uchar* buf, uint32 len)
{
    uint32 i = 1;

    for (i =1; i <len+1; i++)
    {   
        if(0 != buf[i-1])
        {
            SMSG(true,"0x%x,",buf[i-1]);
            
            if(0 == i%8)        
            {
                SMSG(true,"\n");
            }
        }
    }
    
    SMSG(true,"\n");    
}

void img_hash_dump (uchar *buf, uint32 size)
{
    uint32 i = 0;

    for (i = 0 ; i < size ; i++)
    {
        if(i % 4 ==0)
        {   
            SMSG(true,"\n");
        }

        if(buf[i] < 0x10)
        {
            SMSG(true,"0x0%x, ",buf[i]);
        }
        else
        {   
            SMSG(true,"0x%x, ",buf[i]);
        }
    }
}

void img_hash_compute (uchar *buf, uint32 size)
{
    SEC_ASSERT(0);
}

uint32 img_hash_update (char* part_name)
{
    uint32 ret = SEC_OK;
    
    SEC_ASSERT(0);

    return ret;
}


uint32 img_hash_check (char* part_name)
{
    uint32 ret = SEC_OK;
    
    SEC_ASSERT(0);
    
    return ret;
}


char* asf_get_build_info(void)
{
    return BUILD_TIME""BUILD_BRANCH;
}

