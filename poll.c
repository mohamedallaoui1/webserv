/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   poll.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ayylaaba <ayylaaba@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/17 11:10:36 by ayylaaba          #+#    #+#             */
/*   Updated: 2024/02/17 13:45:51 by ayylaaba         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>     // for fprintf()
#include <unistd.h>    // for close()
#include <sys/epoll.h> // for epoll_create1()

int main()
{
	int epoll_fd = epoll_create1(0);
	
	if (epoll_fd == -1) {
		fprintf(stderr, "Failed to create epoll file descriptor\n");
		return (1);
	}
    printf ("fd <=== %d ===>\n", epoll_fd);
	if (close(epoll_fd)) {
		fprintf(stderr, "Failed to close epoll file descriptor\n");
		return (1);
	}
	return (0);
}