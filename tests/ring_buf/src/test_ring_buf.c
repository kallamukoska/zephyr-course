/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
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
	/* TODO(l8-task1): Push a value, call rb_init(4) again, then
	 * verify the buffer is empty and count is 0.
	 * See TEST_SPEC.md "Suite ring_buf_init" #2.
	 */
	zassume_equal(rb_push(99), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_init(4), 0, "Reinitializing the buffer must succeed.");

	zassert_true(rb_is_empty(), "Fresh buffer must be empty.");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0.");
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
	/* TODO(l8-task1): rb_push(42), rb_pop(&v) -> v == 42, buffer empty after.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #1.
	 */
	int v;
	zassume_equal(rb_push(42), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_pop(&v), 0, "Poping a value out of the ring buffer must succeed.");
	zassert_equal(v, 42, "Poped value must be correct.");
	zassert_true(rb_is_empty(), "Buffer with last value poped must be empty.");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	/* TODO(l8-task1): rb_push(1), rb_push(2), rb_push(3); pop three times
	 * and verify the values come out as 1, 2, 3 in that order.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #2.
	 */
	int v1, v2, v3;
	zassume_equal(rb_push(1), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_push(2), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_push(3), 0, "Pushing a value into the ring buffer must succeed.");

	zassume_equal(rb_pop(&v1), 0, "Poping a value out of the ring buffer must succeed.");
	zassume_equal(rb_pop(&v2), 0, "Poping a value out of the ring buffer must succeed.");
	zassume_equal(rb_pop(&v3), 0, "Poping a value out of the ring buffer must succeed.");
	
	zassert_equal(v1, 1, "Poped value must be correct.");
	zassert_equal(v2, 2, "Poped value must be correct.");
	zassert_equal(v3, 3, "Poped value must be correct.");
	
	zassert_true(rb_is_empty(), "Ring buffer must be empty.");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* TODO(l8-task1): Fill the buffer to its capacity of 4, then push
	 * one more value -> -ENOSPC.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #3.
	 */
	zassume_equal(rb_push(1), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_push(2), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_push(3), 0, "Pushing a value into the ring buffer must succeed.");
	zassume_equal(rb_push(4), 0, "Pushing a value into the ring buffer must succeed.");

	zassume_true(rb_is_full(), "Ring buffer must be full.");
	zassert_equal(rb_push(99), -ENOSPC, "Ring buffer overflow.");
	zassert_equal(rb_count(), 4, "Correct count of the ring buffer.");
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
	/* TODO(l8-task1): rb_push(7); rb_peek(&v) -> v == 7; rb_peek(&v) again
	 * -> v == 7; rb_count() still == 1.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #1.
	 */
	int v;

	zassert_equal(rb_push(7), 0, "Pushing a value into the ring buffer must succeed.");
	
	zassert_equal(rb_peek(&v), 0, "Peeking into the ring buffer must succeed.");
	zassert_equal(v, 7, "Peeked value must be correct.");

	zassert_equal(rb_peek(&v), 0, "Peeking into the ring buffer must succeed.");
	zassert_equal(v, 7, "Peeked value must be correct.");

	zassert_equal(rb_count(), 1, "Peek must not consume.");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* TODO(l8-task1): rb_pop(NULL) -> -EINVAL.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #2.
	 */
	zassert_equal(rb_pop(NULL), -EINVAL, "Invalid value poped from the ring buffer.");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* TODO(l8-task1): push 4 values -> rb_is_full() == true, rb_count() == 4.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #3.
	 */
	zassert_equal(rb_push(1), 0, "Pushing a value into the ring buffer must succeed.");
	zassert_equal(rb_push(2), 0, "Pushing a value into the ring buffer must succeed.");
	zassert_equal(rb_push(3), 0, "Pushing a value into the ring buffer must succeed.");
	zassert_equal(rb_push(4), 0, "Pushing a value into the ring buffer must succeed.");

	zassert_true(rb_is_full(), "Ring buffer must be full.");
	zassert_equal(rb_count(), 4, "Correct count of the ring buffer.");
}
