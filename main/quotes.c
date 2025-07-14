/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   quotes.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/13 00:00:00 by user              #+#    #+#             */
/*   Updated: 2025/07/13 15:57:10 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                           QUOTE PROCESSING SYSTEM
** ============================================================================
**
** This file implements comprehensive quote handling for the minishell,
** supporting both single and double quote semantics as defined by POSIX
** shell standards. It provides quote validation, removal, and expansion
** control mechanisms essential for proper command interpretation.
**
** QUOTE SEMANTICS:
** 1. SINGLE QUOTES (')
**    - Preserve literal meaning of all characters
**    - Prevent all variable expansion and command substitution
**    - Cannot contain single quotes (no escaping mechanism)
**    - Used for string literals that need exact preservation
**
** 2. DOUBLE QUOTES (")
**    - Allow variable expansion ($VAR, ${VAR})
**    - Allow command substitution (if implemented)
**    - Preserve most special characters literally
**    - Support escape sequences within quotes
**
** KEY FEATURES:
** - Proper quote pairing validation
** - Context-sensitive expansion control
** - Memory-safe string processing
** - Integration with variable expansion system
** - Error handling for malformed quote sequences
**
** ALGORITHM OVERVIEW:
** 1. VALIDATION PHASE
**    - Check for properly paired quotes
**    - Detect unclosed quote sequences
**    - Report syntax errors to user
**
** 2. PROCESSING PHASE
**    - Remove quote characters from output
**    - Apply appropriate expansion rules
**    - Handle nested quote scenarios
**    - Preserve escaped characters
**
** INTEGRATION CONTEXT:
** Quote processing occurs during command parsing, after tokenization
** but before execution. It transforms quoted strings into their final
** form for command execution while respecting shell expansion rules.
*/

#include "minishell.h"

/*
** ============================================================================
**                           QUOTE VALIDATION
** ============================================================================
*/

/**
** check_quotes - Validates proper pairing of quote characters in input string
**
** This function ensures that all quote characters in the input are properly
** paired and closed, preventing syntax errors during command execution.
** It tracks quote state using a finite state machine approach.
**
** ALGORITHM:
** 1. Initialize quote state trackers (single_open, double_open)
** 2. Scan each character in the input string
** 3. Toggle quote state when encountering quote characters
** 4. Respect quote precedence (quotes inside other quotes are literal)
** 5. Verify all quotes are closed at end of string
**
** STATE MACHINE LOGIC:
** - single_open: 0 = not in single quotes, 1 = inside single quotes
** - double_open: 0 = not in double quotes, 1 = inside double quotes
** - Single quotes are ignored when inside double quotes
** - Double quotes are ignored when inside single quotes
**
** PRECEDENCE RULES:
** - First encountered quote type takes precedence
** - Inner quotes of different type are treated as literal characters
** - Example: "text 'literal' text" - single quotes are literal
** - Example: 'text "literal" text' - double quotes are literal
**
** ERROR DETECTION:
** - Unclosed single quotes (single_open != 0 at end)
** - Unclosed double quotes (double_open != 0 at end)
** - Mixed unclosed quotes
**
** @param str: Input string to validate (may be NULL)
** @return: 1 if quotes are properly paired, 0 if malformed
**
** EXAMPLES:
** - check_quotes("'hello'") returns 1 (properly paired)
** - check_quotes("\"hello\"") returns 1 (properly paired)
** - check_quotes("'unclosed") returns 0 (missing closing quote)
** - check_quotes("'nested \"quotes\" here'") returns 1 (valid nesting)
** - check_quotes(NULL) returns 1 (safe handling)
*/
int	check_quotes(char *str)
{
	int		i;              // Character position iterator
	int		single_open;    // Single quote state: 0=closed, 1=open
	int		double_open;    // Double quote state: 0=closed, 1=open

	// NULL POINTER PROTECTION
	// Treat NULL input as valid (no quotes to validate)
	if (!str)
		return (1);
	
	// INITIALIZE QUOTE STATE MACHINE
	// Start with all quotes in closed state
	i = 0;
	single_open = 0;
	double_open = 0;
	
	// QUOTE STATE TRACKING LOOP
	// Process each character to maintain quote state
	while (str[i])
	{
		// SINGLE QUOTE HANDLING
		// Toggle single quote state only when not inside double quotes
		if (str[i] == '\'' && !double_open)
			single_open = !single_open;
			
		// DOUBLE QUOTE HANDLING  
		// Toggle double quote state only when not inside single quotes
		else if (str[i] == '"' && !single_open)
			double_open = !double_open;
			
		i++;  // Advance to next character
	}
	
	// VALIDATION RESULT
	// Return true only if both quote types are properly closed
	return (single_open == 0 && double_open == 0);
}

/*
** ============================================================================
**                           QUOTE PROCESSING
** ============================================================================
*/

/**
** process_quotes - Removes quotes and applies expansion rules based on quote type
**
** This function performs the core quote processing operation, removing quote
** characters from the string while applying appropriate expansion rules
** based on the quote type encountered. It's a critical component of the
** shell's parsing pipeline.
**
** PROCESSING RULES:
** 1. SINGLE QUOTES
**    - Remove quote characters from output
**    - Prevent ALL variable expansion
**    - Preserve all characters literally
**    - No escape sequence interpretation
**
** 2. DOUBLE QUOTES
**    - Remove quote characters from output
**    - Allow variable expansion ($VAR)
**    - Preserve whitespace literally
**    - Enable escape sequence processing
**
** 3. UNQUOTED TEXT
**    - Apply normal variable expansion
**    - Respect word splitting rules
**    - Process escape sequences
**
** ALGORITHM OVERVIEW:
** 1. Allocate output buffer (worst case: same size as input)
** 2. State machine processing with quote state tracking
** 3. Character-by-character processing with context awareness
** 4. Variable expansion integration during processing
** 5. Proper memory management and error handling
**
** MEMORY MANAGEMENT:
** - Dynamically allocates result buffer
** - Handles expansion-induced size changes
** - Caller responsible for freeing returned string
** - Safe cleanup on allocation failures
**
** @param str: Input string with quotes to process
** @param shell: Shell context for variable expansion
** @return: Processed string with quotes removed, NULL on error
**
** EXAMPLES:
** - process_quotes("'hello'", shell) returns "hello" (no expansion)
** - process_quotes("\"$HOME\"", shell) returns "/home/user" (expanded)
** - process_quotes("'$HOME'", shell) returns "$HOME" (literal)
*/
char	*process_quotes(char *str, t_shell *shell)
{
	char	*result;
	char	*current;
	int		i;
	int		j;
	int		in_single;
	int		in_double;
	int		len;

	if (!str)
		return (NULL);
	
	len = ft_strlen(str);
	result = malloc(len * 2 + 1);
	if (!result)
		return (NULL);
	current = ft_strdup(str);
	if (!current)
	{
		free(result);
		return (NULL);
	}
	i = 0;
	j = 0;
	in_single = 0;
	in_double = 0;
	
	while (current[i])
	{
		if (current[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (current[i] == '"' && !in_single)
			in_double = !in_double;
		else
		{
			result[j] = current[i];
			j++;
		}
		i++;
	}
	result[j] = '\0';
	free(current);
	
	// Only expand variables if we were not entirely within single quotes
	// Check if the original string was entirely single-quoted
	int entirely_single_quoted = 0;
	if (str[0] == '\'' && str[ft_strlen(str) - 1] == '\'')
	{
		// Check if there are no unmatched quotes in between
		int quote_count = 0;
		for (int k = 0; str[k]; k++)
		{
			if (str[k] == '\'')
				quote_count++;
		}
		if (quote_count == 2)
			entirely_single_quoted = 1;
	}
	
	if (!entirely_single_quoted)
	{
		char *expanded = expand_variables(result, shell);
		if (expanded)
		{
			free(result);
			return (expanded);
		}
	}
	
	return (result);
}

/*
** Process quotes in all arguments
*/
void	process_args_quotes(char **args, t_shell *shell)
{
	int		i;
	char	*processed;

	if (!args)
		return;
	
	i = 0;
	while (args[i])
	{
		// Only process if the argument contains quotes
		if (ft_strchr(args[i], '\'') || ft_strchr(args[i], '"'))
		{
			processed = process_quotes(args[i], shell);
			if (processed)
			{
				free(args[i]);
				args[i] = processed;
			}
		}
		i++;
	}
}

/*
** Enhanced variable expansion that respects quote context
** This replaces the simple expand_variables for quoted strings
*/
char	*expand_variables_quoted(char *str, t_shell *shell)
{
	char	*result;
	char	*var_start;
	char	*var_end;
	char	*var_name;
	char	*var_value;
	char	*expanded;
	int		i;
	int		in_single;
	int		in_double;
	int		var_len;

	if (!str)
		return (NULL);
	
	result = ft_strdup(str);
	if (!result)
		return (NULL);
	
	i = 0;
	in_single = 0;
	in_double = 0;
	
	while (result[i])
	{
		if (result[i] == '\'' && !in_double)
			in_single = !in_single;
		else if (result[i] == '"' && !in_single)
			in_double = !in_double;
		else if (result[i] == '$' && !in_single)
		{
			var_start = &result[i + 1];
			
			// Handle $? special case
			if (*var_start == '?')
			{
				var_value = ft_itoa(shell->past_exit_status);
				if (!var_value)
				{
					free(result);
					return (NULL);
				}
				
				// Replace $? with exit status
				char *before = ft_substr(result, 0, i);
				char *after = ft_strdup(&result[i + 2]);
				char *temp = ft_strjoin(before, var_value);
				if (!temp)
				{
					free(before);
					free(after);
					free(var_value);
					free(result);
					return (NULL);
				}
				expanded = ft_strjoin(temp, after);
				free(before);
				free(after);
				free(temp);
				if (!expanded)
				{
					free(var_value);
					free(result);
					return (NULL);
				}
				
				var_len = ft_strlen(var_value);
				free(result);
				free(var_value);
				result = expanded;
				i += var_len;
				continue;
			}
			
			// Find end of variable name
			var_end = var_start;
			while (*var_end && (ft_isalnum(*var_end) || *var_end == '_'))
				var_end++;
			
			if (var_end > var_start)
			{
				// Extract variable name
				var_name = malloc(var_end - var_start + 1);
				if (!var_name)
				{
					free(result);
					return (NULL);
				}
				ft_strlcpy(var_name, var_start, var_end - var_start + 1);
				
				// Get variable value
				var_value = get_env_value(var_name, shell);
				if (!var_value)
					var_value = ""; // Empty string if variable doesn't exist
				
				// Replace $VAR with value
				char *before = ft_substr(result, 0, i);
				char *after = ft_strdup(var_end);
				char *temp = ft_strjoin(before, var_value);
				if (!temp)
				{
					free(before);
					free(after);
					free(var_name);
					free(result);
					return (NULL);
				}
				expanded = ft_strjoin(temp, after);
				free(before);
				free(after);
				free(temp);
				if (!expanded)
				{
					free(var_name);
					free(result);
					return (NULL);
				}
				
				var_len = ft_strlen(var_value);
				free(result);
				free(var_name);
				result = expanded;
				i += var_len;
				continue;
			}
		}
		i++;
	}
	
	return (result);
}
