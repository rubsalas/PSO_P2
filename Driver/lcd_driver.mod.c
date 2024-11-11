#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x290c8a96, "module_layout" },
	{ 0x40a943fb, "cdev_del" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x49d590, "class_destroy" },
	{ 0x5ecde9bf, "device_destroy" },
	{ 0xfe990052, "gpio_free" },
	{ 0xf1f7fb73, "gpiod_direction_output_raw" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x2b3144a5, "cdev_add" },
	{ 0xfe7fdc41, "cdev_init" },
	{ 0xad232455, "device_create" },
	{ 0xdf84066, "__class_create" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x5f754e5a, "memset" },
	{ 0xae353d77, "arm_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xf9a482f9, "msleep" },
	{ 0xfbe2272c, "gpiod_set_raw_value" },
	{ 0x38e0975f, "gpio_to_desc" },
	{ 0x92997ed8, "_printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "00C1DFD14C4C64ECCBA4A46");
