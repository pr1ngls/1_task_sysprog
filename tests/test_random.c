#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_get_random 473

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

static long get_random(unsigned long *out) {
  return syscall(__NR_get_random, out);
}

static void test_basic(void) {
  unsigned long v = 0;

  long ret = get_random(&v);

  TEST("get_random returns 0", ret == 0);

  TEST("random value is non-zero", ret == 0 && v != 0);

  printf("random = 0x%lx\n", v);
}

static void test_changes(void) {
  unsigned long a = 0;
  unsigned long b = 0;

  get_random(&a);
  get_random(&b);

  TEST("random values differ", a != b);

  printf("random1 = 0x%lx\n", a);
  printf("random2 = 0x%lx\n", b);
}

static void test_null(void) {
  long ret = get_random(NULL);

  TEST("NULL pointer returns -EFAULT", ret == -1 && errno == EFAULT);
}

int main(void) {
  printf("=== test_random ===\n\n");

  test_basic();

  test_changes();

  test_null();

  printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);

  return failed ? 1 : 0;
}
