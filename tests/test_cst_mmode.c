#include <errno.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>

#define __NR_read_csr 474

#define CSR_MVENDORID 0xf11
#define CSR_MARCHID 0xf12
#define CSR_MIMPID 0xf13
#define CSR_MISA 0x301

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

static long read_csr(unsigned long csr, unsigned long *out) {
  return syscall(__NR_read_csr, csr, out);
}

static void test_whitelisted_csrs(void) {
  struct {
    const char *name;
    unsigned long num;
  } csrs[] = {
      {"mvendorid", CSR_MVENDORID},
      {"marchid", CSR_MARCHID},
      {"mimpid", CSR_MIMPID},
      {"misa", CSR_MISA},
  };

  for (int i = 0; i < 4; i++) {

    unsigned long value = 0;

    long ret = read_csr(csrs[i].num, &value);

    printf("%s = 0x%lx\n", csrs[i].name, value);

    TEST(csrs[i].name, ret == 0);
  }
}

static void test_invalid_csr(void) {
  unsigned long value = 0;

  long ret = read_csr(0xbadcafe, &value);

  TEST("invalid csr returns -EINVAL", ret == -1 && errno == EINVAL);
}

static void test_null_out(void) {
  long ret;

  ret = read_csr(CSR_MVENDORID, NULL);

  TEST("NULL out returns -EFAULT", ret == -1 && errno == EFAULT);
}

int main(void) {
  printf("=== test_csr_mmode ===\n\n");

  test_whitelisted_csrs();

  test_invalid_csr();

  test_null_out();

  printf("\n=== Results: %d passed, %d failed ===\n", passed, failed);

  return failed ? 1 : 0;
}
