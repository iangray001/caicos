#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
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
	{ 0xef0eea15, "module_layout" },
	{ 0x68d7418f, "device_remove_bin_file" },
	{ 0x5a34a45c, "__kmalloc" },
	{ 0xac1477e8, "dev_set_drvdata" },
	{ 0x18717589, "pci_disable_device" },
	{ 0xaebbd8c1, "pci_release_regions" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0x4d6353f2, "pci_set_master" },
	{ 0x8aeae331, "pci_set_dma_mask" },
	{ 0xcece049a, "device_del" },
	{ 0x1dddd350, "pci_iounmap" },
	{ 0x3db3dc2d, "device_find_child" },
	{ 0xea147363, "printk" },
	{ 0xab545e9c, "sysfs_create_group" },
	{ 0x5181f2e0, "class_unregister" },
	{ 0xff879ff0, "device_create" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0x2ee68475, "pci_clear_master" },
	{ 0xc57aecc4, "put_device" },
	{ 0x93fca811, "__get_free_pages" },
	{ 0xec6a1375, "device_create_bin_file" },
	{ 0x819860ff, "pci_unregister_driver" },
	{ 0xe52947e7, "__phys_addr" },
	{ 0x4302d0eb, "free_pages" },
	{ 0xb29d36c, "device_for_each_child" },
	{ 0x37a0cba, "kfree" },
	{ 0x3aa6e03e, "pci_request_regions" },
	{ 0x577bc166, "pci_disable_msi" },
	{ 0xa8994138, "__pci_register_driver" },
	{ 0x4723892a, "class_destroy" },
	{ 0x1675606f, "bad_dma_address" },
	{ 0x8a9c561f, "pci_enable_msi_block" },
	{ 0x3bc00847, "pci_iomap" },
	{ 0x436c2179, "iowrite32" },
	{ 0xe905648d, "pci_enable_device" },
	{ 0xbd0a32c4, "__class_create" },
	{ 0x710f63eb, "dev_get_drvdata" },
	{ 0x601c902, "dma_ops" },
	{ 0xe484e35f, "ioread32" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v000010EEd00007011sv*sd*bc*sc*i*");
