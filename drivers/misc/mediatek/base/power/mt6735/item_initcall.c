#include <linux/pm.h>
#include <linux/bug.h>
#include <linux/memblock.h>

#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
//#include <mach/mt_typedefs.h>
#include <linux/timer.h>
//#include <mach/mt_pmic_wrap.h>
#include <linux/syscore_ops.h>
#include <asm/io.h>
#include <mach/items.h>
#include <asm/setup.h>
#include <linux/of_fdt.h>
//#include <mach/mtk_memcfg.h>

/*
extern u64 pmem_start;  // pmem_start is inited in mt_fixup
extern u64 kernel_mem_sz;       // kernel_mem_sz is inited in mt_fixup
extern u64 RESERVED_MEM_SIZE_FOR_TEST_3D ;
extern u64 FB_SIZE_EXTERN ;
extern u64 RESERVED_MEM_SIZE_FOR_FB_MAX;
extern u64 RESERVED_MEM_SIZE_FOR_PMEM;

u64 RESERVED_MEM_SIZE_FOR_PMEM = 0x0;
u64 pmem_start = 0x12345678;  // pmem_start is inited in mt_fixup
u64 kernel_mem_sz = 0x0;       // kernel_mem_sz is inited in mt_fixup
u64 RESERVED_MEM_SIZE_FOR_TEST_3D = 0x0;
u64 FB_SIZE_EXTERN = 0x0;
u64 RESERVED_MEM_SIZE_FOR_FB_MAX = 0x1500000;


#define TOTAL_RESERVED_MEM_SIZE (RESERVED_MEM_SIZE_FOR_PMEM + \
                                 RESERVED_MEM_SIZE_FOR_FB)

#define MAX_PFN        ((max_pfn << PAGE_SHIFT) + PHYS_OFFSET)

#define PMEM_MM_START  (pmem_start)
#define PMEM_MM_SIZE   (RESERVED_MEM_SIZE_FOR_PMEM)

#define TEST_3D_START  (PMEM_MM_START + PMEM_MM_SIZE)
#define TEST_3D_SIZE   (RESERVED_MEM_SIZE_FOR_TEST_3D)

#define FB_START      (TEST_3D_START + RESERVED_MEM_SIZE_FOR_TEST_3D)
#define FB_SIZE       (RESERVED_MEM_SIZE_FOR_FB)
*/
//#define ITEMS_START    0x7fec0000  //FB_START-0x100000



struct mem_desc_t {
	unsigned long start;
	unsigned long size;
};

u64 items_start;

static int __init  item_map(void)
{
	unsigned long va;
	va= (unsigned long)ioremap_nocache(items_start, 0x100000);
	//printk("items va=%x pa=%x\n",(unsigned long)va,(unsigned long)items_start);
	printk("items *va=0x%08lx pa=%0x\n", va, (u32)items_start);
	item_init((char*)va,ITEM_SIZE_NORMAL);
	item_list(NULL);
	return 0;
}

static int dt_scan_memory(unsigned long node, const char *uname, int depth, void *data)
{
	char *type = (char *)of_get_flat_dt_prop(node, "device_type", NULL);
	__be32 *reg, *endp;
	int l;

	 struct mem_desc_t *mem_desc;

	/* We are scanning "memory" nodes only */
	if (type == NULL) {
		/*
		 * The longtrail doesn't have a device_type on the
		 * /memory node, so look for the node called /memory@0.
		 */
		if (depth != 1 || strcmp(uname, "memory@0") != 0)
			return 0;
	} else if (strcmp(type, "memory") != 0)
		return 0;

	reg = (__be32 *)of_get_flat_dt_prop(node, "reg", (int *)&l);
	if (reg == NULL)
		return 0;

	endp = reg + (l / sizeof(__be32));

	while ((endp - reg) >= (dt_root_addr_cells + dt_root_size_cells)) {
		u64 base, size;

		base = dt_mem_next_cell(dt_root_addr_cells, (const __be32 **)&reg);
		size = dt_mem_next_cell(dt_root_size_cells, (const __be32 **)&reg);

		if (size == 0)
			continue;
	}

	mem_desc = (struct mem_desc_t *)of_get_flat_dt_prop(node,
			"items_reserved_mem", NULL);
	/*
	if (mem_desc && mem_desc->size) {
		pr_notice("[PHY layout]items_reserved_mem   :   "
			"0x%08llx - 0x%08llx (0x%llx)\n",
			mem_desc->start,
			mem_desc->start +
			mem_desc->size - 1,
			mem_desc->size
			);
	}*/
	items_start = mem_desc->start;
	printk("items: of_get_flat_dt_prop  mem_desc->start = %0x\n",(u32)mem_desc->start);
	printk("kernel items: mem_desc->start = 0x%0x\n",(u32)mem_desc->start);
	printk("kernel items: mem_desc->szie = 0x%0x\n",(u32)mem_desc->size);
	return node;
}

static int __init items_early_memory_info(void)
{
	int node;
	node = of_scan_flat_dt(dt_scan_memory, NULL);
	return 0;
}
pure_initcall(items_early_memory_info);
postcore_initcall(item_map);

MODULE_AUTHOR("Nicholas Zheng <srxmcu@foxmail.com>");
MODULE_DESCRIPTION("item Driver");
MODULE_LICENSE("GPL");

