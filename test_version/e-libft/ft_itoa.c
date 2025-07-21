/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emgenc <emgenc@student.42istanbul.com.t    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/27 12:48:36 by emgenc            #+#    #+#             */
/*   Updated: 2024/10/27 12:48:37 by emgenc           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_digits(int n)
{
	int	digits;

	digits = 0;
	if (n <= 0)
		digits++;
	while (n != 0)
	{
		n = n / 10;
		digits++;
	}
	return (digits);
}

static void	ft_fill_array(int size, int offset, int n, char *array)
{
	while (size > offset)
	{
		array[size - 1] = n % 10 + '0';
		n = n / 10;
		size--;
	}
}

char	*ft_itoa(int n)
{
	int		offset;
	int		digits;
	char	*arr;

	offset = 0;
	digits = ft_digits(n);
	arr = (char *)malloc(sizeof(char) * digits + 1);
	if (!arr)
		return (0);
	if (n == -2147483648)
	{
		arr[0] = '-';
		arr[1] = '2';
		n = 147483648;
		offset = 2;
	}
	if (n < 0)
	{
		arr[0] = '-';
		offset = 1;
		n = -n;
	}
	ft_fill_array(digits, offset, n, arr);
	arr[digits] = '\0';
	return (arr);
}
