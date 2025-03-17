
#include "../main/minishell.h"
#include "stdarg.h"

void print_strings(char **arr) {
    if (!arr) return; // Check for NULL pointer

    for (int i = 0; arr[i] != NULL; i++) { // Iterate until NULL terminator
        printf("%s\n", arr[i]);
    }
}

void print_split(t_split *split) {
    if (!split || !split->start) return; // Check for NULL pointer

    for (int i = 0; i < split->size; i++) { // Iterate until split.size
        if (split->start[i] != NULL)
            printf("%s\n", split->start[i]);
        else
            printf("(null)\n"); // Indicate NULL values
    }
}

//free's and puts NULL into the given intex in split
void free_index(int index, t_split *split)
{
    free(split->start[index]);
    split->start[index] = NULL;
}
//takes in split, and amouth of indexes you want to free and NULL
//it takes as many indexes(int) as amounth and frees and puts NULL on all of them 
void free_indexes(t_split *split, int ac, char **av)
{
    while (ac > 2)
    {
        free_index(atoi(av[ac - 1]), split);
        ac--;
    }

}

//to complie use: (make sure libft.a exists)
//cc t_split_utils/test.c t_split_utils/t_split_utils.c e-libft/libft.a -o test
int main(int ac, char **av)
{
    if (ac < 2)
    {
        printf("please enter a string with indexes you want to remove");
        return 0;
    }
    printf("Test 1: Create and print split\n");
    t_split split = create_split_str(av[1]);
    print_strings(split.start);
    printf("--------------------------\n");

    printf("Test 2: Free given indexes\n");
    free_indexes(&split, ac, av);
    print_strings(split.start);
    printf("--------------------------\n");

    // **New test: Print all elements BEFORE rearranging**
    printf("Test 2.5: Print after free (before rearrange)\n");
    print_split(&split);
    printf("--------------------------\n");

    printf("Test 3: Rearranging split after freeing indexes\n");
    rearrange_split(&split);
    print_strings(split.start);
    printf("--------------------------\n");

    printf("Test 4: Free everything and print again\n");
    free_split(&split);
    print_strings(split.start); // Should not print anything

    return 0;
}