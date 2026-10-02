/*
 * Ring Buffer Module - Homework Test Implementation
 *
 * Implemented according to TEST_SPEC.md.
 *
 * Run:
 *   west twister -T tests/ring_buf -p qemu_x86 (on Windows)
 *   west twister -T tests/ring_buf -p native_sim (on Linux/WSL)
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	zassert_ok(rb_push(99), "Push failed");
	zassert_false(rb_is_empty(), "Buffer should not be empty after push");

	/* Re-initialize buffer */
	zassert_ok(rb_init(4), "rb_init(4) failed");

	/* Verify state is cleared */
	zassert_true(rb_is_empty(), "Buffer must be empty after reinit");
	zassert_equal(rb_count(), 0, "Buffer count must be 0 after reinit");
	zassert_false(rb_is_full(), "Buffer should not be full after reinit");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int val = 0;

	zassert_ok(rb_push(42), "Pushing single item should succeed");
	zassert_ok(rb_pop(&val), "Popping single item should succeed");
	zassert_equal(val, 42, "Popped value should match pushed value");
	zassert_true(rb_is_empty(), "Buffer should be empty after pop");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int val = 0;

	zassert_ok(rb_push(1), "Push 1 failed");
	zassert_ok(rb_push(2), "Push 2 failed");
	zassert_ok(rb_push(3), "Push 3 failed");

	zassert_ok(rb_pop(&val), "Pop 1 failed");
	zassert_equal(val, 1, "First out should be 1");

	zassert_ok(rb_pop(&val), "Pop 2 failed");
	zassert_equal(val, 2, "Second out should be 2");

	zassert_ok(rb_pop(&val), "Pop 3 failed");
	zassert_equal(val, 3, "Third out should be 3");

	zassert_true(rb_is_empty(), "Buffer should be empty after 3 pops");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	for (int i = 1; i <= 4; i++) {
		zassert_ok(rb_push(i), "Push %d failed", i);
	}

	zassert_true(rb_is_full(), "Buffer should be full at capacity 4");

	int ret = rb_push(99);
	zassert_equal(ret, -ENOSPC, "Pushing to full buffer should return -ENOSPC");
	zassert_equal(rb_count(), 4, "Count should still be 4 after rejected push");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	int val = 0;

	zassert_ok(rb_push(7), "Push failed");

	zassert_ok(rb_peek(&val), "First peek failed");
	zassert_equal(val, 7, "First peek should return 7");

	val = 0;
	zassert_ok(rb_peek(&val), "Second peek failed");
	zassert_equal(val, 7, "Second peek should return 7");

	zassert_equal(rb_count(), 1, "Count should still be 1 after peeking");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	int ret = rb_pop(NULL);
	zassert_equal(ret, -EINVAL, "rb_pop(NULL) should return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	zassert_false(rb_is_full(), "Initial buffer should not be full");

	for (int i = 1; i <= 4; i++) {
		zassert_ok(rb_push(i), "Push %d failed", i);
	}

	zassert_true(rb_is_full(), "Buffer should report full after filling");
	zassert_equal(rb_count(), 4, "Buffer count should be 4");
}
