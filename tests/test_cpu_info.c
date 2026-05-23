#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_get_cpu_info 472

struct cpu_info {
  unsigned long mvendorid;
  unsigned long marchid;
  unsigned long mimpid;
  unsigned long mhartid;
  unsigned long misa;
};

static int passed;
static int failed;

#define TEST(name, cond)                                                       \
  do {                                                                         \
    if (cond) {                                                                \
      printf("[PASS] %s\n", name);                                             \
      passed++;                                                                \
    } else {                                                                   \
      printf("[FAIL] %s\n", name);                                             \
      failed++;                                                                \
    }                                                                          \
  } while (0)

static long get_cpu_info(struct cpu_info *info) {
  return syscall(__NR_get_cpu_info, info);
}

static void test_basic(void) {
  struct cpu_info info;

  memset(&info, 0, sizeof(info));

  long ret = get_cpu_info(&info);

  TEST("syscall returns 0", ret == 0);

  TEST("mvendorid != 0", info.mvendorid != 0);
  TEST("marchid   != 0", info.marchid != 0);
  TEST("misa      != 0", info.misa != 0);

  printf("mvendorid = 0x%lx\n", info.mvendorid);
  printf("marchid   = 0x%lx\n", info.marchid);
  printf("mimpid    = 0x%lx\n", info.mimpid);
  printf("mhartid   = 0x%lx\n", info.mhartid);
  printf("misa      = 0x%lx\n", info.misa);
}

static void test_null(void) {
  long ret = get_cpu_info(NULL);

  TEST("NULL pointer returns -EFAULT", ret == -1 && errno == EFAULT);
}

static void test_repeatability(void) {
  struct cpu_info a, b;

  get_cpu_info(&a);
  get_cpu_info(&b);

  TEST("CPU info stable across calls", a.mvendorid == b.mvendorid &&
                                           a.marchid == b.marchid &&
                                           a.misa == b.misa);
}

int main(void) {
  printf("=== test_cpu_info ===\n\n");

  test_basic();
  test_null();
  test_repeatability();

  printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);

  return failed ? 1 : 0;
}
