CC = c++

CXXFLAGS = -Wall -Wextra -Werror -MMD -MP -g -std=c++98

NAME = ircserv

RM = rm -rf

OBJS_DIR = objects/

SRCS =	main.cpp \
		server.cpp \
		string_utils.cpp

OBJS = $(addprefix $(OBJS_DIR), $(SRCS:.cpp=.o))

DEPS = $(OBJS:.o=.d)

$(NAME): $(OBJS)
	$(CC) $(CXXFLAGS) $(OBJS) -o $(NAME)

all: $(NAME)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

$(OBJS_DIR)%.o: %.cpp | $(OBJS_DIR)
	$(CC) $(CXXFLAGS) -o $@ -c $<

clean:
	$(RM) $(OBJS)
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: all clean fclean re
