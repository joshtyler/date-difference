/* date1 Version 1.0
Author: Joshua Tyler
SOURCE CODE IS BEST VIEWED WITH A TAB WIDTH OF TWO */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Structure to hold a date */
typedef struct
{
  int day;
  int month;
  int year;
} date;

/* Function descriptions can be found with the function definitions. */
date getInput(void);
date validateInput(const char* string);
date printError(char* string);
int countDays(date date1, date date2);
int dayNumber(date date1);

/*
	Global constant array to store the number of days in each month.
	The first element is 0 so that array index matches month no.
	i.e. Feburary corresponds to daysInEachMonth[2]
*/
const int daysInEachMonth[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/* Same format as daysInEachMonth[], but the array stores the cumulative no. of days up to the start of each month */
const int cumulativeDaysTilEachMonth[13] = {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

/*
	Function: main()
	Purpose: Program to calculate the number of days between two dates.
	Input:	Two dates on stdin, in the format <day>/<month>/<year> or <day>-<month>-<year> (where <day>, <month> and <year> are integers), separated by a newline.
					If the input is invalid, the program will keep prompting for another input.
					Valid years are greater than 0 and less than 10000
	Output:	How many days after date1 date2 is (i.e. -ve if date2 is before date1), printed to stdout terminated by a newline character.
					Error messages will be printed to stderr if the input is invalid.
	Arguments: None.
	Return value: Returns EXIT_SUCCESS if the program runs successfully.
	IMPORTANT NOTE: This program does not account for leap years.
*/
int main(void)
{
	/* variables to store each formatted date */
	date date1, date2;

	date1 = getInput();
	date2 = getInput();

	printf("%d", countDays(date1, date2));

	return EXIT_SUCCESS;
}

/*
	Function getInput()
	Purpose: Requests strings from the user until a correctly formatted string is input, then converts the input to a date structure.
	Arguments: none.
	Output: Error message to stderr if the user enters an invalid date, or there is a memory allocation error.
	Return value: The date given by user (in a date structure)
*/
date getInput(void)
{
	/* Allocates memory to store input string */
	int strLen = 20;
	char *inputStr;
	inputStr = (char*) malloc(strLen * sizeof(char));
	if(inputStr == NULL)
	{
		printError("Memory allocation error.\nThe program will now exit.\n");
		exit(EXIT_FAILURE);
	}
	char *tempPtr;

	/* Holds formatted date, initialised to error value */
	date dateIn = {-1,0,0};

	/* Variable used to contian the current input from the user (it's an int in case getchar() returns EOF) */
	int c;

	/* loop counter */
	int i;
	
	/* Loops until a valid input is given */
	while(dateIn.day == -1)
	{
		/* Reads current line of stdin to input string, c is set to 'a' to ensure the loop runs at least once.
		tempPtr is set to point to the input string to ensure it's not NULL, this is important for error checking after the loop. */
		for(i=0, c= 'a', tempPtr = inputStr; (c != EOF) && (c != '\n'); )
		{
			c = getchar();

			/* If the input string has been filled, realloate it to have double the space.
			Print error and break if there isn't enough memory.*/
			if(i == (strLen - 1))
			{
				strLen *= 2;
				tempPtr = (char*)realloc(inputStr, strLen * sizeof(char));
				if(tempPtr == NULL)
				{
					strLen /=2;
					printError("Memory allocation error, please use a smaller input string.\n");
					break;
				}
				else
					inputStr = tempPtr;
			}

			/* Store the character entered in the correct place in memory, if the end of the input hasn't been reached. */
			if((c != EOF) && (c != '\n'))
				{
					*(inputStr + i) = (char)c;
					i++;
				}
		}

		/* If there wasn't enough space for the string, try getting input again */
		if(tempPtr == NULL)
			continue;

		/* Null terminate string and validate the input */
		*(inputStr + i) = '\0';
		dateIn = validateInput(inputStr);
	}

	free(inputStr);
	return dateIn;
}

/*
	Function validateInput()
	Purpose: checks if the string passed to it contains a validly formatted date, converts the date held in a valid string to a date structure.
	Arguments: The string to process (passsed as a constant since the function is read only).
	Output: Error message to stderr if the user enters an invalid date.
	Return value: date structure, either containing the date held by the input string, or the day member as -1 for an invalid input.
*/
date validateInput(const char* string)
{
	/* output structure */
	date dateOut;

	/* Buffer for matching an extra character on end of input string. */
	char buffer;

	/* Checks for charcters other than digits, '-'s and '/'s in input
	Necessary to prevent whitespace in input being ignored. */
	int i;
	for(i=0; *(string + i) != '\0'; i++)
		if( !isdigit(*(string + i)) && (*(string + i) != '/') && (*(string + i) != '-') )
			return printError("Format is incorrect, please enter date again\n");

	/* Tries to assign variables from input string to integers with slashes. If not all variables are assigned it tries with dashes.
	If any extra characters (i.e extra '-'s or '/'s) are found on the end of the input string, it is counted as being invalid. */
	if (sscanf(string, "%d/%d/%d%1[/-]", &dateOut.day, &dateOut.month, &dateOut.year, &buffer) != 3)
		if (sscanf(string, "%d-%d-%d%1[/-]", &dateOut.day, &dateOut.month, &dateOut.year, &buffer) != 3)
			/* if not all variables can be assigned format is wrong, therefore print an error and return dateOut.day as -1 */
			return printError("Format is incorrect, please enter date again\n");

	/*	Now test day, month and year to see if they are in the valid range.
	Print error and return dateOut.day as -1 if invalid */
	if ((dateOut.year >= 10000) || (dateOut.year < 1))
		return printError("Year is invalid, please enter date again\n");

	if ((dateOut.month > 12) || (dateOut.month < 1))
		return printError("Month is invalid, please enter date again\n");

	/* Note: daysInEachMonth[dateOut.month] evaluates to the no. of days in that month */
	if ((dateOut.day > daysInEachMonth[dateOut.month]) || (dateOut.day < 1))
		return printError("Day is invalid, please enter date again\n");

	return dateOut;
}

/*
	Function: printError()
	Purpose: Prints the string passed to it to stderr, and returns a date structure with the error value that getInput() expects.
	Arguments: String to print.
	Return value: A date structure with the day element set to -1.
*/
date printError(char* string)
{
	date returnDate = {-1,0,0};
	fputs(string,stderr);
	return returnDate;
}

/*
	Function: countDays()
	Purpose: Calculates the number of days between the dates passed to it.
	Arguments: Two date structures to calculate the difference between.
	Return value: How many days after date1 date2 is (-ve if date2 is before date1).
*/
int countDays(date date1, date date2)
{
	/* Difference method works by calculating the "day number" (i.e number of days since 0/0/0) of each date and subtracting them.
	These variables hold the "day numbers" */
	int date1number = dayNumber(date1);
	int date2number = dayNumber(date2);
	return date2number - date1number;
}

/*
	Function: dayNumber()
	Purpose: Calculates the "day number" (i.e number of days since 0/0/0) of a date.
	Arguments: Date to calculate day no. of.
	Return value: Day number of date given.
*/
int dayNumber(date dateToCalc)
{
	return (365 * (dateToCalc.year)) + cumulativeDaysTilEachMonth[dateToCalc.month] + dateToCalc.day;
}

