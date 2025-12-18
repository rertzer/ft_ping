CC := cc
FLAGS := -Wall -Wextra -Werror -O3 -g3

NAME := ft_ping 

SRC_DIR := src/

OBJ_DIR := obj/

INC_DIR := includes/
INC := -I ../includes/

SOURCES := \
	exit.c \
	help.c \
	host.c \
	icmp.c \
	parsing.c \
	ping.c \
	signals.c \
	socket.c \
	stats.c \
	main.c

OBJ := $(SOURCES:.c=.o)
OBJS := $(addprefix $(OBJ_DIR), $(OBJ))
DEPS := $(OBJS:.o=.d)

all: $(NAME)

$(NAME): $(OBJ_DIR) $(OBJS)
	$(CC) $(FLAGS) -o $@ $(OBJS) -lm
	@echo 'setting raw socket capability'
	@setcap cap_net_raw=pe $@ || echo "You need to be root!"

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
