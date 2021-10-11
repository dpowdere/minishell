# ************************************************************************** #
#                                                                            #
#                                                        :::      ::::::::   #
#   Makefile                                           :+:      :+:    :+:   #
#                                                    +:+ +:+         +:+     #
#   By: ngragas <ngragas@student.21-school.ru>     +#+  +:+       +#+        #
#                                                +#+#+#+#+#+   +#+           #
#   Created: 2021/10/11 17:36:54 by ngragas           #+#    #+#             #
#   Updated: 202#    Updated: 2021/10/11 18:41:08 by ngragas          ###   ########.fr        #
                                                           #
# ************************************************************************** #

NAME	=	minishell
CFLAGS	= 	-Wall -Wextra -Werror -MMD -g
SRC		=	main		\
			setup		\
			readline	\
			parse		\
			execute

SRC_DIR	=	sources/
INC_DIR	=	includes/
OBJ_DIR	=	objects/

SRC		:=	$(addsuffix .c, $(SRC))
OBJ		=	$(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEP		=	$(OBJ:.o=.d)

LIB_DIR	=	libft/
LIB		=	$(LIB_DIR)libft.a

all:
	$(MAKE) $(NAME) -j8
$(LIB): FORCE
	$(MAKE) -C $(LIB_DIR)
$(NAME): $(LIB) $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ -lft -L$(LIB_DIR)
$(OBJ): | $(OBJ_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)%.o: $(SRC_DIR)%.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@ -I$(INC_DIR) -I$(LIB_DIR)
-include $(DEP)

clean:
	$(RM) $(OBJ)
	$(RM) $(DEP)
	$(RM)r $(OBJ_DIR)
	$(MAKE) -C $(LIB_DIR) $@
fclean: clean
	$(RM) $(NAME)
	$(RM) $(LIB)
re: fclean all

norm:
	@norminette $(SRC_DIR)* $(INC_DIR)*
.PHONY: FORCE all clean fclean re norm
FORCE:
