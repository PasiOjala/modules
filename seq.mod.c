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
	{ 0x9546d51a, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0xa4166a9b, __VMLINUX_SYMBOL_STR(seq_release) },
	{ 0xb3775f44, __VMLINUX_SYMBOL_STR(seq_read) },
	{ 0xcec966c4, __VMLINUX_SYMBOL_STR(seq_lseek) },
	{ 0x701e6c2c, __VMLINUX_SYMBOL_STR(remove_proc_entry) },
	{ 0x1dc2330e, __VMLINUX_SYMBOL_STR(proc_create_data) },
	{ 0x8ef51045, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0xd511a1f3, __VMLINUX_SYMBOL_STR(kmem_cache_alloc) },
	{ 0x37a0cba, __VMLINUX_SYMBOL_STR(kfree) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x4abdb61c, __VMLINUX_SYMBOL_STR(seq_printf) },
	{ 0xde36ffd1, __VMLINUX_SYMBOL_STR(seq_open) },
	{ 0xefd6cf06, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr0) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "BAAECFE893E133F1E8CA924");
