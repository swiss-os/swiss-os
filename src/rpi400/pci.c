
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

#include "../../include/pci.h"
#include "../../include/klib.h"
#include "../../include/rtos.h"

#define ARM_PCIE_PCI_ADDR 0xf8000000ULL
#define ARM_PCIE_CPU_ADDR 0x600000000ULL
#define ARM_PCIE_ADDR_SIZE 0x4000000UL
#define BRCM_PCIE_CAP_REGS 0x00ac
#define PCI_EXP_RTCTL 28
#define PCI_EXP_RTCTL_CRSSVE 0x0010
#define PCI_COMMAND 0x04

#define PCIE_GEN 2
#define DATA_ENDIAN 0
#define MMIO_ENDIAN 0
#define BURST_SIZE_128 0
#define PCI_EXP_LNKCAP_SLS 0x0000000f
#define PCI_EXP_LNKSTA_CLS 0x000f
#define PCI_EXP_LNKSTA_NLW 0x03f0
#define PCI_EXP_LNKSTA_NLW_SHIFT 4

#define ARM_PCIE_REG_CFG_INDEX (ARM_PCIE_HOST_BASE + 0x9000)
#define ARM_PCIE_REG_CFG_DATA (ARM_PCIE_HOST_BASE + 0x8000)

#define get_field_pcie_dl_active(val) \
    ((val & 0x20) >> 0x5)

#define get_field_pcie_phylinkup(val) \
    ((val & 0x10) >> 0x4)

#define get_field_pcie_port(val) \
    ((val & 0x80) >> 0x7)

#define get_field_pcie_majmain(val) \
    ((val & 0xffff) >> 0)

#define set_field_pcie_scb_access_en(tmp, val) \
    ((tmp & ~0x1000) | (0x1000 & ((val) << 0xc)))

#define set_field_pcie_cfg_read_ur_mode(tmp, val) \
    ((tmp & ~0x2000) | (0x2000 & ((val) << 0xd)))

#define set_field_pcie_max_burst_size(tmp, val) \
    ((tmp & ~0x300000) | (0x300000 & ((val) << 0x14)))

#define set_field_pcie_config_lo_size(tmp, val) \
    ((tmp & ~0x1f) | (0x1f & ((val) << 0x0)))

#define pcie__read_class_revision(base) \
    in32(base + 0x8)

#define pcie__read_header_type(base) \
    in8(base + 0xe)

#define pcie__read_cap_regs_list_id(base) \
    in8(base + 0xac + 0)

#define pcie__write_primary_bus(base, val) \
    out8(base + 0x18, val)

#define pcie__write_secondary_bus(base, val) \
    out8(base + 0x19, val)

#define pcie__write_subordinate_bus(base, val) \
    out8(base + 0x1a, val)

#define pcie__write_sec_latency_timer(base, val) \
    out8(base + 0x1b, val)

#define pcie__write_cache_line_size(base, val) \
    out8(base + 0xc, val)

#define pcie__write_io_limit(base, val) \
    out8(base + 0x1d, val)

#define pcie__write_io_base(base, val) \
    out8(base + 0x1c, val)

#define pcie__write_io_base_upper16(base, val) \
    out16(base + 0x30, val)

#define pcie__write_io_limit_upper16(base, val) \
    out16(base + 0x32, val)

#define pcie__write_memory_base(base, val) \
    out16(base + 0x20, val)

#define pcie__write_memory_limit(base, val) \
    out16(base + 0x22, val)

#define pcie__write_bridge_control(base, val) \
    out8(base + 0x3e, val)

#define pcie__write_cap_regs_exp_rtctl(base, val) \
    out8(base + 0xac + 28, val)

#define pcie__write_command(base, val) \
    out16(base + 0x4, val)
     
#define pcie__readback_rc_cfg_priv1_id_val3_class_code(base, val) \
    readback_field(base + 0x043c, 0xffffff, 0x0, val)

#define pcie__readback_rc_cfg_vendor_vendor_specific_reg1_endian_mode_bar2(base, val) \
    readback_field(base + 0x0188, 0xc, 0x2, val)

#define pcie__readback_misc_hard_pcie_hard_debug_clkreq_debug_enable(base, val) \
    readback_field(base + 0x4204, 0x2, 0x1, val)

#define pcie__read_misc_pcie_status(base) \
    in32(base + 0x4068)

#define pcie__read_misc_revision(base) \
        in32(base + 0x406c)

#define pcie__read_misc_misc_ctrl(base) \
        in32(base + 0x4008)

#define pcie__write_misc_misc_ctrl(base, val) \
        out32(base + 0x4008, val)

#define pcie__read_bcrm_cap_regs_exp_lnksta(base) \
    in16(base + 0x00ac + 18)

#define pcie__read_bcrm_cap_regs_exp_lnkcap(base) \
    in32(base + 0x00ac + 12)

#define pcie__read_bcrm_cap_regs_exp_lnkctl2(base) \
    in16(base + 0x00ac + 48)

#define pcie__write_bcrm_cap_regs_exp_lnkcap(base, val) \
    out32(base + 0x00ac + 12, val)

#define pcie__write_bcrm_cap_regs_exp_lnkctl2(base, val) \
    out16(base + 0x00ac + 48, val)

#define pcie__write_misc_misc_ctrl_scb0_size(base, val) \
    write_field(base + 0x4008, 0xf8000000, 0x1b, val)

#define pcie__write_misc_rc_bar1_config_lo_size(base, val) \
    write_field(base + 0x402c, 0x1f, 0x0, val)

#define pcie__write_misc_rc_bar3_config_lo_size(base, val) \
    write_field(base + 0x403c, 0x1f, 0x0, val)

#define pcie__write_misc_cpu_2_pcie_mem_win0_base(base, win, cpu_addr_mb) \
    write_field((base) + 0x4070 + (win), 0xfff0, 0x4, cpu_addr_mb)

#define pcie__write_misc_cpu_2_pcie_mem_win0_limit(base, win, limit_addr_mb) \
    write_field((base) + 0x4070 + (win), 0xfff00000, 0x14, limit_addr_mb)

#define pcie__write_misc_cpu_2_pcie_mem_win0_base_hi(base, win, addr) \
    write_field((base) + 0x4080 + (win), 0xff, 0x0, (os_uint32)((addr) >> 0x0c))

#define pcie__write_misc_cpu_2_pcie_mem_win0_limit_hi(base, win, addr) \
    write_field((base) + 0x4084 + (win), 0xff, 0x0, (os_uint32)((addr) >> 0x0c))

#define pcie__write_misc_cpu_2_pcie_mem_win0_lo(base, win, val) \
    out32(base + 0x400c + (win), val)

#define pcie__write_misc_cpu_2_pcie_mem_win0_hi(base, win, val) \
    out32(base + 0x4010 + (win), val)

#define pcie__write_misc_rc_bar2_config_lo(base, val) \
    out32(base + 0x4034, val)

#define pcie__write_misc_rc_bar2_config_hi(base, val) \
    out32(base + 0x4038, val)

#define pcie__readback_misc_hard_pcie_hard_debug_serdes_iddq(base, val) \
    readback_field(base + 0x4204, 0x08000000, 0x1b, val)
#define pcie__readback_rgr1_sw_init_1_init_generic(base, val) \
    readback_field(base + 0x9210, 0x2, 0x1, val)
#define pcie__readback_rgr1_sw_init_1_perst(base, val) \
    readback_field(base + 0x9210, 0x1, 0x0, val)
#define pcie__readback_intr2_cpu_base_clr(base, val) \
    out32(base + 0x4300 + 0x8, val); \
    in32(base + 0x4300 + 0x8)
#define pcie__readback_intr2_cpu_base_mask_set(base, val) \
    out32(base + 0x4300 + 0x10, val); \
    in32(base + 0x4300 + 0x10)


static os_uint8 volatile *volatile cfg_mem = (volatile os_uint8 *)(ARM_PCIE_REG_CFG_DATA);

static os_uint8 in8(os_intn port)
{
    return *(os_uint8 *)port;
}
static os_uint16 in16(os_intn port)
{
    return *(os_uint16 *)port;
}
static os_uint32 in32(os_intn port)
{
    return *(os_uint32 *)port;
}
static void out8(os_intn port, os_intn data)
{
    *(os_uint8 *)port = data & 0xFF;
}
static void out16(os_intn port, os_intn data)
{
    *(os_uint16 *)port = data & 0xFFFF;
}
static void out32(os_intn port, os_intn data)
{
    *(os_uint32 *)port = (os_uint32)data;
}

void pci__set_addr(os_uint32 bus, os_uint32 slot,
                   os_uint32 function, os_uint32 offset)
{
    os_uint32 address;
    address = ((os_uint32)(bus & 0xFF) << 20) | ((os_uint32)(slot & 0x1F) << 15) | ((os_uint32)(function & 0x7) << 12);
    out32(ARM_PCIE_REG_CFG_INDEX, address);
}

os_uint8 pci__cfg_read8(os_uint32 bus, os_uint32 slot,
                        os_uint32 function, os_uint32 offset)
{
    pci__set_addr(bus, slot, function, offset);
    return cfg_mem[offset];
}

os_uint16 pci__cfg_read16(os_uint32 bus, os_uint32 slot,
                          os_uint32 function, os_uint32 offset)
{
    pci__set_addr(bus, slot, function, offset);
    return ((os_uint16 *)(cfg_mem + offset))[0];
}

os_uint32 pci__cfg_read32(os_uint32 bus, os_uint32 slot,
                          os_uint32 function, os_uint32 offset)
{
    pci__set_addr(bus, slot, function, offset);
    return ((os_uint32 *)(cfg_mem + offset))[0];
}

os_uint8 pci__cfg_write8(os_uint32 bus, os_uint32 slot,
                         os_uint32 function, os_uint32 offset,
                         os_uint8 data)
{
    pci__set_addr(bus, slot, function, offset);
    *((os_uint8 *)(cfg_mem + offset)) = data;
    return *((os_uint8 *)(cfg_mem + offset));
}

os_uint16 pci__cfg_write16(os_uint32 bus, os_uint32 slot,
                           os_uint32 function, os_uint32 offset,
                           os_uint32 data)
{
    pci__set_addr(bus, slot, function, offset);
    *((os_uint16 *)(cfg_mem + offset)) = data;
    return *((os_uint16 *)(cfg_mem + offset));
}

os_uint32 pci__cfg_write32(os_uint32 bus, os_uint32 slot,
                           os_uint32 function, os_uint32 offset,
                           os_size data)
{
    pci__set_addr(bus, slot, function, offset);
    *((os_uint32 *)(cfg_mem + offset)) = data;
    return *((os_uint32 *)(cfg_mem + offset));
}

/*https://github.com/fysnet/FYSOS/blob/master/main/usb/utils/include/pci.h
 */
os_uint32 pci__mem_range(os_uint8 bus,
                         os_uint8 dev,
                         os_uint8 func,
                         os_uint8 offset)
{
    os_size org0, org1, cmnd;
    os_size range[2];

    cmnd = pci__cfg_read_command(bus, dev, func);
    pci__cfg_write_command(bus, dev, func, cmnd & ~0x07);

    org0 = pci__cfg_read32(bus, dev, func, offset);
    if ((org0 & 0x07) == PCI_BASE_ADDRESS_MEMORY_64BIT_SPACE)
    {
        org1 = pci__cfg_read32(bus, dev, func, offset + 4);
    }

    pci__cfg_write32(bus, dev, func, offset, 0xFFFFFFFF);
    if ((org0 & 0x07) == PCI_BASE_ADDRESS_MEMORY_64BIT_SPACE)
    {
        pci__cfg_write32(bus, dev, func, offset + 4, 0xFFFFFFFF);
    }

    range[0] = pci__cfg_read32(bus, dev, func, offset);
    range[1] = 0;
    if ((org0 & 0x07) == PCI_BASE_ADDRESS_MEMORY_64BIT_SPACE)
    {
        range[1] = pci__cfg_read32(bus, dev, func, offset + 4);
    }

    pci__cfg_write32(bus, dev, func, offset, org0);
    if ((org0 & 0x07) == PCI_BASE_ADDRESS_MEMORY_64BIT_SPACE)
    {
        pci__cfg_write32(bus, dev, func, offset + 4, org1);
    }

    pci__cfg_write_command(bus, dev, func, cmnd);

    if (org0 & PCI_BASE_ADDRESS_IO_SPACE)
    {
        org0 = range[0];
        if ((org0 & 0xFFFF0000) == 0)
        {
            org0 |= 0xFFFF0000;
        }
        return (os_size)(~(org0 & ~0x1) + 1);
    }
    else
    {
        return (~(range[0] & ~0xF) + 1);
    }
}


static void write_field(os_uint32 p, os_uint32 mask, int shift, os_uint32 val)
{
    os_uint32 reg = in32(p);

    reg = (reg & ~mask) | ((val << shift) & mask);
    out32(p, reg);
}

static void readback_field(os_uint32 p, os_uint32 mask, int shift, os_uint32 val)
{
    write_field(p, mask, shift, val);
    (void)in32(p);
}

static int encode_ibar_size(os_uint64 size)
{
    int log2_in = 0;

    if (size < 1)
    {
        return -1;
    }
    size >>= 1;
    while (size > 0)
    {
        size >>= 1;
        log2_in++;
    }
    if (log2_in >= 12 && log2_in <= 15)
        /* Covers 4KB to 32KB (inclusive) */
        return (log2_in - 12) + 0x1c;
    else if (log2_in >= 16 && log2_in <= 37)
        /* Covers 64KB to 32GB, (inclusive) */
        return log2_in - 15;
    /* Something is awry so disable */
    return 0;
}

os_intn pcie__link_up(os_uint32 base)
{
    os_uint32 val = pcie__read_misc_pcie_status(base);
    os_uint32 dla = get_field_pcie_dl_active(val);
    os_uint32 plu = get_field_pcie_phylinkup(val);

    return (dla && plu) ? 1 : 0;
}

/* The controller is capable of serving in both RC and EP roles */
os_intn pcie__rc_mode(os_uint32 base)
{
    os_uint32 val = pcie__read_misc_pcie_status(base);
    return !!get_field_pcie_port(val);
}

void pcie__set_outbound_win(os_uint32 base, os_uint32 win, os_uint64 cpu_addr,
                           os_uint64 pcie_addr, os_uint64 size)
{
    os_uint64 cpu_addr_mb, limit_addr_mb;

    pcie__write_misc_cpu_2_pcie_mem_win0_lo(base, win * 8, pcie_addr + MMIO_ENDIAN);
    pcie__write_misc_cpu_2_pcie_mem_win0_hi(base, win * 8, (pcie_addr >> 32) & 0xFFFFFFFF);
   
    cpu_addr_mb = cpu_addr >> 20;
    limit_addr_mb = (cpu_addr + size - 1) >> 20;
    pcie__write_misc_cpu_2_pcie_mem_win0_base(base, win*4, cpu_addr_mb);
    pcie__write_misc_cpu_2_pcie_mem_win0_limit(base, win*4, limit_addr_mb);
    pcie__write_misc_cpu_2_pcie_mem_win0_base_hi(base, win * 8, cpu_addr_mb);
    pcie__write_misc_cpu_2_pcie_mem_win0_limit_hi(base, win * 8, limit_addr_mb);
}
/*
https://github.com/torvalds/linux/blob/master/drivers/pci/controller/pcie-brcmstb.c
*/
os_intn pci__init()
{
    os_uint32 base = ARM_PCIE_HOST_BASE;
    os_uint32 rev;
    os_uint8 ht;
    os_uint8 cp;
    os_uint32 tmp;
    os_uint32 lnkcap;
    os_uint16 lnkctl2;
     os_uint16 lnksta;
    os_uint16 cls;
    os_uint16 nlw;
    os_uint32 limit;
    os_uint32 j, i;
    

    /* Reset the bridge */
    pcie__readback_rgr1_sw_init_1_init_generic(base, 1);
    pcie__readback_rgr1_sw_init_1_perst(base, 1);
    k__usleep(200);

    pcie__readback_rgr1_sw_init_1_perst(base, 0);
    pcie__readback_rgr1_sw_init_1_init_generic(base, 0);
    pcie__readback_misc_hard_pcie_hard_debug_serdes_iddq(base, 0); 
    k__usleep(200);

    tmp = pcie__read_misc_revision(base);
    rev = get_field_pcie_majmain(tmp);

    tmp = pcie__read_misc_misc_ctrl(base);
    tmp = set_field_pcie_scb_access_en(tmp, 1);
    tmp = set_field_pcie_cfg_read_ur_mode(tmp, 1);
    tmp = set_field_pcie_max_burst_size(tmp, BURST_SIZE_128);
    pcie__write_misc_misc_ctrl(base, tmp);
   
    tmp = MEM_PCIE_DMA_RANGE_PCIE_START;
    tmp = set_field_pcie_config_lo_size(tmp, encode_ibar_size(MEM_PCIE_DMA_RANGE_SIZE));
 
    pcie__write_misc_rc_bar2_config_lo(base, tmp);
    pcie__write_misc_rc_bar2_config_hi(base, 0);

    pcie__write_misc_misc_ctrl_scb0_size(base, encode_ibar_size(MEM_PCIE_DMA_RANGE_SIZE));

    pcie__write_misc_rc_bar1_config_lo_size(base, 0);
    pcie__write_misc_rc_bar3_config_lo_size(base, 0);

    pcie__readback_intr2_cpu_base_clr(base, 0xffffffff);
    pcie__readback_intr2_cpu_base_mask_set(base, 0xffffffff);

    lnkcap = pcie__read_bcrm_cap_regs_exp_lnkcap(base);
    lnkctl2 = pcie__read_bcrm_cap_regs_exp_lnkctl2(base);

    lnkcap = (lnkcap & ~PCI_EXP_LNKCAP_SLS) | PCIE_GEN;
    pcie__write_bcrm_cap_regs_exp_lnkcap(base, lnkcap);
    
    lnkctl2 = (lnkctl2 & ~0xf) | PCIE_GEN;
    pcie__write_bcrm_cap_regs_exp_lnkctl2(base, lnkctl2);
    
    pcie__readback_rgr1_sw_init_1_perst(base, 0);
    k__usleep(100000);

    limit = 100;
    for (i = 1, j = 0; j < limit && !pcie__link_up(base);
         j += i, i = i * 2)
    {
        k__usleep((i + j > limit ? limit - j : i) * 1000);
    }
    if (!pcie__link_up(base))
    {
        k__printf("\nLink down!!!\n");
        return -1;
    }

    if (!pcie__rc_mode(base))
    {
        k__printf("PCIe misconfigured; is in EP mode");
        return -1;
    }

    pcie__set_outbound_win(base, 0, ARM_PCIE_CPU_ADDR, MEM_PCIE_RANGE_PCIE_START, MEM_PCIE_RANGE_SIZE);

    pcie__readback_rc_cfg_priv1_id_val3_class_code(base, 0x060400);

    lnksta = pcie__read_bcrm_cap_regs_exp_lnksta(base);
    cls = lnksta & PCI_EXP_LNKSTA_CLS;
    nlw = (lnksta & PCI_EXP_LNKSTA_NLW) >> PCI_EXP_LNKSTA_NLW_SHIFT;

    k__printf("Link up, %x Gbps x%x\n", cls, nlw);

    pcie__readback_rc_cfg_vendor_vendor_specific_reg1_endian_mode_bar2(base, DATA_ENDIAN);

    pcie__readback_misc_hard_pcie_hard_debug_clkreq_debug_enable(base, 1);

    k__printf("PCIE_MISC_REVISION  %x %x %x\n", rev, tmp, encode_ibar_size(0x100000000ULL));

    /* https://github.com/rsta2/circle/blob/1884ccf7f630debded2f225f6f931668a6e3264a/lib/bcmpciehostbridge.cpp#L691
     */
 
    rev = pcie__read_class_revision(base) >> 8; /* revision */
    ht = pcie__read_header_type(base);        /* header type */
    if (rev != 0x060400 || ht != PCI_HEADER_TYPE_PCI_TO_PCI_BRIDGE)
    {
        k__printf("PCI_HEADER_TYPE_PCI_TO_PCI_BRIDGE failure %x %x\n", rev, ht);
        while (1)
        {
            ;
        }
        return -1;
    }

    pcie__write_primary_bus(base, 0);
    pcie__write_secondary_bus(base, 1);
    pcie__write_subordinate_bus(base, 1);
    pcie__write_sec_latency_timer(base, 0x40);
    pcie__write_cache_line_size(base, 8);
    
    pcie__write_io_limit(base, 0);
    pcie__write_io_base(base, 0xf0);
    pcie__write_io_base_upper16(base, 0);
    pcie__write_io_limit_upper16(base, 0);
    
    pcie__write_memory_base(base, (MEM_PCIE_RANGE_PCIE_START) >> 16);
    pcie__write_memory_limit(base, (MEM_PCIE_RANGE_PCIE_START) >> 16);
    pcie__write_bridge_control(base, PCI_BRIDGE_CTL_PARITY);
 
    cp = pcie__read_cap_regs_list_id(base);
    if (cp != PCI_CAPABILITY_PCI_EXPRESS)
    {
        k__printf("PCI_CAPABILITY_PCI_EXPRESS failure %x\n", cp);
        return -1;
    }
 
    pcie__write_cap_regs_exp_rtctl(base, PCI_EXP_RTCTL_CRSSVE);
    pcie__write_command(base, PCI_COMMAND_MEMORY_SPACE | PCI_COMMAND_BUS_MASTER | 
        PCI_COMMAND_PARITY_ERROR_RESPONSE | PCI_COMMAND_SERR_ENABLE);
   
    k__printf("\n BRIDGE  status %x \n",
              pci__cfg_read_status(0, 0, 0));

    if (pci__enable_device(XHCI_PCI_CLASS_CODE, XHCI_PCIE_BUS, XHCI_PCIE_SLOT, XHCI_PCIE_FUNC))
    {
        k__printf("ERROR cannot enable PCI XHCI controller.\n");
        return -1;
    }

    return 0;
}

/* https://github.com/rsta2/circle/blob/master/lib/bcmpciehostbridge.cpp#L722 */
os_intn pci__enable_device(os_uint32 class_code, os_uint32 bus, os_uint32 slot, os_uint32 function)
{
    os_uint8 int_pin;
    os_uint32 rev;
    os_uint8 ht;
    os_intn ret;
    int i;

    property_tag_simple notify_reset;
    pci__cfg_write_command(bus, slot, function, 0);
    notify_reset.value = bus << 20 | slot << 15 | function << 12;
    notify_reset.tag.tag_id = PROPTAG_NOTIFY_XHCI_RESET;
    notify_reset.tag.value_buf_size = sizeof(notify_reset) - sizeof(property_tag);
    notify_reset.tag.value_length = 4 & ~VALUE_LENGTH_RESPONSE;
    ret = tags__get_tags(&notify_reset, sizeof(notify_reset));
    notify_reset.tag.value_length &= ~VALUE_LENGTH_RESPONSE;
    if (ret || notify_reset.tag.value_length == 0)
    {
        k__printf("tag failed\n");
    }
    
    for (i = 1; i < 0x20000; i++) {
            ret = ret * i;
    }
    rev = pci__cfg_read_revision(bus, slot, function) >> 8;
    ht = pci__cfg_read_header_type(bus, slot, function);
    if (rev != class_code || ht != PCI_HEADER_TYPE_SINGLE_FUNCTION)
    {
        k__printf("Error pci enable %x %x\n", rev, ht);
        return -1;
    }

    pci__cfg_write_cache_line_size(bus, slot, function, 64 / 4);
    pci__cfg_write_base_addr(bus, slot, function, 0,
                             MEM_PCIE_RANGE_PCIE_START | PCI_BASE_ADDRESS_MEMORY_64BIT_SPACE);
    pci__cfg_write_base_addr(bus, slot, function, 1, 0);

    int_pin = pci__cfg_read_interrupt_pin(bus, slot, function);
    if (int_pin != 1)
    {
        pci__cfg_write_interrupt_pin(bus, slot, function, 1);
        int_pin = pci__cfg_read_interrupt_pin(bus, slot, function);
    }
    pci__cfg_write_command(bus, slot, function, PCI_COMMAND_MEMORY_SPACE | PCI_COMMAND_BUS_MASTER | PCI_COMMAND_PARITY_ERROR_RESPONSE | PCI_COMMAND_SERR_ENABLE /*| PCI_COMMAND_INTERRUPT_DISABLE*/);
    k__printf("\nPci %x enabled %d %d %d int pin %d\n",
              pci__cfg_read_status(bus, slot, function),
              bus, slot, function, int_pin);

    return 0;
}
