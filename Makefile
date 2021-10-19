# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/11 19:10:04 by ngragas           #+#    #+#              #
#    Updated: 2021/10/19 19:10:26 by dpowdere         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	minishell
SRC		=	main			\
			env				\
			signals			\
			readline		\
			get_tokens		\
			check_tokens	\
			execute			\
			utils

SRC_DIR	=	sources/
INC_DIR	=	includes/
OBJ_DIR	=	objects/

SRC		:=	$(addsuffix .c, $(SRC))
OBJ		=	$(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEP		=	$(OBJ:.o=.d)

LIB_DIR	=	libft/
LIB		=	$(LIB_DIR)libft.a

RL_LIB_DIR	:=	$(HOME)/.brew/opt/readline/lib
RL_INC_DIR	:=	$(HOME)/.brew/opt/readline/include

CPPFLAGS	:=	-MMD -I$(INC_DIR) -I$(LIB_DIR) -I$(RL_INC_DIR)
CFLAGS		:=	-Wall -Wextra -Werror -g #-fsanitize=address
LDFLAGS		:=	-L$(LIB_DIR) -L$(RL_LIB_DIR)
LDLIBS		:=	-lft -lreadline

all:
	$(MAKE) $(NAME) -j8
$(LIB): FORCE
	$(MAKE) -C $(LIB_DIR)
$(NAME): $(LIB) $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)
$(OBJ): | $(OBJ_DIR)
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)
$(OBJ_DIR)%.o: $(SRC_DIR)%.c Makefile
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@
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
	@norminette $(SRC_DIR)* $(INC_DIR)* $(LIB_DIR)*.[ch]

install-deps:
	brew install readline
.PHONY: FORCE all clean fclean install-deps re norm
FORCE:
