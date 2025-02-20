
#include "t_split.h"
#include <string.h>

#define TESTSTR1 "g m infinite"
#define TESTSTR2 "the|cloud&&razer"
#define TESTSTR3 ""
#define TESTSTR4 "echo gminfinite | grep m && echo sucsess || echo fail"

//prints the result of the test
void print_result(t_split *test)
{
    int i;
    printf("\n\tsize: %d", test->size);
    printf("\n\tstart:%3p", test->start);
    i = 0;
    while(i <= test->size)
    {
        printf("\n\t\t%-2d:%s", i, test->start[i]);
        i++;
    }
    printf(" -> after last value\n");
}

//test for create_split_str, send 1 to test() function to call
void create_split_str_test(char *str)
{
    t_split test;

    test = create_split_str(str);
    print_result(&test);
    free_split(&test);
    printf("\n");
}

void create_split_test(char*str, int start, int size)
{
    t_split test;
    t_split temp;

    test = create_split_str(str);
    if (test.size - size - 1 > 0)
    {
        temp = create_split(&(test.start[0]), test.size - size - 1);
        print_result(&temp);
    }
    temp = create_split(&(test.start[start + 1]), size);
    print_result(&temp);
    free_split(&test);
    printf("\n");
}

void strnsplit_test(char *string, char *tofind)
{
    t_split test;

    test = create_split_str(string);
    print_result(&test);
    printf("value of strnsplit:%-2d", strnsplit(test, tofind));
    free_split(&test);
    printf("\n----------------------------------------------------------\n");
}

//the general test function
void test(int test_number, char *test_name, void *params)
{
    printf("test: %s", test_name);
    if(test_number == 1)
        create_split_str_test((char *)params);
    if( test_number == 2)
    {
        char **temp = (char**)params;
        create_split_test(temp[0], ft_atoi(temp[1]), ft_atoi(temp[2]));
    }
    if  (test_number == 3)
    {
        char **temp = params;
        strnsplit_test(temp[0], temp[1]);
    }
}

int main()
{
    //test for create_split_str
    /*  test(1, TESTSTR1, &(TESTSTR1));
        test(1, TESTSTR2, &(TESTSTR2));
        test(1, "empty string", &(""));
        test(1, TESTSTR4, &(TESTSTR4)); */

    //test for create_split
    /* {
        char *params[] = {TESTSTR1, "1", "1", NULL};
        test(2, TESTSTR1": cut at m", params);
    }
    {
        char *params[] = {TESTSTR4, "8", "2", NULL};
        test(2, TESTSTR4": cut at ||", params);
    }
    {
        char *params[] = {TESTSTR4, "10", "0", NULL};
        test(2, TESTSTR4": cut at fail", params);
    }
    {
        char *params[] = {TESTSTR4, "3", "7", NULL};
        test(2, TESTSTR4": cut at grep", params);
    } */
   
   //test for find_str_in_split
    /* char *params[] = {TESTSTR1, "m",NULL};
        test(3, TESTSTR1": searched \"m\"", params);
    params[1] = "finite";
        test(3, TESTSTR1": searched \"finite\"", params);
    params[1] = "nite";
        test(3, TESTSTR1": searched \"nite\"", params);
    params[1] = "infinite",
        test(3, TESTSTR1": searched \"infinite\"", params);
    params[0] = TESTSTR4;
    params[1] = "||";
        test(3, TESTSTR1": searched \"||\"", params);
    params[1] = "";
        test(3, TESTSTR1": searched \"\"", params);
    params[1] = NULL;
        test(3, TESTSTR1": searched NULL", params);
    params[1] = "echo";
    test(3, TESTSTR4": searched \"echo\"", params); */
}