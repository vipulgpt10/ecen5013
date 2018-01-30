 /**
  * @file file_op.c
  * @brief The implementation file for testing file operations
  *
  * This implementation file provides the function definition for testing
  * various file operations.
  *
  * @author Vipul Gupta
  * @date 29 January 2018
  *
  */


#include "fileop.h"


void print_name(char * myname)
{
	printf("\nHello %s!\n", myname);
}

void char_file(char ip)
{
	FILE * fptr = fopen("file_test.txt", "w+");
	
	if(fptr == NULL)
		return;
	else
	{
		fputc(ip, fptr);
		fclose(fptr);
		return;	
	}
}

void str_file(char * ip)
{
	FILE * fptr = fopen("file_test.txt", "a+");

	if(fptr == NULL)
		return;
	else
	{
		fputc('\n', fptr);
		fputs(ip, fptr);
		fclose(fptr);
		return;
	}
}

void read_file(void)
{
	FILE * fptr = fopen("file_test.txt", "r+");

	if(fptr == NULL)
		return;
	else
	{
		char ch;
		ch = fgetc(fptr);
		while(ch != EOF)
		{
			printf("%c", ch);
			ch = fgetc(fptr);
		}

		fclose(fptr);
	}
}


int main()
{
	//
	char myname[20];
	printf("Enter your name...\n");
	scanf("%s", myname);
	print_name(myname);

	char ch;	
	printf("Enter the character to write on the file..\n");
	scanf(" %c", &ch);
	char_file(ch);

	char * str = (char *)malloc(80 * sizeof(char));
	if(str == NULL)
	{
		printf("Null pointer returned!");
		return 0;
	}
	printf("Enter the string to write on the file..\n");
	scanf(" %[^\n]s", str);
	str_file(str);

	printf("Reading from file...");
	read_file();
	free(str);
	printf("\n");

	return 0;
}
