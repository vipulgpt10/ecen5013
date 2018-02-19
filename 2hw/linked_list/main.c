/* test file */

#include "dlist.h"


struct info
{
	uint32_t data;
	struct dlNode node;
};

void traverse(struct dlNode * head)
{
	while(head!=NULL)
	{
		printf("%p\n", head);
		head = head->next;
	}
}

int main()
{
	

	struct dlNode * head = NULL;


	insert_at_beginning(&head, 1);
	insert_at_position(head, 2, 0);
	// // insert_at_beginning(&head);

	// insert_at_end(&head);
	// insert_at_end(&head);

	printf(" Size = %d\n", (int)list_size(head));

	traverse(head);

	printf("----\n");

	// delete_from_end(&head);

	// insert_at_beginning(&head);

	// delete_from_position(&head, 2);
	// insert_at_end(&head);

	// destroy(&head);

	printf(" Size = %d\n", (int)list_size(head));



	traverse(head);

	// free(head);

	return 0;
}


