# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2020/10/04 20:29:14 by ngragas           #+#    #+#              #
#    Updated: 2021/11/01 23:10:41 by ngragas          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	= libft.a
CFLAGS	= -Wall -Wextra -Werror
HEADER	= libft.h
SRCS	=	ft_isspace.c	ft_isprint.c	ft_isascii.c					\
			ft_isdigit.c	ft_isalpha.c	ft_isalnum.c					\
			ft_toupper.c	ft_tolower.c	ft_strmapi.c					\
			ft_strlen.c		ft_strncmp.c	ft_memcmp.c						\
			ft_memchr.c		ft_strchr.c		ft_strrchr.c	ft_strnstr.c	\
			ft_strlcpy.c	ft_memcpy.c		ft_memccpy.c	ft_memmove.c	\
			ft_bzero.c		ft_memset.c										\
			ft_minmax.c														\
																			\
			ft_calloc.c														\
			ft_ptr_malloc.c		ft_ptr_free.c			ft_ptr_assign.c		\
			ft_ptr_array_len.c	ft_ptr_array_free.c							\
																			\
			ft_strdup.c		ft_substr.c		ft_strlcat.c	ft_strjoin.c	\
			ft_strtrim.c	ft_split.c		ft_atoi.c		ft_itoa.c		\
			ft_putchar_fd.c	ft_putstr_fd.c	ft_putendl_fd.c	ft_putnbr_fd.c	\
																			\
			ft_lstnew.c		ft_lstadd_front.c	ft_lstadd_back.c			\
			ft_lstremove.c	ft_lstremoveif.c	ft_lstpop.c					\
			ft_lstdelone.c	ft_lstclear.c									\
			ft_lstsort.c	ft_lstreverse.c		ft_lstsize.c				\
			ft_lstiter.c	ft_lstmap.c			ft_lstlast.c				\
			ft_lstreduce.c	ft_lstpipeline.c								\
			ft_lstinsert.c	ft_lstdetach.c									\
			ft_lst_to_ptr_array.c											\
																			\
			ft_wchrto8.c		ft_wstrto8.c	ft_write.c					\
			ft_file_exists.c	ft_basename.c								\
			get_next_line.c

OBJS	= $(SRCS:.c=.o)

all: $(NAME)
clean:
	@$(RM) $(OBJS)
fclean: clean
	@$(RM) $(NAME)
re: fclean all
norm:
	norminette
$(OBJS): $(HEADER)
$(NAME): $(OBJS)
	$(AR) rcs $(NAME) $?
.PHONY: all clean fclean re norm
