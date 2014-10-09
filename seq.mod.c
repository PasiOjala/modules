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
	{ 0x11c92bdf, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x2fdef982, __VMLINUX_SYMBOL_STR(seq_release) },
	{ 0xe887354a, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0x7c4c2659, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x7cc137e5, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x8bc20274, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0xd3813bef, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x7f457a3b, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0xfd485ec8, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xd6e713b0, __VMLINUX_SYMBOL_STR(seq_open) },
	{ 0xb4390f9a, __VMLINUX_SYMBOL_STR(mcount) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "BAAECFE893E133F1E8CA924");
