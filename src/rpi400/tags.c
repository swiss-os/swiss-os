/*
            MMXXIV October 10 PUBLIC DOMAIN by JML

     The authors and contributors disclaim copyright, patents 
           and all related rights to this software.

 Anyone is free to copy, modify, publish, use, compile, sell, or
 distribute this software, either in source code form or as a
 compiled binary, for any purpose, commercial or non-commercial,
 and by any means.

 The authors waive all rights to patents, both currently owned 
 by the authors or acquired in the future, that are necessarily 
 infringed by this software, relating to make, have made, repair,
 use, sell, import, transfer, distribute or configure hardware 
 or software in finished or intermediate form, whether by run, 
 manufacture, assembly, testing, compiling, processing, loading 
 or applying this software or otherwise.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT OF ANY PATENT, COPYRIGHT, TRADE SECRET OR OTHER
 PROPRIETARY RIGHT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR 
 ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "../../include/klib.h"

static os_uint32 in32(os_intn port)
{
    return *(os_uint32 *)port;
}

static void out32(os_intn port, os_intn data)
{
    *(os_uint32 *)port = (os_uint32)data;
}

os_intn tags__get_tags(void *tag, os_uint32 size)
{
    os_uint32 *end;
    os_uint32 addr;
    os_uint32 data;
    os_uint32 channel = BCM_MAILBOX_PROP_OUT;
    property_buffer *buf = (void *)MEM_COHERENT_REGION;

    buf->buf_size = sizeof(property_buffer) + size + sizeof(os_uint32);
    buf->code = CODE_REQUEST;
    k__memcpy(buf->tags, tag, size);
    end = (os_uint32 *)(buf->tags + size);
    end[0] = PROPTAG_END;

    os__data_sync_barrier();

    addr = BUS_ADDRESS(buf);

    data = addr;

    /*flush*/
    while (!(in32(MAILBOX0_STATUS) & MAILBOX_STATUS_EMPTY))
    {
        in32(MAILBOX0_READ);

        k__usleep(20000);
    }

    /*write*/
    while (in32(MAILBOX1_STATUS) & MAILBOX_STATUS_FULL)
    {
        ;
    }
    out32(MAILBOX1_WRITE, channel | data);

    /*read*/
    do
    {
        while (in32(MAILBOX0_STATUS) & MAILBOX_STATUS_EMPTY)
        {
            ;
        }

        data = in32(MAILBOX0_READ);
    } while ((data & 0xF) != channel);
    data &= ~0xF;

    if (data != addr)
    {
        k__printf("Mailbox failed\n");
        return -1;
    }

    os__data_mem_barrier();

    if (buf->code != CODE_RESPONSE_SUCCESS)
    {
        k__printf("CODE_RESPONSE_SUCCESS failed\n");
        return -1;
    }

    k__memcpy(tag, buf->tags, size);
  
    return 0;
}
