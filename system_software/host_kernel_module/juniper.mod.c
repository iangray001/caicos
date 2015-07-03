#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x27d7953d, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x7947e8d7, __VMLINUX_SYMBOL_STR(device_remove_bin_file) },
	{ 0x53fe6e81, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xbd549e7f, __VMLINUX_SYMBOL_STR(dma_set_mask) },
	{ 0xf6f01633, __VMLINUX_SYMBOL_STR(pci_disable_device) },
	{ 0xc408d0ce, __VMLINUX_SYMBOL_STR(pci_release_regions) },
	{ 0xf432dd3d, __VMLINUX_SYMBOL_STR(__init_waitqueue_head) },
	{ 0x4ea62cc9, __VMLINUX_SYMBOL_STR(pci_set_master) },
	{ 0x71be3b45, __VMLINUX_SYMBOL_STR(device_del) },
	{ 0x5c25f477, __VMLINUX_SYMBOL_STR(pci_iounmap) },
	{ 0xa99a4f15, __VMLINUX_SYMBOL_STR(device_find_child) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0x35d00c5c, __VMLINUX_SYMBOL_STR(sysfs_create_group) },
	{ 0x3e255a1f, __VMLINUX_SYMBOL_STR(class_unregister) },
	{ 0x4c9d28b0, __VMLINUX_SYMBOL_STR(phys_base) },
	{ 0x3e00ce0f, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0x96263528, __VMLINUX_SYMBOL_STR(pci_clear_master) },
	{ 0x847ffa20, __VMLINUX_SYMBOL_STR(put_device) },
	{ 0x93fca811, __VMLINUX_SYMBOL_STR(__get_free_pages) },
	{ 0x7f322024, __VMLINUX_SYMBOL_STR(device_create_bin_file) },
	{ 0xbdfb6dbb, __VMLINUX_SYMBOL_STR(__fentry__) },
	{ 0x25f924ce, __VMLINUX_SYMBOL_STR(pci_enable_msi_range) },
	{ 0xde75460a, __VMLINUX_SYMBOL_STR(pci_unregister_driver) },
	{ 0xca1de323, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x4302d0eb, __VMLINUX_SYMBOL_STR(free_pages) },
	{ 0x73af61a1, __VMLINUX_SYMBOL_STR(device_for_each_child) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x2eb0c37b, __VMLINUX_SYMBOL_STR(pci_request_regions) },
	{ 0x45647d51, __VMLINUX_SYMBOL_STR(pci_disable_msi) },
	{ 0x181c19c2, __VMLINUX_SYMBOL_STR(__pci_register_driver) },
	{ 0xa5249913, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x119f4c89, __VMLINUX_SYMBOL_STR(pci_iomap) },
	{ 0x436c2179, __VMLINUX_SYMBOL_STR(iowrite32) },
	{ 0x6e9ff1f2, __VMLINUX_SYMBOL_STR(pci_enable_device) },
	{ 0x5e10929f, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x663f5504, __VMLINUX_SYMBOL_STR(dma_ops) },
	{ 0xe484e35f, __VMLINUX_SYMBOL_STR(ioread32) },
	{ 0xf20dabd8, __VMLINUX_SYMBOL_STR(free_irq) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v000010EEd00007011sv*sd*bc*sc*i*");
