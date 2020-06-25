cmd_/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o :=  gcc-9 -Wp,-MD,/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/.nvme_mock_module.o.d  -nostdinc -isystem /usr/lib/gcc/x86_64-linux-gnu/9/include -I/usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include -I./arch/x86/include/generated -I/usr/src/linux-headers-5.6.14-1rodete2-common/include -I./include -I/usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi -I./arch/x86/include/generated/uapi -I/usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi -I./include/generated/uapi -include /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kconfig.h -include /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler_types.h -D__KERNEL__ -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -fno-PIE -Werror=implicit-function-declaration -Werror=implicit-int -Wno-format-security -std=gnu89 -mno-sse -mno-mmx -mno-sse2 -mno-3dnow -mno-avx -m64 -falign-jumps=1 -falign-loops=1 -mno-80387 -mno-fp-ret-in-387 -mpreferred-stack-boundary=3 -mskip-rax-setup -mtune=generic -mno-red-zone -mcmodel=kernel -DCONFIG_X86_X32_ABI -DCONFIG_AS_CFI=1 -DCONFIG_AS_CFI_SIGNAL_FRAME=1 -DCONFIG_AS_CFI_SECTIONS=1 -DCONFIG_AS_SSSE3=1 -DCONFIG_AS_AVX=1 -DCONFIG_AS_AVX2=1 -DCONFIG_AS_AVX512=1 -DCONFIG_AS_SHA1_NI=1 -DCONFIG_AS_SHA256_NI=1 -DCONFIG_AS_ADX=1 -Wno-sign-compare -fno-asynchronous-unwind-tables -mindirect-branch=thunk-extern -mindirect-branch-register -fno-jump-tables -fno-delete-null-pointer-checks -Wno-frame-address -Wno-format-truncation -Wno-format-overflow -Wno-address-of-packed-member -O2 --param=allow-store-data-races=0 -Wframe-larger-than=2048 -fstack-protector-strong -Wno-unused-but-set-variable -Wimplicit-fallthrough -Wno-unused-const-variable -fno-var-tracking-assignments -g -pg -mrecord-mcount -mfentry -DCC_USING_FENTRY -flive-patching=inline-clone -Wdeclaration-after-statement -Wvla -Wno-pointer-sign -Wno-stringop-truncation -Wno-array-bounds -Wno-stringop-overflow -Wno-restrict -Wno-maybe-uninitialized -fno-strict-overflow -fno-merge-all-constants -fmerge-constants -fno-stack-check -fconserve-stack -Werror=date-time -Werror=incompatible-pointer-types -Werror=designated-init -fmacro-prefix-map=/usr/src/linux-headers-5.6.14-1rodete2-common/= -fcf-protection=none -Wno-packed-not-aligned  -DMODULE  -DKBUILD_BASENAME='"nvme_mock_module"' -DKBUILD_MODNAME='"nvme_mock_module"' -c -o /usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o /usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.c

source_/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o := /usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.c

deps_/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o := \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kconfig.h \
    $(wildcard include/config/cpu/big/endian.h) \
    $(wildcard include/config/booger.h) \
    $(wildcard include/config/foo.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler_types.h \
    $(wildcard include/config/have/arch/compiler/h.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/optimize/inlining.h) \
    $(wildcard include/config/cc/has/asm/inline.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler_attributes.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler-gcc.h \
    $(wildcard include/config/retpoline.h) \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/types.h \
    $(wildcard include/config/have/uid16.h) \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/types.h \
  arch/x86/include/generated/uapi/asm/types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/int-ll64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/int-ll64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/bitsperlong.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitsperlong.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/bitsperlong.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/posix_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stddef.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/stddef.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/posix_types.h \
    $(wildcard include/config/x86/32.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/posix_types_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/posix_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/init.h \
    $(wildcard include/config/have/arch/prel32/relocations.h) \
    $(wildcard include/config/strict/kernel/rwx.h) \
    $(wildcard include/config/strict/module/rwx.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/compiler.h \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/stack/validation.h) \
    $(wildcard include/config/kasan.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/barrier.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/alternative.h \
    $(wildcard include/config/smp.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stringify.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/asm.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/nops.h \
    $(wildcard include/config/mk7.h) \
    $(wildcard include/config/x86/p6/nop.h) \
    $(wildcard include/config/x86/64.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/barrier.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kasan-checks.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/module.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/sysfs.h) \
    $(wildcard include/config/modules/tree/lookup.h) \
    $(wildcard include/config/livepatch.h) \
    $(wildcard include/config/unused/symbols.h) \
    $(wildcard include/config/module/sig.h) \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/kallsyms.h) \
    $(wildcard include/config/tracepoints.h) \
    $(wildcard include/config/tree/srcu.h) \
    $(wildcard include/config/bpf/events.h) \
    $(wildcard include/config/jump/label.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/event/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
    $(wildcard include/config/module/unload.h) \
    $(wildcard include/config/constructors.h) \
    $(wildcard include/config/function/error/injection.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
    $(wildcard include/config/page/poisoning/zero.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/const.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/const.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/panic/timeout.h) \
  /usr/lib/gcc/x86_64-linux-gnu/9/include/stdarg.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/limits.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/limits.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/linkage.h \
    $(wildcard include/config/x86.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/export.h \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/module/rel/crcs.h) \
    $(wildcard include/config/trim/unused/ksyms.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/linkage.h \
    $(wildcard include/config/x86/alignment/16.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bitops.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bits.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/bitops.h \
    $(wildcard include/config/x86/cmov.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/rmwcc.h \
    $(wildcard include/config/cc/has/asm/goto.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/sched.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/arch_hweight.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cpufeatures.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/required-features.h \
    $(wildcard include/config/x86/minimum/cpu/family.h) \
    $(wildcard include/config/math/emulation.h) \
    $(wildcard include/config/x86/pae.h) \
    $(wildcard include/config/x86/cmpxchg64.h) \
    $(wildcard include/config/x86/use/3dnow.h) \
    $(wildcard include/config/matom.h) \
    $(wildcard include/config/paravirt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/disabled-features.h \
    $(wildcard include/config/x86/smap.h) \
    $(wildcard include/config/x86/umip.h) \
    $(wildcard include/config/x86/intel/memory/protection/keys.h) \
    $(wildcard include/config/x86/5level.h) \
    $(wildcard include/config/page/table/isolation.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/const_hweight.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/instrumented-atomic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/instrumented-non-atomic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/instrumented-lock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/le.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/byteorder.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/byteorder/little_endian.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/byteorder/little_endian.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/swab.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/swab.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/swab.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/byteorder/generic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bitops/ext2-atomic-setbit.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/typecheck.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/printk.h \
    $(wildcard include/config/message/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/default.h) \
    $(wildcard include/config/console/loglevel/quiet.h) \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk/nmi.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kern_levels.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/kernel.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/sysinfo.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cache.h \
    $(wildcard include/config/x86/l1/cache/shift.h) \
    $(wildcard include/config/x86/internode/cache/shift.h) \
    $(wildcard include/config/x86/vsmp.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dynamic_debug.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/jump_label.h \
    $(wildcard include/config/have/arch/jump/label/relative.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/jump_label.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/build_bug.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/div64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/div64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stat.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/stat.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/stat.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
    $(wildcard include/config/posix/timers.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/seqlock.h \
    $(wildcard include/config/debug/lock/alloc.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/preemption.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/preempt.h \
    $(wildcard include/config/preempt/count.h) \
    $(wildcard include/config/debug/preempt.h) \
    $(wildcard include/config/trace/preempt/toggle.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/preempt.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/percpu.h \
    $(wildcard include/config/x86/64/smp.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/percpu.h \
    $(wildcard include/config/have/setup/per/cpu/area.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/percpu-defs.h \
    $(wildcard include/config/debug/force/weak/per/cpu.h) \
    $(wildcard include/config/amd/mem/encrypt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/thread_info.h \
    $(wildcard include/config/thread/info/in/task.h) \
    $(wildcard include/config/have/arch/within/stack/frames.h) \
    $(wildcard include/config/hardened/usercopy.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bug.h \
    $(wildcard include/config/bug/on/data/corruption.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/bug.h \
    $(wildcard include/config/debug/bugverbose.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/restart_block.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/time64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/math64.h \
    $(wildcard include/config/arch/supports/int128.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/time.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/time_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/current.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/thread_info.h \
    $(wildcard include/config/vm86.h) \
    $(wildcard include/config/x86/iopl/ioperm.h) \
    $(wildcard include/config/frame/pointer.h) \
    $(wildcard include/config/compat.h) \
    $(wildcard include/config/ia32/emulation.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/page.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/page_types.h \
    $(wildcard include/config/physical/start.h) \
    $(wildcard include/config/physical/align.h) \
    $(wildcard include/config/dynamic/physical/mask.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mem_encrypt.h \
    $(wildcard include/config/arch/has/mem/encrypt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mem_encrypt.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/bootparam.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/screen_info.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/screen_info.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/apm_bios.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/apm_bios.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/ioctl.h \
  arch/x86/include/generated/uapi/asm/ioctl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/ioctl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/ioctl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/edd.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/edd.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/ist.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/ist.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/video/edid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/video/edid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/page_64_types.h \
    $(wildcard include/config/dynamic/memory/layout.h) \
    $(wildcard include/config/randomize/base.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/kaslr.h \
    $(wildcard include/config/randomize/memory.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/page_64.h \
    $(wildcard include/config/debug/virtual.h) \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/x86/vsyscall/emulation.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/range.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/memory_model.h \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pfn.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/getorder.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cpufeature.h \
    $(wildcard include/config/x86/feature/names.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/processor.h \
    $(wildcard include/config/x86/vmx/feature/names.h) \
    $(wildcard include/config/kvm.h) \
    $(wildcard include/config/stackprotector.h) \
    $(wildcard include/config/paravirt/xxl.h) \
    $(wildcard include/config/x86/debugctlmsr.h) \
    $(wildcard include/config/cpu/sup/amd.h) \
    $(wildcard include/config/xen.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/processor-flags.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/processor-flags.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/math_emu.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/ptrace.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/segment.h \
    $(wildcard include/config/xen/pv.h) \
    $(wildcard include/config/x86/32/lazy/gs.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/ptrace.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/ptrace-abi.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/paravirt_types.h \
    $(wildcard include/config/pgtable/levels.h) \
    $(wildcard include/config/paravirt/debug.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/desc_defs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/kmap_types.h \
    $(wildcard include/config/debug/highmem.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/kmap_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable_types.h \
    $(wildcard include/config/mem/soft/dirty.h) \
    $(wildcard include/config/proc/fs.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable_64_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/sparsemem.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/pgtable-nop4d.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/nospec-branch.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/static_key.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/alternative-asm.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/msr-index.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/spinlock_types.h \
    $(wildcard include/config/paravirt/spinlocks.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/qspinlock_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/qrwlock_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/sigcontext.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/msr.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/msr-index.h \
  arch/x86/include/generated/uapi/asm/errno.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/errno.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/errno-base.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cpumask.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bitmap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
    $(wildcard include/config/fortify/source.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/string.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/string.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/string_64.h \
    $(wildcard include/config/x86/mce.h) \
    $(wildcard include/config/arch/has/uaccess/flushcache.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/atomic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/atomic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cmpxchg.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cmpxchg_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/atomic64_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/atomic-instrumented.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/atomic-fallback.h \
    $(wildcard include/config/generic/atomic64.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/atomic-long.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/msr.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/tracepoint-defs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/paravirt.h \
    $(wildcard include/config/debug/entry.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/frame.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/special_insns.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/fpu/types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/unwind_hints.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/orc_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/vmxfeatures.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/personality.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/personality.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/err.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/irqflags.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bottom_half.h \
  arch/x86/include/generated/asm/mmiowb.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/mmiowb.h \
    $(wildcard include/config/mmiowb.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/spinlock_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rwlock_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/spinlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/qspinlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/qspinlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/qrwlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/qrwlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rwlock.h \
    $(wildcard include/config/preempt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/uninline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
    $(wildcard include/config/generic/lockbreak.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/time32.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/timex.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/timex.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/param.h \
  arch/x86/include/generated/uapi/asm/param.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/param.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/timex.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/tsc.h \
    $(wildcard include/config/x86/tsc.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/uidgid.h \
    $(wildcard include/config/multiuser.h) \
    $(wildcard include/config/user/ns.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/highuid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kmod.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/umh.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/gfp.h \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/zone/dma.h) \
    $(wildcard include/config/zone/dma32.h) \
    $(wildcard include/config/zone/device.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/pm/sleep.h) \
    $(wildcard include/config/contig/alloc.h) \
    $(wildcard include/config/cma.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mmdebug.h \
    $(wildcard include/config/debug/vm.h) \
    $(wildcard include/config/debug/vm/pgflags.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mmzone.h \
    $(wildcard include/config/force/max/zoneorder.h) \
    $(wildcard include/config/memory/isolation.h) \
    $(wildcard include/config/shuffle/page/allocator.h) \
    $(wildcard include/config/zsmalloc.h) \
    $(wildcard include/config/memcg.h) \
    $(wildcard include/config/memory/hotplug.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/transparent/hugepage.h) \
    $(wildcard include/config/flat/node/mem/map.h) \
    $(wildcard include/config/page/extension.h) \
    $(wildcard include/config/deferred/struct/page/init.h) \
    $(wildcard include/config/have/memory/present.h) \
    $(wildcard include/config/have/memoryless/nodes.h) \
    $(wildcard include/config/have/memblock/node/map.h) \
    $(wildcard include/config/need/multiple/nodes.h) \
    $(wildcard include/config/have/arch/early/pfn/to/nid.h) \
    $(wildcard include/config/sparsemem/extreme.h) \
    $(wildcard include/config/memory/hotremove.h) \
    $(wildcard include/config/have/arch/pfn/valid.h) \
    $(wildcard include/config/holes/in/zone.h) \
    $(wildcard include/config/arch/has/holes/memorymodel.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/wait.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/wait.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/numa.h \
    $(wildcard include/config/nodes/shift.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/nodemask.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pageblock-flags.h \
    $(wildcard include/config/hugetlb/page.h) \
    $(wildcard include/config/hugetlb/page/size/variable.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/page-flags-layout.h \
    $(wildcard include/config/numa/balancing.h) \
    $(wildcard include/config/kasan/sw/tags.h) \
  include/generated/bounds.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mm_types.h \
    $(wildcard include/config/have/aligned/struct/page.h) \
    $(wildcard include/config/userfaultfd.h) \
    $(wildcard include/config/swap.h) \
    $(wildcard include/config/have/arch/compat/mmap/bases.h) \
    $(wildcard include/config/membarrier.h) \
    $(wildcard include/config/aio.h) \
    $(wildcard include/config/mmu/notifier.h) \
    $(wildcard include/config/arch/want/batched/unmap/tlb/flush.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mm_types_task.h \
    $(wildcard include/config/split/ptlock/cpus.h) \
    $(wildcard include/config/arch/enable/split/pmd/ptlock.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/tlbbatch.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/auxvec.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/auxvec.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/auxvec.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rbtree.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcupdate.h \
    $(wildcard include/config/preempt/rcu.h) \
    $(wildcard include/config/rcu/stall/common.h) \
    $(wildcard include/config/no/hz/full.h) \
    $(wildcard include/config/rcu/nocb/cpu.h) \
    $(wildcard include/config/tasks/rcu.h) \
    $(wildcard include/config/tree/rcu.h) \
    $(wildcard include/config/tiny/rcu.h) \
    $(wildcard include/config/debug/objects/rcu/head.h) \
    $(wildcard include/config/prove/rcu.h) \
    $(wildcard include/config/rcu/boost.h) \
    $(wildcard include/config/arch/weak/release/acquire.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcutree.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rwsem.h \
    $(wildcard include/config/rwsem/spin/on/owner.h) \
    $(wildcard include/config/debug/rwsems.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/osq_lock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/completion.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/uprobes.h \
    $(wildcard include/config/uprobes.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/errno.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/errno.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/uprobes.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/notifier.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mutex.h \
    $(wildcard include/config/mutex/spin/on/owner.h) \
    $(wildcard include/config/debug/mutexes.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/debug_locks.h \
    $(wildcard include/config/debug/locking/api/selftests.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/srcu.h \
    $(wildcard include/config/tiny/srcu.h) \
    $(wildcard include/config/srcu.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/workqueue.h \
    $(wildcard include/config/debug/objects/work.h) \
    $(wildcard include/config/freezer.h) \
    $(wildcard include/config/wq/watchdog.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/timer.h \
    $(wildcard include/config/debug/objects/timers.h) \
    $(wildcard include/config/preempt/rt.h) \
    $(wildcard include/config/no/hz/common.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ktime.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/jiffies.h \
  include/generated/timeconst.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/timekeeping.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/timekeeping32.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcu_segcblist.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/srcutree.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcu_node_tree.h \
    $(wildcard include/config/rcu/fanout.h) \
    $(wildcard include/config/rcu/fanout/leaf.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mmu.h \
    $(wildcard include/config/modify/ldt/syscall.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/page-flags.h \
    $(wildcard include/config/arch/uses/pg/uncached.h) \
    $(wildcard include/config/memory/failure.h) \
    $(wildcard include/config/idle/page/tracking.h) \
    $(wildcard include/config/thp/swap.h) \
    $(wildcard include/config/ksm.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/memory_hotplug.h \
    $(wildcard include/config/arch/has/add/pages.h) \
    $(wildcard include/config/have/arch/nodedata/extension.h) \
    $(wildcard include/config/have/bootmem/info/node.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mmzone.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mmzone_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/smp.h \
    $(wildcard include/config/x86/local/apic.h) \
    $(wildcard include/config/x86/io/apic.h) \
    $(wildcard include/config/debug/nmi/selftest.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mpspec.h \
    $(wildcard include/config/eisa.h) \
    $(wildcard include/config/x86/mpparse.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/mpspec_def.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/x86_init.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/apicdef.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/apic.h \
    $(wildcard include/config/x86/x2apic.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/fixmap.h \
    $(wildcard include/config/provide/ohci1394/dma/init.h) \
    $(wildcard include/config/pci/mmconfig.h) \
    $(wildcard include/config/x86/intel/mid.h) \
    $(wildcard include/config/acpi/apei/ghes.h) \
    $(wildcard include/config/intel/txt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/acpi.h \
    $(wildcard include/config/acpi/apei.h) \
    $(wildcard include/config/acpi.h) \
    $(wildcard include/config/acpi/numa.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/acpi/pdc_intel.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/numa.h \
    $(wildcard include/config/numa/emu.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/topology.h \
    $(wildcard include/config/sched/mc/prio.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/topology.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/vsyscall.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/fixmap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/hardirq.h \
    $(wildcard include/config/kvm/intel.h) \
    $(wildcard include/config/have/kvm.h) \
    $(wildcard include/config/x86/thermal/vector.h) \
    $(wildcard include/config/x86/mce/threshold.h) \
    $(wildcard include/config/x86/mce/amd.h) \
    $(wildcard include/config/x86/hv/callback/vector.h) \
    $(wildcard include/config/hyperv.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/io_apic.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/irq_vectors.h \
    $(wildcard include/config/pci/msi.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/topology.h \
    $(wildcard include/config/use/percpu/numa/node/id.h) \
    $(wildcard include/config/sched/smt.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/arch_topology.h \
    $(wildcard include/config/generic/arch/topology.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/percpu.h \
    $(wildcard include/config/need/per/cpu/embed/first/chunk.h) \
    $(wildcard include/config/need/per/cpu/page/first/chunk.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/smp.h \
    $(wildcard include/config/up/late/init.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/llist.h \
    $(wildcard include/config/arch/have/nmi/safe/cmpxchg.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sysctl.h \
    $(wildcard include/config/sysctl.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/sysctl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/elf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/elf.h \
    $(wildcard include/config/x86/x32/abi.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/user.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/user_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/fsgsbase.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/syscall.h \
    $(wildcard include/config/x86/x32/disabled.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/audit.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/elf-em.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched.h \
    $(wildcard include/config/virt/cpu/accounting/native.h) \
    $(wildcard include/config/sched/info.h) \
    $(wildcard include/config/schedstats.h) \
    $(wildcard include/config/fair/group/sched.h) \
    $(wildcard include/config/rt/group/sched.h) \
    $(wildcard include/config/uclamp/task.h) \
    $(wildcard include/config/uclamp/buckets/count.h) \
    $(wildcard include/config/cgroup/sched.h) \
    $(wildcard include/config/blk/dev/io/trace.h) \
    $(wildcard include/config/psi.h) \
    $(wildcard include/config/compat/brk.h) \
    $(wildcard include/config/cgroups.h) \
    $(wildcard include/config/blk/cgroup.h) \
    $(wildcard include/config/arch/has/scaled/cputime.h) \
    $(wildcard include/config/virt/cpu/accounting/gen.h) \
    $(wildcard include/config/posix/cputimers.h) \
    $(wildcard include/config/keys.h) \
    $(wildcard include/config/sysvipc.h) \
    $(wildcard include/config/detect/hung/task.h) \
    $(wildcard include/config/audit.h) \
    $(wildcard include/config/auditsyscall.h) \
    $(wildcard include/config/rt/mutexes.h) \
    $(wildcard include/config/ubsan.h) \
    $(wildcard include/config/block.h) \
    $(wildcard include/config/task/xacct.h) \
    $(wildcard include/config/cpusets.h) \
    $(wildcard include/config/x86/cpu/resctrl.h) \
    $(wildcard include/config/futex.h) \
    $(wildcard include/config/perf/events.h) \
    $(wildcard include/config/rseq.h) \
    $(wildcard include/config/task/delay/acct.h) \
    $(wildcard include/config/fault/injection.h) \
    $(wildcard include/config/latencytop.h) \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/kcov.h) \
    $(wildcard include/config/bcache.h) \
    $(wildcard include/config/vmap/stack.h) \
    $(wildcard include/config/security.h) \
    $(wildcard include/config/gcc/plugin/stackleak.h) \
    $(wildcard include/config/arch/task/struct/on/stack.h) \
    $(wildcard include/config/debug/rseq.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/sched.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rculist.h \
    $(wildcard include/config/prove/rcu/list.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/refcount.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sem.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/sem.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ipc.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rhashtable-types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/ipc.h \
  arch/x86/include/generated/uapi/asm/ipcbuf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/ipcbuf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/sembuf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/shm.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/shm.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/hugetlb_encode.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/shmbuf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/shmbuf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/shmparam.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kcov.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/kcov.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/plist.h \
    $(wildcard include/config/debug/plist.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hrtimer.h \
    $(wildcard include/config/high/res/timers.h) \
    $(wildcard include/config/time/low/res.h) \
    $(wildcard include/config/timerfd.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hrtimer_defs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/timerqueue.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/seccomp.h \
    $(wildcard include/config/seccomp.h) \
    $(wildcard include/config/have/arch/seccomp/filter.h) \
    $(wildcard include/config/seccomp/filter.h) \
    $(wildcard include/config/checkpoint/restore.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/seccomp.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/seccomp.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/unistd.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/unistd.h \
  arch/x86/include/generated/uapi/asm/unistd_64.h \
  arch/x86/include/generated/asm/unistd_64_x32.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/ia32_unistd.h \
  arch/x86/include/generated/asm/unistd_32_ia32.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/seccomp.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/unistd.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/resource.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/resource.h \
  arch/x86/include/generated/uapi/asm/resource.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/resource.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/resource.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/latencytop.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched/prio.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched/types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/signal_types.h \
    $(wildcard include/config/old/sigaction.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/signal.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/signal.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/signal.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/signal-defs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/uapi/asm/siginfo.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/siginfo.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/task_io_accounting.h \
    $(wildcard include/config/task/io/accounting.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/posix-timers.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/alarmtimer.h \
    $(wildcard include/config/rtc/class.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/rseq.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/asm-offsets.h \
  include/generated/asm-offsets.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/vdso.h \
    $(wildcard include/config/x86/x32.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/elf.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kobject.h \
    $(wildcard include/config/uevent/helper.h) \
    $(wildcard include/config/debug/kobject/release.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sysfs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kernfs.h \
    $(wildcard include/config/kernfs.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/idr.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/radix-tree.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/xarray.h \
    $(wildcard include/config/xarray/multi.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kconfig.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kobject_ns.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kref.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/moduleparam.h \
    $(wildcard include/config/alpha.h) \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/ppc64.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rbtree_latch.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/error-injection.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/error-injection.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/module.h \
    $(wildcard include/config/unwinder/orc.h) \
    $(wildcard include/config/m486sx.h) \
    $(wildcard include/config/m486.h) \
    $(wildcard include/config/m586.h) \
    $(wildcard include/config/m586tsc.h) \
    $(wildcard include/config/m586mmx.h) \
    $(wildcard include/config/mcore2.h) \
    $(wildcard include/config/m686.h) \
    $(wildcard include/config/mpentiumii.h) \
    $(wildcard include/config/mpentiumiii.h) \
    $(wildcard include/config/mpentiumm.h) \
    $(wildcard include/config/mpentium4.h) \
    $(wildcard include/config/mk6.h) \
    $(wildcard include/config/mk8.h) \
    $(wildcard include/config/melan.h) \
    $(wildcard include/config/mcrusoe.h) \
    $(wildcard include/config/mefficeon.h) \
    $(wildcard include/config/mwinchipc6.h) \
    $(wildcard include/config/mwinchip3d.h) \
    $(wildcard include/config/mcyrixiii.h) \
    $(wildcard include/config/mviac3/2.h) \
    $(wildcard include/config/mviac7.h) \
    $(wildcard include/config/mgeodegx1.h) \
    $(wildcard include/config/mgeode/lx.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/module.h \
    $(wildcard include/config/have/mod/arch/specific.h) \
    $(wildcard include/config/modules/use/elf/rel.h) \
    $(wildcard include/config/modules/use/elf/rela.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/orc_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/fs.h \
    $(wildcard include/config/read/only/thp/for/fs.h) \
    $(wildcard include/config/fs/posix/acl.h) \
    $(wildcard include/config/cgroup/writeback.h) \
    $(wildcard include/config/ima.h) \
    $(wildcard include/config/file/locking.h) \
    $(wildcard include/config/fsnotify.h) \
    $(wildcard include/config/fs/encryption.h) \
    $(wildcard include/config/fs/verity.h) \
    $(wildcard include/config/epoll.h) \
    $(wildcard include/config/quota.h) \
    $(wildcard include/config/fs/dax.h) \
    $(wildcard include/config/mandatory/file/locking.h) \
    $(wildcard include/config/migration.h) \
    $(wildcard include/config/io/uring.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/wait_bit.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kdev_t.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/kdev_t.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dcache.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rculist_bl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/list_bl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bit_spinlock.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/lockref.h \
    $(wildcard include/config/arch/use/cmpxchg/lockref.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stringhash.h \
    $(wildcard include/config/dcache/word/access.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hash.h \
    $(wildcard include/config/have/arch/hash.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/path.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/list_lru.h \
    $(wildcard include/config/memcg/kmem.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/shrinker.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/capability.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/capability.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/semaphore.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/fcntl.h \
    $(wildcard include/config/arch/32bit/off/t.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/fcntl.h \
  arch/x86/include/generated/uapi/asm/fcntl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/asm-generic/fcntl.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/openat2.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/fiemap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/migrate_mode.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/percpu-rwsem.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcuwait.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/rcu_sync.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/delayed_call.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/uuid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/uuid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/errseq.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ioprio.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched/rt.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/iocontext.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/fs_types.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/fs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/quota.h \
    $(wildcard include/config/quota/netlink/interface.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/percpu_counter.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/dqblk_xfs.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dqblk_v1.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dqblk_v2.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dqblk_qtree.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/projid.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/quota.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/nfs_fs_i.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/blkdev.h \
    $(wildcard include/config/blk/rq/alloc/time.h) \
    $(wildcard include/config/blk/wbt.h) \
    $(wildcard include/config/blk/dev/integrity.h) \
    $(wildcard include/config/blk/dev/zoned.h) \
    $(wildcard include/config/pm.h) \
    $(wildcard include/config/blk/dev/bsg.h) \
    $(wildcard include/config/blk/dev/throttling.h) \
    $(wildcard include/config/blk/debug/fs.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched/clock.h \
    $(wildcard include/config/have/unstable/sched/clock.h) \
    $(wildcard include/config/irq/time/accounting.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/major.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/genhd.h \
    $(wildcard include/config/fail/make/request.h) \
    $(wildcard include/config/solaris/x86/partition.h) \
    $(wildcard include/config/bsd/disklabel.h) \
    $(wildcard include/config/unixware/disklabel.h) \
    $(wildcard include/config/minix/subpartition.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/slab.h \
    $(wildcard include/config/debug/slab.h) \
    $(wildcard include/config/failslab.h) \
    $(wildcard include/config/have/hardened/usercopy/allocator.h) \
    $(wildcard include/config/slab.h) \
    $(wildcard include/config/slub.h) \
    $(wildcard include/config/slob.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/overflow.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/percpu-refcount.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/kasan.h \
    $(wildcard include/config/kasan/vmalloc.h) \
    $(wildcard include/config/kasan/generic.h) \
    $(wildcard include/config/kasan/inline.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/blk_types.h \
    $(wildcard include/config/blk/cgroup/iocost.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bvec.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mm.h \
    $(wildcard include/config/have/arch/mmap/rnd/bits.h) \
    $(wildcard include/config/have/arch/mmap/rnd/compat/bits.h) \
    $(wildcard include/config/arch/uses/high/vma/flags.h) \
    $(wildcard include/config/arch/has/pkeys.h) \
    $(wildcard include/config/ppc.h) \
    $(wildcard include/config/parisc.h) \
    $(wildcard include/config/sparc64.h) \
    $(wildcard include/config/x86/intel/mpx.h) \
    $(wildcard include/config/stack/growsup.h) \
    $(wildcard include/config/shmem.h) \
    $(wildcard include/config/dev/pagemap/ops.h) \
    $(wildcard include/config/device/private.h) \
    $(wildcard include/config/pci/p2pdma.h) \
    $(wildcard include/config/arch/has/pte/devmap.h) \
    $(wildcard include/config/debug/vm/rb.h) \
    $(wildcard include/config/page/poisoning.h) \
    $(wildcard include/config/init/on/alloc/default/on.h) \
    $(wildcard include/config/init/on/free/default/on.h) \
    $(wildcard include/config/debug/pagealloc.h) \
    $(wildcard include/config/arch/has/set/direct/map.h) \
    $(wildcard include/config/hibernation.h) \
    $(wildcard include/config/hugetlbfs.h) \
    $(wildcard include/config/mapping/dirty/helpers.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/page_ext.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stacktrace.h \
    $(wildcard include/config/stacktrace.h) \
    $(wildcard include/config/arch/stackwalk.h) \
    $(wildcard include/config/have/reliable/stacktrace.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/stackdepot.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/page_ref.h \
    $(wildcard include/config/debug/page/ref.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/memremap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ioport.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sizes.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable.h \
    $(wildcard include/config/debug/wx.h) \
    $(wildcard include/config/have/arch/transparent/hugepage/pud.h) \
    $(wildcard include/config/have/arch/soft/dirty.h) \
    $(wildcard include/config/arch/enable/thp/migration.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/fpu/xstate.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/uaccess.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/uaccess.h \
    $(wildcard include/config/x86/intel/usercopy.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/smap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/extable.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/uaccess_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/fpu/api.h \
    $(wildcard include/config/x86/debug/fpu.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable_64.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable-invert.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/pgtable.h \
    $(wildcard include/config/have/arch/huge/vmap.h) \
    $(wildcard include/config/x86/espfix64.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/huge_mm.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/sched/coredump.h \
    $(wildcard include/config/core/dump/default/elf/headers.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/vmstat.h \
    $(wildcard include/config/vm/event/counters.h) \
    $(wildcard include/config/debug/tlbflush.h) \
    $(wildcard include/config/debug/vm/vmacache.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/vm_event_item.h \
    $(wildcard include/config/memory/balloon.h) \
    $(wildcard include/config/balloon/compaction.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/local.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/device.h \
    $(wildcard include/config/debug/devres.h) \
    $(wildcard include/config/generic/msi/irq/domain.h) \
    $(wildcard include/config/pinctrl.h) \
    $(wildcard include/config/generic/msi/irq.h) \
    $(wildcard include/config/dma/declare/coherent.h) \
    $(wildcard include/config/dma/cma.h) \
    $(wildcard include/config/arch/has/sync/dma/for/device.h) \
    $(wildcard include/config/arch/has/sync/dma/for/cpu.h) \
    $(wildcard include/config/arch/has/sync/dma/for/cpu/all.h) \
    $(wildcard include/config/of.h) \
    $(wildcard include/config/devtmpfs.h) \
    $(wildcard include/config/sysfs/deprecated.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/dev_printk.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ratelimit.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/klist.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pm.h \
    $(wildcard include/config/vt/console/sleep.h) \
    $(wildcard include/config/pm/clk.h) \
    $(wildcard include/config/pm/generic/domains.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/device/bus.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/device/class.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/device/driver.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/device.h \
    $(wildcard include/config/intel/iommu.h) \
    $(wildcard include/config/amd/iommu.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pm_wakeup.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/pagemap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/highmem.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hardirq.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/ftrace_irq.h \
    $(wildcard include/config/ftrace/nmi/enter.h) \
    $(wildcard include/config/hwlat/tracer.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/vtime.h \
    $(wildcard include/config/virt/cpu/accounting.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/context_tracking_state.h \
    $(wildcard include/config/context/tracking.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/cacheflush.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/cacheflush.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hugetlb_inline.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/backing-dev-defs.h \
    $(wildcard include/config/debug/fs.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/flex_proportions.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/mempool.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bio.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/bsg.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/bsg.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/scatterlist.h \
    $(wildcard include/config/need/sg/dma/length.h) \
    $(wildcard include/config/debug/sg.h) \
    $(wildcard include/config/sgl/alloc.h) \
    $(wildcard include/config/arch/no/sg/chain.h) \
    $(wildcard include/config/sg/pool.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/io.h \
    $(wildcard include/config/mtrr.h) \
    $(wildcard include/config/x86/pat.h) \
  arch/x86/include/generated/asm/early_ioremap.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/early_ioremap.h \
    $(wildcard include/config/generic/early/ioremap.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/iomap.h \
    $(wildcard include/config/has/ioport/map.h) \
    $(wildcard include/config/pci.h) \
    $(wildcard include/config/generic/iomap.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/pci_iomap.h \
    $(wildcard include/config/no/generic/pci/ioport/map.h) \
    $(wildcard include/config/generic/pci/iomap.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/asm-generic/io.h \
    $(wildcard include/config/generic/ioremap.h) \
    $(wildcard include/config/virt/to/bus.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/logic_pio.h \
    $(wildcard include/config/indirect/pio.h) \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/fwnode.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/vmalloc.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/vmalloc.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/arch/x86/include/asm/pgtable_areas.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/uapi/linux/blkzoned.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/elevator.h \
  /usr/src/linux-headers-5.6.14-1rodete2-common/include/linux/hashtable.h \

/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o: $(deps_/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o)

$(deps_/usr/local/google/home/basimsahaf/Desktop/workspace/scsi2nvme/modules/nvme_mock/nvme_mock_module.o):
