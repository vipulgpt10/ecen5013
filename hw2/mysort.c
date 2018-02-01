 /**
  * @file mysort.c
  * @brief The implementation file for sorting string data
  *
  * This implementation file provides the function definition for sorting 
  * string data.
  *
  * @author Vipul Gupta
  * @date 30 January 2018
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

void my_sort(uint8_t * str, size_t len, uint8_t * sort_str)
{

    uint8_t * hash_tbl = (uint8_t *)calloc(128, sizeof(uint8_t));

    if((str == NULL) || (sort_str == NULL))
        return;
    else
    {
        size_t i, cnt, j = 0;
        
        /* Store the count of each char in hash_tbl based on ASCII value */
        for(i = 0; i < len; i++)
        {
            *(hash_tbl + (*(str + i))) += 1;
        }

        for(i = 128; i > 0; i--)
        {
            cnt = *(hash_tbl + i - 1);
            if(cnt)
            {
                while(cnt--)
                {
                    *(sort_str + j) = (i - 1);
                    j++;
                }
            }
        }
        return;    
    }
}

int main()
{
    uint8_t i;
    uint8_t * buff = (uint8_t *)malloc(256 * sizeof(uint8_t));
    uint8_t * sort_buff = (uint8_t *)malloc(256 * sizeof(uint8_t));


    printf("Randomly generated string to sort:\n");

    srand(time(0));

    for(i = 0; i < 255; i++)
    {
        *(buff + i) = (rand() % 96) + 32;
        printf("%c", *(buff + i));

    }
    printf("\n");

    // scanf("%[^\n]s", buff);


    my_sort(buff, strlen(buff), sort_buff); 

    printf("\nSorted string:\n");
    printf("%s\n", sort_buff);

    return 0;
}