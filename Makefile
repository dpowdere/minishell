# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ngragas <ngragas@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/11 19:10:04 by ngragas           #+#    #+#              #
#    Updated: 2021/10/22 23:25:08 by dpowdere         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME	=	minishell
SRC		=	main			\
			env				\
			signals			\
			readline		\
			get_tokens		\
			check_tokens	\
			get_raw_cmds	\
			get_cooked_cmd	\
			execute			\
			free			\
			debug_tokens	\
			debug_cmds		\
			utils

SRC_DIR	=	sources/
INC_DIR	=	includes/
OBJ_DIR	=	objects/

SRC		:=	$(addsuffix .c, $(SRC))
OBJ		=	$(addprefix $(OBJ_DIR), $(SRC:.c=.o))
DEP		=	$(OBJ:.o=.d)

LIB_DIR	=	libft/
LIB		=	$(LIB_DIR)libft.a

HOMEBREW_PREFIX := $(shell brew config | grep HOMEBREW_PREFIX | cut -d' ' -f2)
RL_LIB_DIR	:=	$(HOMEBREW_PREFIX)/opt/readline/lib
RL_INC_DIR	:=	$(HOMEBREW_PREFIX)/opt/readline/include

CPPFLAGS	:=	-MMD -I$(INC_DIR) -I$(LIB_DIR) -I$(RL_INC_DIR)
CFLAGS		:=	-Wall -Wextra -Werror -g3 #-fsanitize=address
LDFLAGS		:=	-L$(LIB_DIR) -L$(RL_LIB_DIR)
LDLIBS		:=	-lft -lreadline

ifneq ($(findstring linux,$(shell $(CC) -dumpmachine)),)
  ON_LINUX	:=	1
  CPPFLAGS	:=	-MMD -I$(INC_DIR) -I$(LIB_DIR)
  LDFLAGS	:=	-L$(LIB_DIR)
endif

ifdef DEBUG_TOKENS
  CPPFLAGS	+=	-DDEBUG_TOKENS=$(DEBUG_TOKENS)
endif
ifdef DEBUG_RAW_CMDS
  CPPFLAGS	+=	-DDEBUG_RAW_CMDS=$(DEBUG_RAW_CMDS)
endif
ifdef DEBUG_COOKED_CMDS
  CPPFLAGS	+=	-DDEBUG_COOKED_CMDS=$(DEBUG_COOKED_CMDS)
endif

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
ifdef ON_LINUX
	sudo apt-get install -y libreadline-dev
else
	brew install readline
endif
.PHONY: FORCE all clean fclean install-deps re norm
FORCE:
