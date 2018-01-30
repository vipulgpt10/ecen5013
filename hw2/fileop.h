/**
 * @file file_op.h
 * @brief An abtraction for testing file operations
 *
 * This header file provides the prototypes of the function for testing
 * various file operations.
 *
 * @author Vipul Gupta
 * @date 29 January 2018
 *
 */

#ifndef __FILEOP_H__
#define __FILEOP_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * @brief Prints the string name
 *
 * Given the string, it will print it on the screen.
 *
 * @param myname String Name to display
 */
void print_name(char * myname);

/**
 * @brief Writes a character to the file (write mode)
 *
 * Given the character data, it will write it to the file.
 *
 * @param ip Character data
 */
void char_file(char ip);

/**
 * @brief Writes a string to the file (append mode)
 *
 * Given the string, it will write it to the file.
 *
 * @param ip String
 */
void str_file(char * ip);

/**
 * @brief Read from the file (read mode)
 *
 * It will read a character and a string and print it on the screen.
 *
 */
void read_file(void);

#endif /* __FILEOP_H__ */
