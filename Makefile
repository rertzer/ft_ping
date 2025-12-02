CC := cc
FLAGS := -Wall -Wextra -Werror -o3

NAME := ft_ping 

SRC_DIR := src/

OBJ_DIR := obj/

INC_DIR := includes/
INC := -I ../includes/

SOURCES := \
	parsing.c \
	main.c

OBJ := $(SOURCES:.c=.o)
OBJS := $(addprefix $(OBJ_DIR), $(OBJ))
DEPS := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.c
	$(CC) $(FLAGS) -c -MMD $< -o $@ -I $(INC_DIR) 

$(OBJ_DIR):
	mkdir  $(OBJ_DIR)

clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -fd $(OBJ_DIR)

fclean: clean 
	rm -f $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
