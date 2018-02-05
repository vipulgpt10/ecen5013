#include <math.h>
#include <stdlib.h>
#include <stdint.h>

#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "dlist.c"

void test_insert_beg_NULL_HEAD(void **state)
{
	struct dlNode * head = NULL;
	uint32_t ndata = 10;
	struct dlNode * ptr = insert_at_beginning(&head, ndata);
	size_t lsize = list_size(head);
	assert_int_equal(head, ptr);
	assert_int_equal(head->data, ndata);
	assert_int_equal(lsize, 1);
}

void test_insert_end_NULL_HEAD(void **state)
{
	struct dlNode * head = NULL;
	uint32_t ndata = 11;
	struct dlNode * ptr = insert_at_end(&head, ndata);
	size_t lsize = list_size(head);
	assert_int_equal(head, ptr);
	assert_int_equal(head->data, ndata);
	assert_int_equal(lsize, 1);
}

void test_insert_pos_AFTER_HEAD(void **state)
{
	struct dlNode * head = NULL;
	uint32_t ndata = 20, pos = 0;
	struct dlNode * ptr = insert_at_end(&head, 0);
	ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);
	/* Insert at last */
	ptr = insert_at_position(head, ndata, pos);
	size_t lsize = list_size(head);
	assert_int_equal(head, ptr);
	assert_int_equal(lsize, 4);

	struct dlNode * tmp = head;
	tmp = tmp->next;
	assert_int_equal(tmp->data, ndata);
}

void test_insert_pos_AT_TAIL(void **state)
{
	struct dlNode * head = NULL;
	uint32_t ndata = 21, pos = 2;
	struct dlNode * ptr = insert_at_end(&head, 0);
	ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);
	/* Insert at last */
	ptr = insert_at_position(head, ndata, pos);
	size_t lsize = list_size(head);
	assert_int_equal(head, ptr);
	assert_int_equal(lsize, 4);

	struct dlNode * tmp = head;
	while(tmp->next != NULL)
		tmp = tmp->next;
	assert_int_equal(tmp->data, ndata);
}

void test_delete_beg_CHECK_HEAD(void **state)
{
	struct dlNode * head = NULL;
	struct dlNode * ptr1 = insert_at_beginning(&head, 1);
	struct dlNode * ptr2 = insert_at_beginning(&head, 2);

	struct dlNode * ptr3 = delete_from_beginning(&head);
	size_t lsize = list_size(head);
	assert_int_equal(ptr1, ptr3);
	assert_int_equal(lsize, 1);
	assert_int_equal(head->data, 1);
}

void test_delete_end_CHECK_HEAD(void **state)
{
	struct dlNode * head = NULL;
	struct dlNode * ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);

	ptr = delete_from_end(&head);

	struct dlNode * tmp = head;
	while(tmp->next != NULL)
		tmp = tmp->next;

	size_t lsize = list_size(head);
	assert_int_equal(ptr, head);
	assert_int_equal(lsize, 1);
	assert_int_equal(tmp->data, 1);
}


void test_delete_pos_CHECK_POS(void **state)
{
	struct dlNode * head = NULL;
	struct dlNode * ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);
	ptr = insert_at_end(&head, 3);

	/* Delete the node after head */
	ptr = delete_from_position(&head, 1);

	struct dlNode * tmp = head;
	tmp = tmp->next;

	size_t lsize = list_size(head);
	assert_int_equal(ptr, head);
	assert_int_equal(lsize, 2);
	assert_int_equal(tmp->data, 3);
}

void test_destroy_CHECK_SIZE(void **state)
{
	struct dlNode * head = NULL;
	struct dlNode * ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);
	ptr = insert_at_end(&head, 3);

	ptr = destroy(&head);

	size_t lsize = list_size(head);
	assert_int_equal(ptr, head);
	assert_int_equal(lsize, 0);
	assert_int_equal(head, NULL);
}

void test_lsize_CHECK(void **state)
{
	struct dlNode * head = NULL;
	struct dlNode * ptr = insert_at_end(&head, 1);
	ptr = insert_at_end(&head, 2);
	ptr = insert_at_end(&head, 3);

	size_t lsize = list_size(head);
	assert_int_equal(ptr, head);
	assert_int_equal(lsize, 3);
}


int main(int argc, char **argv)
{
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_insert_beg_NULL_HEAD),
	cmocka_unit_test(test_insert_end_NULL_HEAD),
	cmocka_unit_test(test_insert_pos_AFTER_HEAD),
	cmocka_unit_test(test_insert_pos_AT_TAIL),
	cmocka_unit_test(test_delete_beg_CHECK_HEAD),
	cmocka_unit_test(test_delete_end_CHECK_HEAD),
	cmocka_unit_test(test_delete_pos_CHECK_POS),
	cmocka_unit_test(test_destroy_CHECK_SIZE),
	cmocka_unit_test(test_lsize_CHECK)
	};
	// cmocka_unit_test(test_src_in_dst_overlap),


	return cmocka_run_group_tests(tests, NULL, NULL);
}