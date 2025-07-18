/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sep_opt_arg.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/21 21:16:17 by kuzyilma          #+#    #+#             */
/*   Updated: 2025/07/16 20:58:00 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
** OPERATOR SEPARATION AND ARGUMENT PROCESSING
** ============================================================================
** 
** This file implements operator separation functionality that automatically
** adds spaces around shell operators to ensure proper parsing. It processes
** the raw command line input and reformats it so that operators like pipes,
** redirections, and logical operators are properly separated from adjacent
** text, enabling reliable tokenization by the main parser.
** 
** OPERATOR DETECTION AND SPACING:
** ===============================
** The system recognizes and processes the following operators:
** - "||" : Logical OR (conditional execution)
** - "&&" : Logical AND (conditional execution) 
** - "<<" : Here document (input redirection)
** - ">>" : Append output redirection
** - "|"  : Pipe (command chaining)
** - ">"  : Output redirection
** - "<"  : Input redirection
** 
** AUTOMATIC SPACING ALGORITHM:
** ============================
** For each detected operator, the system automatically:
** 1. Adds a space before the operator if the preceding character isn't a space
** 2. Adds a space after the operator if the following character isn't a space
** 3. Preserves existing spaces to avoid double-spacing
** 4. Respects quote boundaries to avoid modifying quoted content
** 
** Example transformations:
** - "cmd>file"     → "cmd > file"
** - "cmd1|cmd2"    → "cmd1 | cmd2" 
** - "cmd<<EOF"     → "cmd << EOF"
** - "cmd1&&cmd2"   → "cmd1 && cmd2"
** 
** QUOTE AWARENESS:
** ===============
** The separator respects shell quoting rules:
** - Single quotes ('): Content is treated literally
** - Double quotes ("): Content is treated literally  
** - Operators inside quotes are not modified
** - Quote state is tracked throughout processing
** 
** BUFFER SAFETY IMPROVEMENTS:
** ===========================
** Fixed critical buffer access violations:
** - Added bounds checking for array access operations
** - Prevents reading before allocated memory (buffer underrun)
** - Prevents reading beyond allocated memory (buffer overrun)
** - Ensures safe access to preceding and following characters
*/

#include "../../main/minishell.h"

static const char	*get_opt(int indx)
{
	static char	*operator[] = {"||", "&&", "<<", ">>", "|", ">", "<"};

	return (operator[indx]);
}

static int	get_inquote(char *un_sep, int size)
{
	int		i;
	char	inquote;

	i = 0;
	inquote = 0;
	while (i <= size)
	{
		inquote ^= (un_sep[i] == '\"' && inquote != 2);
		inquote ^= 2 * (un_sep[i] == '\'' && inquote != 1);
		i++;
	}
	return (inquote);
}

/**
 * @brief Calculate required buffer size for spaced operator output
 * 
 * This function analyzes the input string to determine how much additional
 * space will be needed when operators are properly spaced. It counts all
 * operator occurrences that require spacing and calculates the total buffer
 * size needed for the reformatted string.
 * 
 * BUFFER SIZE CALCULATION:
 * =======================
 * For each operator found outside quotes:
 * - Add 1 if preceding character needs a space
 * - Add 1 if following character needs a space  
 * - Account for multi-character operators (<<, >>, ||, &&)
 * 
 * SAFETY IMPROVEMENTS:
 * ===================
 * Fixed buffer underrun vulnerability:
 * - Added bounds check: (i > 0) before accessing un_sep[i - 1]
 * - Added null terminator check before accessing following characters
 * - Prevents reading before or beyond allocated memory
 * 
 * @param un_sep Input string to analyze for operator spacing requirements
 * @return int Total buffer size needed including original content and spacing
 * 
 * @note Function is now safe from buffer underrun/overrun attacks
 * @note Respects quote boundaries when counting operators
 */
static int	get_sep_size(char *un_sep)
{
	int	i;
	int	extra;
	int	j;

	i = 0;
	extra = 0;
	while (un_sep[i] != '\0')
	{
		j = 0;
		while (get_inquote(un_sep, i) == 0 && j < 7)
		{
			if (ft_strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				extra += (i > 0 && un_sep[i - 1] != ' ') + \
				(un_sep[i + 1 + (j < 4)] != '\0' && un_sep[i + 1 + (j < 4)] != ' ');
				break ;
			}
			j++;
		}
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
	return (i + extra + 1);
}

/**
 * @brief Apply operator spacing to create properly formatted command string
 * 
 * This function performs the actual transformation of the input string by
 * adding appropriate spaces around shell operators. It builds the output
 * string character by character, inserting spaces where needed to ensure
 * operators are properly separated from adjacent text.
 * 
 * SPACING APPLICATION ALGORITHM:
 * =============================
 * For each character in the input:
 * 1. Check if current position starts an operator (outside quotes)
 * 2. If operator found:
 *    - Add space before operator if preceding char isn't space
 *    - Copy the operator to output
 *    - Add space after operator if following char isn't space
 * 3. If not an operator, copy character as-is
 * 
 * SAFETY IMPROVEMENTS:
 * ===================
 * Fixed critical buffer access vulnerabilities:
 * - Added bounds check: (i > 0) before accessing un_sep[i - 1]
 * - Added null check before accessing un_sep[i + 1 + offset]
 * - Prevents buffer underrun when i = 0
 * - Prevents buffer overrun at string boundaries
 * 
 * @param sep Output buffer to receive the spaced string
 * @param un_sep Input string to process for operator spacing
 * @param sep_size Size of the output buffer to prevent overflow
 * 
 * @note Function is now safe from buffer access violations
 * @note Maintains quote awareness throughout processing
 */
static void	set_sep(char *sep, char *un_sep, int sep_size)
{
	int	i;
	int	offset;
	int	j;

	i = 0;
	offset = 0;
	while (un_sep[i] != '\0')
	{
		j = -1;
		while (get_inquote(un_sep, i) == 0 && ++j < 7)
		{
			if (ft_strncmp(&(un_sep[i]), get_opt(j), 1 + (j < 4)) == 0)
			{
				if (i > 0 && un_sep[i - 1] != ' ')
					sep[i + offset++] = ' ';
				ft_strlcat(sep, get_opt(j), sep_size);
				if (un_sep[i + 1 + (j < 4)] != '\0' && un_sep[i + 1 + (j < 4)] != ' ')
					sep[i + 1 + (j < 4) + offset++] = ' ';
				break ;
			}
		}
		if (j > 6 || get_inquote(un_sep, i) != 0)
			sep[i + offset] = un_sep[i];
		i += 1 + (j < 4 && get_inquote(un_sep, i) == 0);
	}
}

void	sep_opt_arg(t_shell *shell)
{
	char	*temp;
	int		sep_size;

	temp = shell->current_input;
	sep_size = get_sep_size(temp);
	if ((size_t)sep_size == ft_strlen(shell->current_input) + 1)
		return ;
	shell->current_input = (char *)ft_calloc(sep_size, sizeof(char));
	set_sep(shell->current_input, temp, sep_size);
	free(temp);
}
